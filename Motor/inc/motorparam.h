//----
// @file motorparam.h
// @author mask <beloved25177@126.com>
// @brief
// @version 1.0
// @date 2023-11-12
//
// @copyright Copyright (c) 2023
// @details 电机需要的一些通用的参数
//----
#pragma once

#include "stm32f4xx.h"

#include <stdbool.h>

//----
// @brief 电机模式
//
//----
typedef enum { HALT, POSITION, SPEED, TORQUE, POSSPEED } MotorMode;

//----
// @brief 监测电机状态
//
//----
typedef struct {
	bool stuck;
	bool enable;
	bool timeOut;
	bool stuckRealse;
	u8	 mode;
	u32	 stuckCnt;
	u32	 timeOutCnt;
} MotorMonitor;

//----
// @brief 电机数据
//
//----
#define MotorValueDefine(T) \
	typedef struct {          \
		T			torque;           \
		T			current;          \
		T			velocity;         \
		T			angleRad;         \
		float angleDeg;         \
	} MotorValue_##T;
#define MotorValue(T) MotorValue_##T
MotorValueDefine(float) MotorValueDefine(s16)
