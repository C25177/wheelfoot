#pragma once

#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

extern unsigned int GlobalTimer;

void tim2Init();
void tim3Init();
