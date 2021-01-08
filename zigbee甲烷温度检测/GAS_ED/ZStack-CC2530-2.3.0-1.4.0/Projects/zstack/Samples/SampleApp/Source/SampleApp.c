#include <stdio.h>
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "MT_APP.h"
#include "MT.h"
#include "ds18b20.h"

/*********************************************************************
 * MACROS
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr)[0])

#define BEEP P1_4
#define GLED P1_5
#define RLED P1_6

#define HAL_ADC_DEC_064     0x00    /* Decimate by 64 : 8-bit resolution */
#define HAL_ADC_DEC_128     0x10    /* Decimate by 128 : 10-bit resolution */
#define HAL_ADC_DEC_256     0x20    /* Decimate by 256 : 12-bit resolution */
#define HAL_ADC_DEC_512     0x30    /* Decimate by 512 : 14-bit resolution */
#define HAL_ADC_DEC_BITS    0x30    /* Bits [5:4] */
#define HAL_ADC_CHANNEL_6          0x06

#define uint unsigned int
#define uchar unsigned char

#define ADC_STOP()  st( ADCCON1 |= 0x30; )

// Macro for initiating a single sample in single-conversion mode (ADCCON1.STSEL = 11).
#define ADC_SAMPLE_SINGLE()       \
  st( ADC_STOP();                 \
      ADCCON1 |= 0x40; )

#define st(x)      do { x } while (__LINE__ == -1)
/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr; //广播
afAddrType_t SampleApp_Flash_DstAddr;    //组播
afAddrType_t SampleApp_P2P_DstAddr;      //点播

aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

uint8 TIM_conter = 0;
byte BEEP_Flag = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_SendPeriodicMessage( void );
void SampleApp_SendFlashMessage( uint16 flashTime );
void SampleApp_Send_P2P_Message(void);
uint ReadGasData( void );
void Waring(uint);
void DelayMS(uint msec);
void LCD_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[]);


/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
/****************************************************************************
* ADC读取函数
****************************************************************************/
uint ReadGasData( void )
{
  uint value = 0;
  long unsigned int AdcValue = 0;
  
  /* Enable channel */
  ADCCFG |= 0x40;
  
  /* writing to this register starts the extra conversion */
  //ADCCON3 = 0x86;// 0x87;
  ADCCON3 = 0xB6;
  for(int i=0; i<4; i++)
  {
    ADCCON3 = 0xB6;
    ADC_SAMPLE_SINGLE(); 
    while(!(ADCCON1 & 0x80));              //等待AD转换完成
    /*value =  ADCL >> 2;                  //ADCL寄存器低2位无效
    value |= (((uint)ADCH) << 6); */    
    value = ADCL>>4;                      
    value |= (((uint)ADCH) << 4);         
    AdcValue += value;  
  }
  AdcValue = AdcValue >> 2;                 //累加除以4，得到平均值
  if(AdcValue > 4090)   // 输入的电压0V时，因为会满量程，检测到AdcValue的12位值为0xfff
    AdcValue = 0;
  
  
  return (AdcValue);
}

