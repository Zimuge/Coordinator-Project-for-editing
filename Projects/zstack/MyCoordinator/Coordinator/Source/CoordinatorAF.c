#include "CoordinatorVariable.h"
#include "CoordinatorAF.h"


 void CoordinatorApp_HandleAfIncomingMessage( afIncomingMSGPacket_t *pckt );//处理其他设备发送过来的数据
 void CoordinatorApp_SendBroadcastMessage(uint16 ClusterID,uint16 len,uint8 * str);//用广播的方式发送数据
 void CoordinatorApp_SendGroupMessage(uint16 ClusterID,uint16 len,uint8 * str);//用组播的方式发送数据
 void CoordinatorApp_SendP2PMessage(uint16 ClusterID,uint16 len,uint8 * str);//用点对点的方式发送数据
 void CoordinatorWriteUART(uint8 * str, uint8 len);// add SOP and  FCS to the str and send the data to the arm

 
 /*********************************************************************
 * @fn      CoordinatorApp_HandleAfIncomingMessage
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void CoordinatorApp_HandleAfIncomingMessage( afIncomingMSGPacket_t *pkt )
{
  uint8 MsgLength = pkt->cmd.DataLength;
  uint8 *str = pkt->cmd.Data;
  uint8 len = str[0];
  uint8 id  = str[1];
  uint8 command = str[2];
  uint16 deviceAddr = pkt->srcAddr.addr.shortAddr;
  
  if(command == CMD_GET_RSSI)
  {
    if(len == 0)
    {
        CoordinatorApp_P2PDstAddr.addr.shortAddr = deviceAddr;
        CoordinatorApp_SendP2PMessage(COORDINATOR_COMMAND_CLUSTERID,3,str);
    }
    return;
  }// this message is send to the devices to tell it the RSSI regardless of the AddrList 
  
  if((id <= DEVICEID_MAX)) // the id is in the range
  {
      if( (AddrList[id].addr == deviceAddr)  || (AddrList[id].status == FALSE))
      {
        if(AddrList[id].status == FALSE) // the deveice of the address is not alive
        {
           AddrList[id].addr = deviceAddr; 
           AddrList[id].status = TRUE;
        }
        AddrList[id].tick_alive = 100; // reload the tick value
        
        switch ( pkt->clusterId )
        {
          case ENDDEVICE_DATA_CLUSTERID:
            switch (command)
            {
              case CMD_SET_DEVICEID:
                if(len == 1)
                {
                   if(str[3] <= DEVICEID_MAX)
                   {
                       AddrList[id].status = FALSE;//the old address is die
                       AddrList[id].tick_alive = 0;
                       AddrList[id].addr = 0x0000;
                       id = str[3];
                       AddrList[id].status = TRUE;//the new address is alive
                       AddrList[id].tick_alive = 100;
                       AddrList[id].addr = deviceAddr;
                   }
                   
                 // HalUARTWrite(0,(uint8 *)(&deviceAddr),2);
                }
                break;
            default:
              if(MsgLength == (len + 3)) // the data from the enddevice will be send to the arm
              {
                 CoordinatorWriteUART(str,MsgLength);
              }
              break;
            }
            break; 
        }
      }
      else
      {
          // the device doesn't exit in the list
      }
  }
  else;
}

/*********************************************************************
 * @fn      CoordinatorApp_SendBroadcastMessage
 *
 * @brief   Send the Broadcast message.
 *
* @param   ClusterID,the clusterID of the message,
           len,the length of the message,
           str,the pointer of the message.
 *
 * @return  none
 */
void CoordinatorApp_SendBroadcastMessage(uint16 ClusterID,uint16 len,uint8 * str)
{
 
      if ( AF_DataRequest( &CoordinatorApp_Broadcast_DstAddr, &CoordinatorApp_epDesc,
                           ClusterID,
                           len,
                           str,//(uint8*)&CoordinatorAppPeriodicCounter,
                           &CoordinatorApp_TransID,
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
 * @fn      CoordinatorApp_SendGroupMessage
 *
 * @brief   Send the Group message .
 *
 * @param   ClusterID,the clusterID of the message,
            len,the length of the message,
            str,the pointer of the message.
 *
 * @return  none
 */
void CoordinatorApp_SendGroupMessage(uint16 ClusterID,uint16 len,uint8 * str)
{
  if ( AF_DataRequest( &CoordinatorApp_Group_DstAddr, &CoordinatorApp_epDesc,
                       ClusterID,
                       len,
                       str,
                       &CoordinatorApp_TransID,
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
 * @fn      CoordinatorApp_SendP2PMessage
 *
 * @brief   Send the PointToPoint message.
 *
 * @param   ClusterID,the clusterID of the message,
            len,the length of the message,
            str,the pointer of the message.
 *
 * @return  none
 */
void CoordinatorApp_SendP2PMessage(uint16 ClusterID,uint16 len,uint8 * str)
{
  if(CoordinatorApp_P2PDstAddr.addr.shortAddr)// ensure that do not send message to self
  {
      if(AF_DataRequest(&CoordinatorApp_P2PDstAddr,&CoordinatorApp_epDesc,
                        ClusterID,
                        len,
                        str,
                        &CoordinatorApp_TransID,
                        AF_DISCV_ROUTE,
                        AF_DEFAULT_RADIUS ) == afStatus_SUCCESS)
      {
       // HalUARTWrite(0,str,4);
      }
      else
      {
        
        
      } 
  }
  else;
}

/*********************************************************************
 * @fn      CoordinatorWriteUART
 *
 * @brief   add SOP and  FCS to the str and send the data to the arm
 *
 * @param   str - the data to send
 *
 *          len - the lengh of the str
 *
 * @return  none
 ********************************************************************/
void CoordinatorWriteUART(uint8 * str, uint8 len)
{
  uint8 i = 0;
  FCS = 0x00;
  for(i = 0; i < len; i++)
  {
      Serial_Data[i + 1] = str[i];
      FCS ^= str[i];
  }
  Serial_Data[i + 1] = FCS; 
  HalUARTWrite(0,Serial_Data,len + 2); 
}