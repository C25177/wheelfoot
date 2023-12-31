#include "robot.h"

Robot		robot;
Tmotor	tmotor[4];
DJmotor djmotor[2];

//----
// @brief 初始化
//
//----
void robotInit() {
	TmotorInit(tmotor, 1);
	DJmotorInit(djmotor, 1);
	yesenseInit(&robot.yesense);

	legInit(&robot.legVir, LEGLEFT, NULL, NULL, NULL);
	legInit(&robot.legR, LEGRIGHT, &djmotor[0], &tmotor[0], &tmotor[1]);
	legInit(&robot.legL, LEGLEFT, &djmotor[1], &tmotor[2], &tmotor[3]);

	robot.flyflag = false;
	robot.mode		= ROBOTNORMAL;

	// TODO: 参数暂定 调节
	pidInit(&robot.yawpid, 1, 1, 1, 0, 1000, PIDPOS);
	pidInit(&robot.rollpid, 1, 1, 1, 0, 1000, PIDPOS);
	pidInit(&robot.splitpid, 80, 0, 1000, 0, 1000, PIDPOS);

	robot.L0Set							= 0.35;
	robot.yawpid.target			= 0;
	robot.rollpid.target		= 0;
	robot.splitpid.target		= 0;
	robot.legL.L0pid.target = robot.L0Set;
	robot.legR.L0pid.target = robot.L0Set;
}

//----
// @brief 状态量更新和一些检查
//
//----
void updateState() {
	//	float data[4];
	//	data[0] = robot.legL.front->monitor.received;
	//	data[1] = robot.legL.behind->monitor.received;
	//	data[2] = robot.legR.front->monitor.received;
	//	data[3] = robot.legR.behind->monitor.received;
	//	oscilloscope(data, 4);

	legUpdate(&robot.legL);
	legUpdate(&robot.legR);
	Zjie(&robot.legL, robot.yesense.pitch.now);
	Zjie(&robot.legR, robot.yesense.pitch.now);

	robot.legVir.dis.now = (robot.legL.dis.now + robot.legR.dis.now) / 2;
	robot.legVir.dis.dot = (robot.legL.dis.dot + robot.legR.dis.dot) / 2;
	robot.legVir.angle1	 = (robot.legL.angle1 + robot.legR.angle1) / 2;
	robot.legVir.angle4	 = (robot.legL.angle4 + robot.legR.angle4) / 2;

	Zjie(&robot.legVir, robot.yesense.pitch.now);

	flyCheck();
}

