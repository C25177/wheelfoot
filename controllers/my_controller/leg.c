#include "leg.h"

//----
// @brief ��ʼ��
//
// @param leg
// @param dir �ȵķ���
//----
void legInit(Leg* leg, int dir) {
	leg->Fset = -61.90455385;	 // �������趨ֵ�ĳ�ʼ��
	leg->dir	= dir;

	pidInit(&leg->L0pid, 2000, 50, 12000, 0, 1000, PIDPOS);

	datastructInit(&leg->dis, 0, 0, 0, 0);
	// TODO: L0 ��ʼλ�� angle0�ĳ�ʼֵ
	datastructInit(&leg->L0, 0, 0, 0, 0);
	datastructInit(&leg->angle0, 0, 0, 0, 0);

	leg->Fnow				 = leg->Fset;
	leg->Tpset			 = 0;
	leg->TFset			 = 0;
	leg->TBset			 = 0;
	leg->TWheelset	 = 0;
	leg->normalforce = leg->Fset;

	// �����
	switch (leg->dir) {
		case LEGLEFT:
			leg->front				 = wb_robot_get_device("MotorFL");
			leg->behind				 = wb_robot_get_device("MotorBL");
			leg->wheel				 = wb_robot_get_device("MotorWheelL");
			leg->frontEncoder	 = wb_robot_get_device("EncoderFL");
			leg->behindEncoder = wb_robot_get_device("EncoderBL");
			leg->wheelEncoder	 = wb_robot_get_device("EncoderWheelL");
			break;
		case LEGRIGHT:
			leg->front				 = wb_robot_get_device("MotorFR");
			leg->behind				 = wb_robot_get_device("MotorBR");
			leg->wheel				 = wb_robot_get_device("MotorWheelR");
			leg->frontEncoder	 = wb_robot_get_device("EncoderFR");
			leg->behindEncoder = wb_robot_get_device("EncoderBR");
			leg->wheelEncoder	 = wb_robot_get_device("EncoderWheelR");
			break;
		default:
			break;
	}
	wb_motor_enable_torque_feedback(leg->front, timestep);
	wb_motor_enable_torque_feedback(leg->behind, timestep);
	wb_motor_enable_torque_feedback(leg->wheel, timestep);
	wb_position_sensor_enable(leg->frontEncoder, timestep);
	wb_position_sensor_enable(leg->behindEncoder, timestep);
	wb_position_sensor_enable(leg->wheelEncoder, timestep);

	inputInit(&leg->X);
	inputInit(&leg->Xd);
	outputInit(&leg->U);
}

//---
// @brief �����Ȳ�״̬��
//
// @param leg
//---
void legUpdate(Leg* leg) {
	leg->dis.now	= wb_position_sensor_get_value(leg->wheelEncoder) * WHEELR;
	leg->dis.dot	= (leg->dis.now - leg->dis.last) / dt;
	leg->dis.last = leg->dis.now;
}

//----
// @brief ����L0��angle0��ԭֵ�͵���ֵ
//
// @param leg
// @param pitch ������ ��������ϵ��ת��
//----
void Zjie(Leg* leg, float pitch) {
	// TODO: ����Ƕ� ���Ӧ���� 1���Ҳ�Ӧ���� -1

	leg->xb			= l1 * cos(leg->angle1) - l5 / 2;
	leg->yb			= l1 * sin(leg->angle1);
	leg->xd			= l5 / 2 + l4 * cos(leg->angle4);
	leg->yd			= l4 * sin(leg->angle4);
	float lbd		= sqrt(pow((leg->xd - leg->xb), 2) + pow((leg->yd - leg->yb), 2));
	float A0		= 2 * l2 * (leg->xd - leg->xb);
	float B0		= 2 * l2 * (leg->yd - leg->yb);
	float C0		= pow(l2, 2) + pow(lbd, 2) - pow(l3, 2);
	float D0		= pow(l3, 2) + pow(lbd, 2) - pow(l2, 2);
	leg->angle2 = 2 * atan2((B0 + sqrt(pow(A0, 2) + pow(B0, 2) - pow(C0, 2))), (A0 + C0));
	leg->angle3 = PI - 2 * atan2((-B0 + sqrt(pow(A0, 2) + pow(B0, 2) - pow(D0, 2))), (A0 + D0));
	leg->xc			= leg->xb + l2 * cos(leg->angle2);
	leg->yc			= leg->yb + l2 * sin(leg->angle2);

	leg->L0.now = sqrt(pow(leg->xc, 2) + pow(leg->yc, 2));

	// ����pitch����ת����
	float cor_XY_then[2][1];
	cor_XY_then[0][0]		= cos(pitch) * leg->xc - sin(pitch) * leg->yc;
	cor_XY_then[1][0]		= sin(pitch) * leg->xc + cos(pitch) * leg->yc;
	leg->angle0.now			= atan2(cor_XY_then[0][0], cor_XY_then[1][0]);

	leg->L0.dot					= (leg->L0.now - leg->L0.last) / dt;
	leg->L0.ddot				= (leg->L0.dot - leg->L0.lastdot) / dt;
	leg->L0.last				= leg->L0.now;
	leg->L0.lastdot			= leg->L0.dot;
	leg->angle0.dot			= (leg->angle0.now - leg->angle0.last) / dt;
	leg->angle0.ddot		= (leg->angle0.dot - leg->angle0.lastdot) / dt;
	leg->angle0.last		= leg->angle0.now;
	leg->angle0.lastdot = leg->angle0.dot;
}

