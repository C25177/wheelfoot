#pragma once
//----
// @file leg.h
// @author mask <beloved25177@126.com>
// @brief
// @version 1.0
// @date 2023-11-12
//
// @copyright Copyright (c) 2023
//
//----

#pragma once

#include "datastruct.h"
#include "pid.h"
#include "robotparam.h"

#include <webots/motor.h>
#include <webots/position_sensor.h>

typedef struct {
	int dir;

	WbDeviceTag wheel;
	WbDeviceTag front;
	WbDeviceTag behind;

	WbDeviceTag wheelEncoder;
	WbDeviceTag frontEncoder;
	WbDeviceTag behindEncoder;

	Input			 X, Xd;
	Output		 U;
	datastruct angle0;
	datastruct L0;
	datastruct dis;
	// ����������ϵ�µ����꣬ԭ���������˵��д�����
	float			 angle1, angle2, angle3, angle4;	// �Ƕȼ���ֵ�Ͷ�ȡ������ʵֵ���Ǻ�ͼ�е�һһ��Ӧ
	float			 angle1set, angle4set;						// �Ƕ��趨ֵ�����ڳ�ʼ�Ƕ�֮�ϵ��趨ֵ

	float xa, ya;
	float xb, yb;
	float xc, yc;
	float xd, yd;
	float xe, ye;
	float Fnow;
	float Tpnow;
	float TFnow;
	float TBnow;
	float TWheelnow;
	float Fset;		// ������
	float Tpset;	// �ؽڴ�Ť��
	float TFset;
	float TBset;
	float TWheelset;
	float normalforce;	// ����Ի����˵�ʵ��֧����
	float K[2][6];
	PID		L0pid;	// ��������pid ���ȳ��Ŀ���
} Leg;

void legInit(Leg* leg, int dir);
void legUpdate(Leg* leg);
void Zjie(Leg* leg, float pitch);
void Njie(Leg* leg, float xc, float yc);
void VMC(Leg* leg);
void INVMC(Leg* leg);
