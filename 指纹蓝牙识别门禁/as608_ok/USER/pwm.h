#ifndef _pwm_H
#define _pwm_H

#include "stm32f10x.h"

void TIM3_PWM_Init(u16 arr,u16 psc);

/*
	TIM3_PWM_Init(7999,8999);//方波频率1Hz
	TIM_SetCompare1(TIM3, 799);//占空比10%
*/

#endif
