#include "usart3.h"    


/*******************************************************************************  
* 函 数 名         : uart3_init  
* 函数功能         : IO端口及串口3，时钟初始化函数   B10,B11     
* 输    入         : 无  
* 输    出         : 无  
*******************************************************************************/    
void usart3_Init(u32 bt)    
{    
    USART_InitTypeDef USART_InitStructure;    
  NVIC_InitTypeDef NVIC_InitStructure;     
    GPIO_InitTypeDef GPIO_InitStructure;    //声明一个结构体变量，用来初始化GPIO    
   //使能串口的RCC时钟    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //使能UART3所在GPIOB的时钟    
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    

   //串口使用的GPIO口配置    
   // Configure USART2 Rx (PB.11) as input floating      
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    

   // Configure USART2 Tx (PB.10) as alternate function push-pull    
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    

   //配置串口    
   USART_InitStructure.USART_BaudRate = bt;    
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
   USART_InitStructure.USART_StopBits = USART_StopBits_1;    
   USART_InitStructure.USART_Parity = USART_Parity_No;    
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    


   // Configure USART3     
   USART_Init(USART3, &USART_InitStructure);//配置串口3    

  // Enable USART1 Receive interrupts 使能串口接收中断    
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);    
   //串口发送中断在发送数据时开启    
   //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);    

   // Enable the USART3     
   USART_Cmd(USART3, ENABLE);//使能串口3    

   //串口中断配置    
   //Configure the NVIC Preemption Priority Bits       
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);    

   // Enable the USART3 Interrupt     
   NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;    
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
   NVIC_Init(&NVIC_InitStructure);    

}    


/*******************************************************************************  
* 函 数 名         : uart3_send_char  
* 函数功能         : 串口3发送一字节        
* 输    入         : 无  
* 输    出         : 无  
*******************************************************************************/    
void uart3_send_char(u8 temp)         
{    
    USART_SendData(USART3,(u8)temp);        
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);         
}    


/*******************************************************************************  
* 函 数 名         : uart3_send_buff  
* 函数功能         : 串口3发送一字符串       
* 输    入         : 无  
* 输    出         : 无  
*******************************************************************************/    
void uart3_send_buff(u8 buf[],u32 len)    
{    
    u32 i;    
    for(i=0;i<len;i++)    
    uart3_send_char(buf[i]);            
}  

void USART3_IRQHandler(void)  
{  
    USART_ClearFlag(USART3,USART_FLAG_TC);  
    if(USART_GetITStatus(USART3,USART_IT_RXNE)!=Bit_RESET)//检查指定的USART中断发生与否  
    {    
        u8  k=USART_ReceiveData(USART3);  
        uart3_send_char(k+1);    
    }    
}  
