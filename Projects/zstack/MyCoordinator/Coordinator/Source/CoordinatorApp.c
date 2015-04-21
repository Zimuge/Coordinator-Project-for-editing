/**************************************************************************************************
  Filename:       CoordinatorApp.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Coordinator Application (no Profile).


  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software mdsay not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "CoordinatorVariable.h"
#include "CoordinatorAF.h"
#include "CoordinatorApp.h"

/*********************************************************************
 * MACROS
 */

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


// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in CoordinatorApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this Coordinator app it is define in RAM.


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */
void CoordinatorApp_CMDSerial(mtOSALSerialData_t *Msg);   // 接收串口数据并处理
/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      CoordinatorApp_Init
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
void CoordinatorApp_Init( uint8 task_id )
{
  CoordinatorApp_TaskID = task_id;
  CoordinatorApp_NwkState = DEV_INIT;
  CoordinatorApp_TransID = 0;
  
  MT_UartInit();
  MT_UartRegisterTaskID(task_id);
  PANID =  _NIB.nwkPanId;
  
  
  //for debug use
  /*uint8 panid;  
  panid = (uint8)((PANID >> 8) & 0x00ff);
  HalUARTWrite(0,&panid,1);
  panid = (uint8)((PANID) & 0x00ff);
  HalUARTWrite(0,&panid,1);*/

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in CoordinatorAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  CoordinatorApp_Broadcast_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  CoordinatorApp_Broadcast_DstAddr.endPoint = CoordinatorAPP_ENDPOINT;
  CoordinatorApp_Broadcast_DstAddr.addr.shortAddr = 0xFFFF;

  // Setup for the flash command's destination address - Group 1
  CoordinatorApp_Group_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  CoordinatorApp_Group_DstAddr.endPoint = CoordinatorAPP_ENDPOINT;
  CoordinatorApp_Group_DstAddr.addr.shortAddr = GroupId;
  
  CoordinatorApp_P2PDstAddr.addrMode = (afAddrMode_t)afAddr16Bit;
  CoordinatorApp_P2PDstAddr.endPoint = CoordinatorAPP_ENDPOINT;
  CoordinatorApp_P2PDstAddr.addr.shortAddr = 0x0000;
  
  // Fill out the endpoint description.
  CoordinatorApp_epDesc.endPoint = CoordinatorAPP_ENDPOINT;
  CoordinatorApp_epDesc.task_id = &CoordinatorApp_TaskID;
  CoordinatorApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&CoordinatorApp_SimpleDesc;
  CoordinatorApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &CoordinatorApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( CoordinatorApp_TaskID );

  // By default, all devices start out in Group 1
  CoordinatorApp_Group.ID = GroupId;//0x0001
  osal_memcpy( CoordinatorApp_Group.name, "Group 1", 7  );
  aps_AddGroup( CoordinatorAPP_ENDPOINT, &CoordinatorApp_Group );
}

/*********************************************************************
 * @fn      CoordinatorApp_ProcessEvent
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
uint16 CoordinatorApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( CoordinatorApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event)
      {
      case CMD_SERIAL_MSG :
           CoordinatorApp_CMDSerial((mtOSALSerialData_t *)MSGpkt);
           break;
        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          CoordinatorApp_HandleAfIncomingMessage( MSGpkt );
          break;
        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          CoordinatorApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ((CoordinatorApp_NwkState == DEV_ZB_COORD))
          {
           // NLME_RestoreFromNV();
            // tell the arm the Personal Area Net with PANID is created 
            uint8 str[6] = {0xff,0xff,0xff,0xff};
            PANID =  _NIB.nwkPanId;
            str[4] = (uint8)((PANID >> 8) & 0x00ff);
            str[5] = (uint8)((PANID) & 0x00ff);
            CoordinatorWriteUART(str,6);
            // Start count the tick_alive of the AddrList
      
            osal_start_timerEx( CoordinatorApp_TaskID,
                              CoordinatorAPP_TICK_ALIVE_MSG_EVT,
                             6000);
            
          }
          else
          {
           //  no existing on the network
          }
          break;
         

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( CoordinatorApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  
  if(events & CoordinatorAPP_TICK_ALIVE_MSG_EVT)
  {
    uint8 i = 0;
    for(i = 0; i < DEVICEID_MAX; i++)
    {
        if(AddrList[i].tick_alive == 0)
        {
            AddrList[i].status = FALSE;
            AddrList[i].tick_alive = 0;
            AddrList[i].addr = 0x0000;
        }
        else
        {
             AddrList[i].tick_alive--;
             AddrList[i].status = TRUE;
        }
    }
    osal_start_timerEx( CoordinatorApp_TaskID,
                              CoordinatorAPP_TICK_ALIVE_MSG_EVT,
                             6000); 
  }
  
  //  (the rest event trrigered by change PANID  ).
  if ( events & CoordinatorAPP_REST_MSG_EVT)
  {
     SystemReset();
    //return (events ^ CoordinatorAPP_SEND_PERIODIC_MSG_EVT);
  }
  
  // Discard unknown events
  return 0;
}


/*********************************************************************
 * @fn      CoordinatorApp_CMDSerial
 *
 * @brief   Send the message from the UART.
 *
 * @param   none
 *
 * @return  none
 */
void CoordinatorApp_CMDSerial(mtOSALSerialData_t * Msg)
{
  /***************for the serial use***********************/
 /* uint8 i = 0;
  uint8 SerialDataLen = Msg->msg[0];
  uint8 *str = &(Msg->msg[1]);
  uint8 len = str[0];//get the command data length
  uint8 id = str[1];// get the current id of the device 
  uint8 command = str[2];*/
  
   /***************for the arm use***********************/
 // uint8 i = 0;
  uint8 *str = &(Msg->msg[0]);
  uint8 len = str[0];//get the command data length
  uint8 id = str[1];// get the current id of the device 
  uint8 command = str[2];
  CoordinatorApp_P2PDstAddr.addr.shortAddr = AddrList[id].addr;// get the addres of the device id
  switch (command)
  {
      case CMD_SET_DEVICEID: 
        if(len == 1)
        {
         // DevList[str[3]] = DevList[id];
          //CoordinatorApp_SendP2PMessage(COORDINATOR_COMMAND_CLUSTERID,4,str);
        //  HalUARTWrite(0,(uint8 *)(&DevList[id]),2);
        }
        break;
    case CMD_SET_PANID:
      {
        if(len == 1)
        {
          if(PANID == ((uint16) str[3]));
          else
          {
           PANID = str[3];
          // tell or the enddevice that the PanId will be changed 
          CoordinatorApp_SendBroadcastMessage(COORDINATOR_COMMAND_CLUSTERID,4,str);
          _NIB.nwkPanId = PANID;
          NLME_UpdateNV(0x01);
          osal_start_timerEx(CoordinatorApp_TaskID,
                              CoordinatorAPP_REST_MSG_EVT ,
                           CoordinatorAPP_REST_MSG_TIMEOUT);
          
           } 
        }
      }
      default:
        if((len == 1) && (id <= DEVICEID_MAX))
        CoordinatorApp_SendP2PMessage(COORDINATOR_COMMAND_CLUSTERID,4,str);
        break;
    }
   // for(i = 0; i<SerialDataLen;i++)
   // {
     // HalUARTWrite(0,&str[i],1); 
    //}
}




/*********************************************************************

*********************************************************************/