#pragma once
#include "pid.h"
#include "yesense.h"
#include "robotparam.h"
#include "leg.h"

typedef struct {
	bool flyflag;

	Leg			legL;
	Leg			legR;
	Leg			legVir;
	Yesense yesense;

	// �����ĸ�PID��������Ϊ��
	PID yawpid;		// ���ٶȿ���
	PID rollpid;		// �����ǿ���
	PID splitpid;	// ˫���������

	RobotRunMode mode;

	float L0Set;	// �趨�ȳ���Ҳ���ǵ�ǰ�����ȵ�ƽ���ȳ�
	float xSet;
	float vSet;
} Car;
	
extern Car car;
void robotInit();
void updateState();
void balanceMode();
void jumpMode();
void haltMode();
void flyCheck();
void robotRun();
