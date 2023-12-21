#include "pid.h"

//----
// @brief ����ʽpid (�޷�)
//
// @param pid
// @param input ����
// @return float ���
//----
float incCompute(PID* pid, float input) {
	pid->err[0] = pid->target - input;
	pid->output = pid->kp * (pid->err[0] - pid->err[1]) + pid->ki * pid->err[0] + pid->kd * (pid->err[0] - 2 * pid->err[1] + pid->err[2]);
	pid->err[2] = pid->err[1];
	pid->err[1] = pid->err[0];
	limitInRange(float)(&pid->output, pid->outputLimit);
	return pid->output;
}
//----
// @brief λ��ʽpid (�޷�)
//
// @param pid
// @param input ����
// @return float ���
//----
float posCompute(PID* pid, float input) {
	pid->err[0]	 = pid->target - input;
	pid->accErr += pid->err[0];
	limitInRange(float)(&pid->accErr, pid->accErrLimit);
	pid->output = pid->kp * pid->err[0] + pid->ki * pid->accErr + pid->kd * (pid->err[0] - pid->err[1]);
	pid->err[2] = pid->err[1];
	pid->err[1] = pid->err[0];
	limitInRange(float)(&pid->output, pid->outputLimit);
	return pid->output;
}

//----
// @brief T�λ�������ʽPID
//
// @param pid
// @param input ����
// @return float ���
//----
float TincCompute(PID* pid, float input) {
	pid->err[0] = pid->target - input;
	float ierr	= (pid->err[0] + pid->err[1]) / 2;
	pid->output = pid->kp * (pid->err[0] - pid->err[1]) + pid->ki * ierr + pid->kd * (pid->err[0] - 2 * pid->err[1] + pid->err[2]);
	pid->err[2] = pid->err[1];
	pid->err[1] = pid->err[0];
	limitInRange(float)(&pid->output, pid->outputLimit);
	return pid->output;
}

//----
// @brief T�λ���λ��ʽPID
//
// @param pid
// @param input ����
// @return float ���
//----
float TposCompute(PID* pid, float input) {
	pid->err[0]	 = pid->target - input;
	pid->accErr += (pid->err[0] + pid->err[1]) / 2;
	limitInRange(float)(&pid->accErr, pid->accErrLimit);
	pid->output = pid->kp * pid->err[0] + pid->ki * pid->accErr + pid->kd * (pid->err[0] - pid->err[1]);
	pid->err[2] = pid->err[1];
	pid->err[1] = pid->err[0];
	limitInRange(float)(&pid->output, pid->outputLimit);
	return pid->output;
}

//----
// @brief ���ο��ƣ�ͬʱ����λ�ú��ٶ�
//
// @param pid
// @param input
// @param inputdot
// @return float
//----
float twiceIncCompute(PID* pid, float input, float inputdot) {
	pid->err[0]	 = pid->target - input;
	float errdot = 0 - inputdot;
	pid->output	 = pid->kp * pid->err[0] + pid->kd * errdot;
	if (pid->ki) {
		pid->accErr += inputdot;
		limitInRange(float)(&pid->accErr, pid->accErrLimit);
		pid->output += pid->ki * pid->accErr;
	}
	limitInRange(float)(&pid->output, pid->outputLimit);
	return pid->output;
}

//----
// @brief PID��ʼ��������kp,ki,kd ѡ��pidģʽ
//
// @param pid
// @param kp
// @param ki
// @param kd
// @param outputLimit
// @param accErrLimit
// @param mode
//----
void pidInit(PID* pid, float kp, float ki, float kd, float outputLimit, float accErrLimit, pidMode mode) {
	pid->kp					 = kp;
	pid->ki					 = ki;
	pid->kd					 = kd;
	pid->err[0]			 = 0;
	pid->err[1]			 = 0;
	pid->err[2]			 = 0;
	pid->outputLimit = outputLimit;
	pid->accErrLimit = accErrLimit;

	switch (mode) {
		case PIDINC:
			pid->compute = incCompute;
			break;
		case PIDPOS:
			pid->compute = posCompute;
			break;
		case PIDTINC:
			pid->compute = TincCompute;
			break;
		case PIDTPOS:
			pid->compute = TposCompute;
			break;
		default:
			pid->compute = incCompute;
			break;
	}
}
