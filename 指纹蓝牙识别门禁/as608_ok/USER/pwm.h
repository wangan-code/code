#ifndef _pwm_H
#define _pwm_H

#include "stm32f10x.h"

void TIM3_PWM_Init(u16 arr,u16 psc);

/*
	TIM3_PWM_Init(7999,8999);//����Ƶ��1Hz
	TIM_SetCompare1(TIM3, 799);//ռ�ձ�10%
*/

#endif