//----
// @brief lqr 控制保持平衡
//
//----
void balanceMode() {
	float L03 = pow(robot.legVir.L0.now, 3);
	float L02 = pow(robot.legVir.L0.now, 2);
	float L01 = pow(robot.legVir.L0.now, 1);

	//	if (robot.flyflag) {
	//		for (int col = 0; col < 6; col++) {
	//			for (int row = 0; row < 2; row++) {
	//				int num = (col << 1) + row;
	//				if (row == 1 && (col == 0 || col == 1))
	//					robot.legVir.K[row][col] = Kcoeff[num][0] * L03 + Kcoeff[num][1] * L02 + Kcoeff[num][2] * L01 + Kcoeff[num][3];
	//				else
	//					robot.legVir.K[row][col] = 0;
	//			}
	//		}
	//	} else {
	for (int col = 0; col < 6; col++) {
		for (int row = 0; row < 2; row++) {
			int num									 = (col << 1) + row;
			robot.legVir.K[row][col] = Kcoeff[num][0] * L03 + Kcoeff[num][1] * L02 + Kcoeff[num][2] * L01 + Kcoeff[num][3];
		}
	}
	//	}
	robot.legVir.X.theta		 = robot.legVir.angle0.now;
	robot.legVir.X.thetadot	 = robot.legVir.angle0.dot;
	robot.legVir.X.x				 = robot.legVir.dis.now;
	robot.legVir.X.v				 = robot.legVir.dis.dot;
	robot.legVir.X.pitch		 = robot.yesense.pitch.now;
	robot.legVir.X.pitchdot	 = robot.yesense.pitch.dot;

	robot.legVir.Xd.theta		 = 0;
	robot.legVir.Xd.thetadot = 0;
	robot.legVir.Xd.x				 = 0;
	robot.legVir.Xd.v				 = 0;
	robot.legVir.Xd.pitch		 = 0;
	robot.legVir.Xd.pitchdot = 0;

	robot.legVir.U.Twheel		 = robot.legVir.K[0][0] * (robot.legVir.Xd.theta - robot.legVir.X.theta) +
													robot.legVir.K[0][1] * (robot.legVir.Xd.thetadot - robot.legVir.X.thetadot) +
													robot.legVir.K[0][2] * (robot.legVir.Xd.x - robot.legVir.X.x) +
													robot.legVir.K[0][3] * (robot.legVir.Xd.v - robot.legVir.X.v) +
													robot.legVir.K[0][4] * (robot.legVir.Xd.pitch - robot.legVir.X.pitch) +
													robot.legVir.K[0][5] * (robot.legVir.Xd.pitchdot - robot.legVir.X.pitchdot);
	robot.legVir.U.Tp = robot.legVir.K[1][0] * (robot.legVir.Xd.theta - robot.legVir.X.theta) +
											robot.legVir.K[1][1] * (robot.legVir.Xd.thetadot - robot.legVir.X.thetadot) +
											robot.legVir.K[1][2] * (robot.legVir.Xd.x - robot.legVir.X.x) +
											robot.legVir.K[1][3] * (robot.legVir.Xd.v - robot.legVir.X.v) +
											robot.legVir.K[1][4] * (robot.legVir.Xd.pitch - robot.legVir.X.pitch) +
											robot.legVir.K[1][5] * (robot.legVir.Xd.pitchdot - robot.legVir.X.pitchdot);

	robot.legL.TWheelset	 = robot.legVir.U.Twheel / 2;
	robot.legR.TWheelset	 = robot.legVir.U.Twheel / 2;

	robot.legL.Tpset			 = robot.legVir.U.Tp / 2;
	robot.legR.Tpset			 = robot.legVir.U.Tp / 2;

	// 前馈力
	robot.legL.Fset				 = FFEEDFORWARD;
	robot.legR.Fset				 = FFEEDFORWARD;
	// 补偿虚拟力
	float lfCompensate		 = robot.legL.L0pid.compute(&robot.legL.L0pid, robot.legL.L0.now);
	float rfCompensate		 = robot.legR.L0pid.compute(&robot.legR.L0pid, robot.legR.L0.now);
	robot.legL.Fset				-= lfCompensate;
	robot.legR.Fset				-= rfCompensate;
	// // 旋转补偿
	// float yawCompensate		 = 0;	 // robot.yawpid->compute(robot.yawpid, robot.yesense.yaw.now);
	// robot.legL.TWheelset -= yawCompensate;
	// robot.legR.TWheelset += yawCompensate;
	// 劈腿补偿
	float splitCompensate	 = robot.splitpid.compute(&robot.splitpid, robot.legL.angle0.now - robot.legR.angle0.now);
	robot.legL.Tpset			+= splitCompensate;
	robot.legR.Tpset			-= splitCompensate;
	// // 翻滚角补偿
	// float rollCompensate	 = 0;	 // robot.rollpid->compute(robot.rollpid, robot.yesense.roll.now);
	// robot.legL.Fset			-= rollCompensate;
	// robot.legR.Fset			+= rollCompensate;

	VMC(&robot.legL);
	VMC(&robot.legR);

	// 方向
	robot.legL.TWheelset								*= robot.legL.dir;
	robot.legL.TFset										*= robot.legL.dir;
	robot.legL.TBset										*= robot.legL.dir;

	robot.legR.TWheelset								*= robot.legR.dir;
	robot.legR.TFset										*= robot.legR.dir;
	robot.legR.TBset										*= robot.legR.dir;

	robot.legR.front->set.torque				 = robot.legR.TFset;
	robot.legR.behind->set.torque				 = robot.legR.TBset;
	robot.legR.wheel->set.torque				 = robot.legR.TWheelset;

	robot.legL.front->set.torque				 = robot.legL.TFset;
	robot.legL.behind->set.torque				 = robot.legL.TBset;
	robot.legL.wheel->set.torque				 = robot.legL.TWheelset;

	robot.legL.front->monitor.received	 = 0;
	robot.legL.behind->monitor.received	 = 0;
	robot.legR.front->monitor.received	 = 0;
	robot.legR.behind->monitor.received	 = 0;
}

