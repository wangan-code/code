
/*********************************************************

TIM3���4·pwm,��������ռ�ձ�

**********************************************************/

#include "pwm.h"

    //������ʱ��3
//   




void TIM3_PWM_Init(u16 arr,u16 psc)
{
	
     //����ṹ��
  	TIM_OCInitTypeDef TIM_OCInitStructure;
     TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
     GPIO_InitTypeDef GPIO_InitStructure;
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
		 //ȫ��ӳ�䣬��TIM3_CH2ӳ�䵽PB5
		 //����STM32���Ĳο��ֲ�2010�еڵ�119ҳ��֪��
		 //��û����ӳ��ʱ��TIM3���ĸ�ͨ��CH1��CH2��CH3��CH4�ֱ��ӦPA6��PA7,PB0,PB1
		 //��������ӳ��ʱ��TIM3���ĸ�ͨ��CH1��CH2��CH3��CH4�ֱ��ӦPB4��PB5,PB0,PB1
		 //����ȫ��ӳ��ʱ��TIM3���ĸ�ͨ��CH1��CH2��CH3��CH4�ֱ��ӦPC6��PC7,PC8,PC9
		 //Ҳ����˵����ȫ��ӳ��֮���ĸ�ͨ����PWM������ŷֱ�ΪPC6��PC7,PC8,PC9�������õ���ͨ��1��ͨ��2�����Զ�Ӧ����ΪPC6��PC7,PC8,PC9�������õ���ͨ��1��ͨ��2�����Զ�Ӧ����Ϊ
		 //GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);  //��ȫ��ӳ��
		GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);  //������ӳ��    //����PC6��PC7��PC8��PC9Ϊ������������4·PWM
	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_1;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
       //���½�Timer����Ϊȱʡֵ
       TIM_DeInit(TIM3);
       //�����ڲ�ʱ�Ӹ�TIM3�ṩʱ��Դ
       TIM_InternalClockConfig(TIM3);
     
     //Ԥ��Ƶϵ��Ϊ0����������Ԥ��Ƶ����ʱTIMER��Ƶ��Ϊ72MHzre.TIM_Prescaler =0;
       TIM_TimeBaseStructure.TIM_Prescaler = arr;
     //���ü��������С��ÿ��20000�����Ͳ���һ�������¼�
       TIM_TimeBaseStructure.TIM_Period = psc;
       //����ʱ�ӷָ�
       TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
       //���ü�����ģʽΪ���ϼ���ģʽ
       TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
       //������Ӧ�õ�TIM3��
       TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
       //�������жϱ�־
			 
       //����ȱʡֵ
       TIM_OCStructInit(&TIM_OCInitStructure);
      
      //TIM3��CH1���
			TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1; //������PWMģʽ���ǱȽ�ģʽ 
			TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; //�Ƚ����ʹ�ܣ�ʹ��PWM������˿�
			TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High; //���ü����Ǹ߻��ǵ�
      //����ռ�ձȣ�ռ�ձ�=(CCRx/ARR)*100%��(TIM_Pulse/TIM_Period)*100%
      TIM_OCInitStructure.TIM_Pulse =0;
      TIM_OC1Init(TIM3, &TIM_OCInitStructure);
      
      
      //TIM3��CH2���
      TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1; //������PWMģʽ���ǱȽ�ģʽ 
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; //�Ƚ����ʹ�ܣ�ʹ��PWM������˿�
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High; //���ü����Ǹ߻��ǵ�
      //����ռ�ձȣ�ռ�ձ�=(CCRx/ARR)*100%��(TIM_Pulse/TIM_Period)*100%
      TIM_OCInitStructure.TIM_Pulse =0;
      TIM_OC2Init(TIM3, &TIM_OCInitStructure);
      
   
      //TIM3��CH3���
      TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1; //������PWMģʽ���ǱȽ�ģʽ 
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; //�Ƚ����ʹ�ܣ�ʹ��PWM������˿�
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High; //���ü����Ǹ߻��ǵ�
      //����ռ�ձȣ�ռ�ձ�=(CCRx/ARR)*100%��(TIM_Pulse/TIM_Period)*100%
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OC3Init(TIM3, &TIM_OCInitStructure);
        
      //TIM3��CH4���
      TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1; //������PWMģʽ���ǱȽ�ģʽ 
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; //�Ƚ����ʹ�ܣ�ʹ��PWM������˿�
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High; //���ü����Ǹ߻��ǵ�
      //����ռ�ձȣ�ռ�ձ�=(CCRx/ARR)*100%��(TIM_Pulse/TIM_Period)*100%
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OC4Init(TIM3, &TIM_OCInitStructure);
      
      //ʹ�����״̬
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			
    
    //����TIM3��PWM���Ϊʹ��
      TIM_CtrlPWMOutputs(TIM3,ENABLE); 
			TIM_Cmd(TIM3,ENABLE);
    
}



