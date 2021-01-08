#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "as608.h"
#include "sys.h"
#include "timer.h"
#include <stdio.h>
#include "OLED_I2C.h"
#include "string.h"	 
#include "BEEP.h"


u8 open_door_str[]="123456";    //蓝牙开门密码

u8 open_door_flag=0; //开门标志位

u16 user_ID=0;  //保存指纹模块返回的用户ID，用于判断用户

void fig_init()
{
			while(PS_HandShake(&AS608Addr))  //与AS608模块握手
	 {
			delay_ms(400);         //指纹工作正常，需要0.4秒缓冲
		  OLED_Clear();//清屏	
		  OLED_ShowString(0,3,"finger init error");//测试6*8字符
			delay_ms(800);	  
		}
}

int check(u8 a[],u8 b[],u32 len)  //检查字符串是否相同，相同返回1，否者返回0
{
  int i;
  for(i=0;i<len;i++)
  {
		if(a[i]!=b[i])
		return 0;
		if(b[i]=='\n')
		return 0;
	}
  return 1;
}

 int main(void) 
 {	
  u8 i;
  vu16 falsh,reclen;
		u16 len;	 
	SystemInit();
	delay_init();
 	OLED_Init();
	 LED_Init();
	BEEP_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //   2组中断分组
	uart_Init(9600);              //串口一初始化     //用于串口printf信息
	usart2_Init(115200);            //串口二初始化     //用于与指纹模块通信
	fig_init();  										//as608指纹模块自检
	PS_StaGPIO_Init();           //初始化PA6   与TCH相连的引脚，高电平代表触摸屏感应  
  oled_139(); 							  //自定义显示
 
	 while(1)
	{	
		  press_FR();	          //刷指纹

		if(open_door_flag==1)   //指纹ok，就开门  
		{  
	  	switch (user_ID)
	   	{
		  	case 1 : for(i=15;i<18;i++)	 OLED_ShowCHinese(56+i*16,5,i); JDQ_init(); break;     
		  	case 2 : for(i=18;i<21;i++)	 OLED_ShowCHinese(56+i*13,5,i); JDQ_init();break;     
				case 3 : for(i=21;i<24;i++)	 OLED_ShowCHinese(56+i*11,5,i); JDQ_init();break;      //管理员	   //汉字提取到"codetab.h"的汉字中
				case 4 : for(i=24;i<27;i++)	 OLED_ShowCHinese(56+i*10,5,i);	JDQ_init();	break; 
				case 5 : for(i=27;i<30;i++)	 OLED_ShowCHinese(56+i*9,5,i);	JDQ_init(); break;    
				default :	OLED_ShowString(0,3,"error"); break;     //管理员	   //汉字提取到"codetab.h"的汉字中
    	}
			
			open_door_flag=0;

			oled_139();
			
		} 
if(USART_RX_STA&0x8000)
		{				
		
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			USART_RX_BUF[len]=0;	 	//加入结束符
			if(len==6)		//监测密码
			{ 
				if(check(USART_RX_BUF,open_door_str,len)==1)
				 JDQ_init();
			}
			USART_RX_STA=0;
	}
	
	}
}