//----
// @brief 跳跃 可以先用简单的位控来实现跳跃
//
//----
void jumpMode() {
	// TODO: 待做 再研究一下子 可以使用贝塞尔曲线来做？还是直接做线性差值？
}

//----
// @brief 宕机模式 实际上就是电机全部设置0电流
//
//----
void haltMode() {
	tmotor[0].monitor.mode	= HALT;
	tmotor[1].monitor.mode	= HALT;
	tmotor[2].monitor.mode	= HALT;
	tmotor[3].monitor.mode	= HALT;
	djmotor[0].monitor.mode = HALT;
	djmotor[1].monitor.mode = HALT;
}

//----
// @brief 腾空检测
//
//----
void flyCheck() {
	// 要计算轮子所受的地面的支持力，也就是FN，且FN=F+Gw 其中F应当时轮子给机体的支持力，其实就是地面给的支持力用于轮子的重力和上层机构的支持力，而对于lp是机体对轮子的力，所以要反向
	float lp							 = robot.legL.Fnow * cos(robot.legL.angle0.now) + robot.legL.Tpnow * sin(robot.legL.angle0.now) / robot.legL.L0.now;
	float rp							 = robot.legR.Fnow * cos(robot.legR.angle0.now) + robot.legR.Tpnow * sin(robot.legR.angle0.now) / robot.legR.L0.now;

	float zmdd						 = robot.yesense.accelz * cos(robot.yesense.pitch.now) - robot.yesense.accelx * sin(robot.yesense.pitch.now);

	float lzwdd						 = zmdd - robot.legL.L0.ddot * cos(robot.legL.angle0.now) + 2 * robot.legL.L0.dot + robot.legL.angle0.dot * sin(robot.legL.angle0.now) + robot.legL.L0.now * robot.legL.angle0.ddot * sin(robot.legL.angle0.now) + robot.legL.L0.now * robot.legL.angle0.dot * robot.legL.angle0.dot * cos(robot.legL.angle0.now);
	float rzwdd						 = zmdd - robot.legR.L0.ddot * cos(robot.legR.angle0.now) + 2 * robot.legR.L0.dot + robot.legR.angle0.dot * sin(robot.legR.angle0.now) + robot.legR.L0.now * robot.legR.angle0.ddot * sin(robot.legR.angle0.now) + robot.legR.L0.now * robot.legR.angle0.dot * robot.legR.angle0.dot * cos(robot.legR.angle0.now);

	robot.legR.normalforce = -rp + MASSWHEEL * (GRAVITY + rzwdd);
	robot.legL.normalforce = -lp + MASSWHEEL * (GRAVITY + lzwdd);

	float force						 = (robot.legL.normalforce + robot.legR.normalforce) / 2;

	if (force < FORCETHRESHOLD)
		robot.flyflag = true;
	else
		robot.flyflag = false;
}

//----
// @brief 开始运行
//
//----
void robotRun() {
	switch (robot.mode) {
		case ROBOTNORMAL:
			balanceMode();
			break;
		case ROBOTJUMP:
			jumpMode();
			break;
		case ROBOTHALT:
			haltMode();
			break;
		default:
			break;
	}
	// 将会运行所有的tmotor和djmotor
	TmotorRun(tmotor);
	DJmotorRun(djmotor);
}
