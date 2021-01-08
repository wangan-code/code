#include "usart3.h"    


/*******************************************************************************  
* �� �� ��         : uart3_init  
* ��������         : IO�˿ڼ�����3��ʱ�ӳ�ʼ������   B10,B11     
* ��    ��         : ��  
* ��    ��         : ��  
*******************************************************************************/    
void usart3_Init(u32 bt)    
{    
    USART_InitTypeDef USART_InitStructure;    
  NVIC_InitTypeDef NVIC_InitStructure;     
    GPIO_InitTypeDef GPIO_InitStructure;    //����һ���ṹ�������������ʼ��GPIO    
   //ʹ�ܴ��ڵ�RCCʱ��    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //ʹ��UART3����GPIOB��ʱ��    
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    

   //����ʹ�õ�GPIO������    
   // Configure USART2 Rx (PB.11) as input floating      
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    

   // Configure USART2 Tx (PB.10) as alternate function push-pull    
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    

   //���ô���    
   USART_InitStructure.USART_BaudRate = bt;    
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
   USART_InitStructure.USART_StopBits = USART_StopBits_1;    
   USART_InitStructure.USART_Parity = USART_Parity_No;    
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    


   // Configure USART3     
   USART_Init(USART3, &USART_InitStructure);//���ô���3    

  // Enable USART1 Receive interrupts ʹ�ܴ��ڽ����ж�    
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);    
   //���ڷ����ж��ڷ�������ʱ����    
   //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);    

   // Enable the USART3     
   USART_Cmd(USART3, ENABLE);//ʹ�ܴ���3    

   //�����ж�����    
   //Configure the NVIC Preemption Priority Bits       
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);    

   // Enable the USART3 Interrupt     
   NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;    
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
   NVIC_Init(&NVIC_InitStructure);    

}    


/*******************************************************************************  
* �� �� ��         : uart3_send_char  
* ��������         : ����3����һ�ֽ�        
* ��    ��         : ��  
* ��    ��         : ��  
*******************************************************************************/    
void uart3_send_char(u8 temp)         
{    
    USART_SendData(USART3,(u8)temp);        
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);         
}    


/*******************************************************************************  
* �� �� ��         : uart3_send_buff  
* ��������         : ����3����һ�ַ���       
* ��    ��         : ��  
* ��    ��         : ��  
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
    if(USART_GetITStatus(USART3,USART_IT_RXNE)!=Bit_RESET)//���ָ����USART�жϷ������  
    {    
        u8  k=USART_ReceiveData(USART3);  
        uart3_send_char(k+1);    
    }    
}  
