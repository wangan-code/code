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


u8 open_door_str[]="123456";    //������������

u8 open_door_flag=0; //���ű�־λ

u16 user_ID=0;  //����ָ��ģ�鷵�ص��û�ID�������ж��û�

void fig_init()
{
			while(PS_HandShake(&AS608Addr))  //��AS608ģ������
	 {
			delay_ms(400);         //ָ�ƹ�����������Ҫ0.4�뻺��
		  OLED_Clear();//����	
		  OLED_ShowString(0,3,"finger init error");//����6*8�ַ�
			delay_ms(800);	  
		}
}

int check(u8 a[],u8 b[],u32 len)  //����ַ����Ƿ���ͬ����ͬ����1�����߷���0
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //   2���жϷ���
	uart_Init(9600);              //����һ��ʼ��     //���ڴ���printf��Ϣ
	usart2_Init(115200);            //���ڶ���ʼ��     //������ָ��ģ��ͨ��
	fig_init();  										//as608ָ��ģ���Լ�
	PS_StaGPIO_Init();           //��ʼ��PA6   ��TCH���������ţ��ߵ�ƽ����������Ӧ  
  oled_139(); 							  //�Զ�����ʾ
 
	 while(1)
	{	
		  press_FR();	          //ˢָ��

		if(open_door_flag==1)   //ָ��ok���Ϳ���  
		{  
	  	switch (user_ID)
	   	{
		  	case 1 : for(i=15;i<18;i++)	 OLED_ShowCHinese(56+i*16,5,i); JDQ_init(); break;     
		  	case 2 : for(i=18;i<21;i++)	 OLED_ShowCHinese(56+i*13,5,i); JDQ_init();break;     
				case 3 : for(i=21;i<24;i++)	 OLED_ShowCHinese(56+i*11,5,i); JDQ_init();break;      //����Ա	   //������ȡ��"codetab.h"�ĺ�����
				case 4 : for(i=24;i<27;i++)	 OLED_ShowCHinese(56+i*10,5,i);	JDQ_init();	break; 
				case 5 : for(i=27;i<30;i++)	 OLED_ShowCHinese(56+i*9,5,i);	JDQ_init(); break;    
				default :	OLED_ShowString(0,3,"error"); break;     //����Ա	   //������ȡ��"codetab.h"�ĺ�����
    	}
			
			open_door_flag=0;

			oled_139();
			
		} 
if(USART_RX_STA&0x8000)
		{				
		
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			USART_RX_BUF[len]=0;	 	//���������
			if(len==6)		//�������
			{ 
				if(check(USART_RX_BUF,open_door_str,len)==1)
				 JDQ_init();
			}
			USART_RX_STA=0;
	}
	
	}
}






