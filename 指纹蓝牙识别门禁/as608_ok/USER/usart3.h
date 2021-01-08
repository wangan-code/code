
#ifndef __USART3_H_  
#define __USART3_H_  

#include "stm32f10x.h"  



/******************************************************************************* 
* 函 数 名         : uart3_init 
* 函数功能         : IO端口及串口3，时钟初始化函数       
* 输    入         : 无 
* 输    出         : 无 
*******************************************************************************/  
void usart3_Init(u32 bt);  



/******************************************************************************* 
* 函 数 名         : uart3_send_char 
* 函数功能         : 串口3发送一字节       
* 输    入         : 无 
* 输    出         : 无 
*******************************************************************************/  
void uart3_send_char(u8 temp);  



/******************************************************************************* 
* 函 数 名         : uart3_send_buff 
* 函数功能         : 串口3发送一字符串      
* 输    入         : 无 
* 输    出         : 无 
*******************************************************************************/  
void uart3_send_buff(u8 buf[],u32 len);



#endif