void SampleApp_Init( uint8 task_id )
{ 
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
  MT_UartInit();                  //串口初始化
  MT_UartRegisterTaskID(task_id); //注册串口任务
  P0SEL &= 0x7f;                  //DS18B20的io口初始化 p0.7
  
  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;

  // Setup for the flash command's destination address - Group 1
  SampleApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  SampleApp_Flash_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Flash_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;

  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7 );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        // Received when a key is pressed
        case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if (  //(SampleApp_NwkState == DEV_ZB_COORD) ||
                 (SampleApp_NwkState == DEV_ROUTER)
              || (SampleApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending the periodic message in a regular interval.
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {
    // Send the periodic message
    SampleApp_SendPeriodicMessage();
    //SampleApp_Send_P2P_Message();

    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
        (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 )
  {
    /* This key sends the Flash Command is sent to Group 1.
     * This device will not receive the Flash Command from this
     * device (even if it belongs to group 1).
     */
    SampleApp_SendFlashMessage( SAMPLEAPP_FLASH_DURATION );
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    /* The Flashr Command is sent to Group 1.
     * This key toggles this device in and out of group 1.
     * If this device doesn't belong to group 1, this application
     * will not receive the Flash command sent to group 1.
     */
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  uint16 flashTime;
  byte buf[2];

  switch ( pkt->clusterId )
  {  
    case SAMPLEAPP_PERIODIC_CLUSTERID:
      
      osal_memset(buf, 0 , 2);
      osal_memcpy(buf, pkt->cmd.Data, 1); //复制数据到缓冲区中
   
      if(buf[0]=='w')      //判断收到的数据是否为"w" 
      {BEEP_Flag = 1; HalUARTWrite(0, "UNUSUAL", 5);}    //报警标志置1，提示接收成功
      /*HalUARTWrite(0, "TEMP:", 5);       //提示接收到数据
      HalUARTWrite(0, pkt->cmd.Data, pkt->cmd.DataLength);//输出接收到的数据
      HalUARTWrite(0, "%", 1);
      HalUARTWrite(0, "\r\n", 2);         // 回车换行*/
      break;

    case SAMPLEAPP_FLASH_CLUSTERID:
      flashTime = BUILD_UINT16(pkt->cmd.Data[1], pkt->cmd.Data[2] );
      HalLedBlink( HAL_LED_4, 4, 50, (flashTime / 4) );
      break;
  }
}

/*********************************************************************
 * @fn      SampleApp_SendPeriodicMessage
 *
 * @brief   Send the periodic message.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_SendPeriodicMessage( void )
{
  uchar waring[1] = "w";
  char str[4];
  char str1[5];
  char strTemp[12];
  char strGas[12];
  uchar TempGas[21];
  uchar EN_ID[5];
  
  float temp;
  uint Gas;

  temp = floatReadDs18B20();              //读取温度数据
  Gas  = ReadGasData();                   //模拟值读取函数
  
  if(Gas<200) Gas = 200;
  //if(Gas>300) BEEP_Flag = 1;
  if(Gas > 300)                                            //发送报警标志
  {
    if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       1,
                       waring,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
   {
      BEEP_Flag = 1;
   }
   else  BEEP_Flag = 1;;
  }
  
  sprintf(str, "%2.1f", temp);
  sprintf(str1, "%-5.2f", (Gas-200)/20.47);  //将浮点数转成字符串
  
  sprintf(EN_ID, "%x", NLME_GetShortAddr());
/*  EN_ID[0] = NLME_GetShortAddr()/16/16/16 + '0';
  EN_ID[1] = NLME_GetShortAddr()/16/16%16 + '0';
  EN_ID[2] = NLME_GetShortAddr()/16%16 + '0';
  EN_ID[3] = NLME_GetShortAddr()%16 + '0';*/
  EN_ID[4] = '\0';

  
  if(BEEP_Flag == 1)                         //判断标志位
  {RLED = !RLED; GLED = 0; BEEP = !BEEP;}
  else
  {RLED = 0; GLED = 1; BEEP = 1;}
  
  
  if(TIM_conter == 10)                   //5秒未接收则解除报警
  {
    BEEP_Flag = 0;
    TIM_conter = 0;
  }
  TIM_conter++;
  
/*  if(Gas>15)
  {RLED = !RLED; GLED = 0; BEEP = !BEEP;}
  else
  {RLED = 0; GLED = 1; BEEP = 1;}*/
       
/*  HalUARTWrite(0, "TEMP:", 5);           //串口输出提示信息
  HalUARTWrite(0, str, 4);
  HalUARTWrite(0, "   ",3);
  HalUARTWrite(0, "GAS:",4);
  HalUARTWrite(0, str1, 4);
  HalUARTWrite(0, "%", 1);
  HalUARTWrite(0, "\r\n",2);*/
  
  HalUARTWrite(0, EN_ID,5);
  

  osal_memcpy(strTemp, "TEMP:", 5);
  osal_memcpy(&strTemp[5], str, 4);
  osal_memcpy(&strTemp[9], " C\0", 3);
  
  osal_memcpy(strGas, "GAS :", 5);
  osal_memcpy(&strGas[5], str1, 5);
  osal_memcpy(&strGas[10], "%\0", 2);
  
  osal_memcpy(&TempGas[0], str, 4);
  osal_memcpy(&TempGas[4], " GAS:",5);
  osal_memcpy(&TempGas[9], str1, 5);
  osal_memcpy(&TempGas[14], "%", 1);
  osal_memcpy(&TempGas[15], " ", 1);
  osal_memcpy(&TempGas[16], EN_ID, 4);
  
  
  
  HalLcdWriteString(strTemp, HAL_LCD_LINE_3);
  HalLcdWriteString(strGas , HAL_LCD_LINE_4);//LCD显示
   
  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       20,
                       TempGas,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
  /*if(Gas > 300)                                            //发送报警标志
  {
    if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       1,
                       waring,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
   {
   }
  }*/
}

/*********************************************************************
 * @fn      SampleApp_SendFlashMessage
 *
 * @brief   Send the flash message to group 1.
 *
 * @param   flashTime - in milliseconds
 *
 * @return  none
 */
void SampleApp_SendFlashMessage( uint16 flashTime )
{
  uint8 buffer[3];
  buffer[0] = (uint8)(SampleAppFlashCounter++);
  buffer[1] = LO_UINT16( flashTime );
  buffer[2] = HI_UINT16( flashTime );

  if ( AF_DataRequest( &SampleApp_Flash_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_FLASH_CLUSTERID,
                       3,
                       buffer,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}

/*********************************************************************
 * @fn      SampleApp_Send_P2P_Message
 *
 * @brief   point to point.
 *
 * @param   none
 *
 * @return  none
 */
/*void SampleApp_Send_P2P_Message( void )
{
  char str[4];
  char str1[5];
  char strTemp[12];
  char strGas[12];
  char TempGas[14];
  float temp;
  int Gas;

  temp = floatReadDs18B20();              //读取温度数据
  Gas  = ReadGasData();                   //模拟值读取函数
  sprintf(str, "%4.1f", temp);
  sprintf(str1, "%-5.2f", (Gas-8)/1.28); //将浮点数转成字符串
  
  if(Gas>15)
  {RLED = !RLED; GLED = 0; BEEP = !BEEP;}
  else
  {RLED = 0; GLED = 1; BEEP = 1;}
       
  HalUARTWrite(0, "TEMP:", 5);           //串口输出提示信息
  HalUARTWrite(0, str, 4);
  HalUARTWrite(0, "   ",3);
  HalUARTWrite(0, "GAS:",4);
  HalUARTWrite(0, str1, 4);
  HalUARTWrite(0, "%", 1);
  HalUARTWrite(0, "\r\n",2);

  osal_memcpy(strTemp, "TEMP:", 5);
  osal_memcpy(&strTemp[5], str, 4);
  osal_memcpy(&strTemp[9], " C\0", 3);
  
  osal_memcpy(strGas, "GAS :", 5);
  osal_memcpy(&strGas[5], str1, 5);
  osal_memcpy(&strGas[10], "%\0", 2);
  
  osal_memcpy(&TempGas[0], str, 4);
  osal_memcpy(&TempGas[4], " GAS:",5);
  osal_memcpy(&TempGas[9], str1, 5);
  
  
  
  HalLcdWriteString(strTemp, HAL_LCD_LINE_3);
  HalLcdWriteString(strGas , HAL_LCD_LINE_4);//LCD显示
  
   
  if ( AF_DataRequest( &SampleApp_P2P_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_P2P_CLUSTERID,
                       14,
                       TempGas,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
  osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                     SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
}*/
/*********************************************************************
*********************************************************************/