//----
// @brief �Ȳ��ṹ��⣬���ڽ�����Ŷ����Ķ�Ӧ�ĵ���Ƕ�
//
// @param leg
// @param xc
// @param yc
//----
void Njie(Leg* leg, float xc, float yc) {
	float m, n, b, x1, y1;
	float A, B, C;

	A							 = 2 * l1 * yc;
	B							 = 2 * l1 * (xc + l5 / 2);
	C							 = l2 * l2 - l1 * l1 - xc * xc - yc * yc - l5 * l5 / 4 + xc * l5;
	leg->angle1set = 2 * atan((A + sqrt(A * A + B * B - C * C)) / (B - C));
	if (leg->angle1set < 0)
		leg->angle1set += 2 * PI;
	m								= l1 * cos(leg->angle1set);
	n								= l1 * sin(leg->angle1set);
	b								= 0;
	x1							= ((xc - m) * cos(b) - (yc - n) * sin(b)) + m;
	y1							= ((xc - m) * sin(b) + (yc - n) * cos(b)) + n;	// �õ�������˶˵������

	A								= 2 * y1 * l4;
	B								= 2 * l4 * (x1 - l5 / 2);
	C								= l3 * l3 + l5 * x1 - l4 * l4 - l5 * l5 / 4 - x1 * x1 - y1 * y1;
	leg->angle4set	= 2 * atan((A - sqrt(A * A + B * B - C * C)) / (B - C));

	leg->angle1set -= InitAngle1;
	leg->angle4set	= InitAngle4 - leg->angle4set;
}

//----
// @brief VMC ���㣬������->ʵ�ʵ��Ť��
//
// @param leg
//----
void VMC(Leg* leg) {
	float trans[2][2] = { l1 * cos(leg->angle0.now + leg->angle3) * sin(leg->angle1 - leg->angle2) / sin(leg->angle2 - leg->angle3),
												l1 * sin(leg->angle0.now + leg->angle3) * sin(leg->angle1 - leg->angle2) / (leg->L0.now * sin(leg->angle2 - leg->angle3)),
												l4 * cos(leg->angle0.now + leg->angle2) * sin(leg->angle3 - leg->angle4) / sin(leg->angle2 - leg->angle3),
												l4 * sin(leg->angle0.now + leg->angle2) * sin(leg->angle3 - leg->angle4) / (leg->L0.now * sin(leg->angle2 - leg->angle3)) };
	leg->TFset				= trans[0][0] * leg->Fset + trans[0][1] * leg->Tpset;
	leg->TBset				= trans[1][0] * leg->Fset + trans[1][1] * leg->Tpset;
}

// TODO: û���Թ���һ��û����
//----
// @brief ����VMC���� ʵ�ʵ��Ť��->������
//
// @param leg
//----
void INVMC(Leg* leg) {
	float trans[2][2] = { -(sin(leg->angle0.now + leg->angle2) * sin(leg->angle2 - leg->angle3)) /
													(l1 * cos(leg->angle0.now + leg->angle2) * sin(leg->angle0.now + leg->angle3) * sin(leg->angle1 - leg->angle2) -
													 l1 * cos(leg->angle0.now + leg->angle3) * sin(leg->angle0.now + leg->angle2) * sin(leg->angle1 - leg->angle2)),
												(sin(leg->angle0.now + leg->angle3) * sin(leg->angle2 - leg->angle3)) /
													(l4 * cos(leg->angle0.now + leg->angle2) * sin(leg->angle0.now + leg->angle3) * sin(leg->angle3 - leg->angle4) -
													 l4 * cos(leg->angle0.now + leg->angle3) * sin(leg->angle0.now + leg->angle2) * sin(leg->angle3 - leg->angle4)),
												(leg->L0.now * cos(leg->angle0.now + leg->angle2) * sin(leg->angle2 - leg->angle3)) /
													(l1 * cos(leg->angle0.now + leg->angle2) * sin(leg->angle0.now + leg->angle3) * sin(leg->angle1 - leg->angle2) -
													 l1 * cos(leg->angle0.now + leg->angle3) * sin(leg->angle0.now + leg->angle2) * sin(leg->angle1 - leg->angle2)),
												-(leg->L0.now * cos(leg->angle0.now + leg->angle3) * sin(leg->angle2 - leg->angle3)) /
													(l4 * cos(leg->angle0.now + leg->angle2) * sin(leg->angle0.now + leg->angle3) * sin(leg->angle3 - leg->angle4) -
													 l4 * cos(leg->angle0.now + leg->angle3) * sin(leg->angle0.now + leg->angle2) * sin(leg->angle3 - leg->angle4)) };

	leg->Fnow					= (trans[0][0] * leg->TFnow + trans[0][1] * leg->TBnow);
	leg->Tpnow				= (trans[1][0] * leg->TFnow + trans[1][1] * leg->TBnow);
}
