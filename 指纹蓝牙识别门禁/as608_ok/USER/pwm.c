
/*********************************************************

TIM3输出4路pwm,单独控制占空比

**********************************************************/

#include "pwm.h"

    //开启定时器3
//   




void TIM3_PWM_Init(u16 arr,u16 psc)
{
	
     //定义结构体
  	TIM_OCInitTypeDef TIM_OCInitStructure;
     TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
     GPIO_InitTypeDef GPIO_InitStructure;
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
		 //全部映射，将TIM3_CH2映射到PB5
		 //根据STM32中文参考手册2010中第第119页可知：
		 //当没有重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PA6，PA7,PB0,PB1
		 //当部分重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PB4，PB5,PB0,PB1
		 //当完全重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PC6，PC7,PC8,PC9
		 //也即是说，完全重映射之后，四个通道的PWM输出引脚分别为PC6，PC7,PC8,PC9，我们用到了通道1和通道2，所以对应引脚为PC6，PC7,PC8,PC9，我们用到了通道1和通道2，所以对应引脚为
		 //GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);  //完全重映射
		GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);  //部分重映射    //设置PC6、PC7、PC8、PC9为复用输出，输出4路PWM
	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_1;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
       //重新将Timer设置为缺省值
       TIM_DeInit(TIM3);
       //采用内部时钟给TIM3提供时钟源
       TIM_InternalClockConfig(TIM3);
     
     //预分频系数为0，即不进行预分频，此时TIMER的频率为72MHzre.TIM_Prescaler =0;
       TIM_TimeBaseStructure.TIM_Prescaler = arr;
     //设置计数溢出大小，每计20000个数就产生一个更新事件
       TIM_TimeBaseStructure.TIM_Period = psc;
       //设置时钟分割
       TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
       //设置计数器模式为向上计数模式
       TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
       //将配置应用到TIM3中
       TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
       //清除溢出中断标志
			 
       //设置缺省值
       TIM_OCStructInit(&TIM_OCInitStructure);
      
      //TIM3的CH1输出
			TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1; //设置是PWM模式还是比较模式 
			TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; //比较输出使能，使能PWM输出到端口
			TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High; //设置极性是高还是低
      //设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%
      TIM_OCInitStructure.TIM_Pulse =0;
      TIM_OC1Init(TIM3, &TIM_OCInitStructure);
      
      
      //TIM3的CH2输出
      TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1; //设置是PWM模式还是比较模式 
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; //比较输出使能，使能PWM输出到端口
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High; //设置极性是高还是低
      //设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%
      TIM_OCInitStructure.TIM_Pulse =0;
      TIM_OC2Init(TIM3, &TIM_OCInitStructure);
      
   
      //TIM3的CH3输出
      TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1; //设置是PWM模式还是比较模式 
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; //比较输出使能，使能PWM输出到端口
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High; //设置极性是高还是低
      //设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OC3Init(TIM3, &TIM_OCInitStructure);
        
      //TIM3的CH4输出
      TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1; //设置是PWM模式还是比较模式 
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; //比较输出使能，使能PWM输出到端口
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High; //设置极性是高还是低
      //设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OC4Init(TIM3, &TIM_OCInitStructure);
      
      //使能输出状态
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			
    
    //设置TIM3的PWM输出为使能
      TIM_CtrlPWMOutputs(TIM3,ENABLE); 
			TIM_Cmd(TIM3,ENABLE);
    
}



