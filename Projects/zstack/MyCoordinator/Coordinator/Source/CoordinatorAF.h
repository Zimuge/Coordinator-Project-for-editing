extern void CoordinatorApp_HandleAfIncomingMessage( afIncomingMSGPacket_t *pckt );//处理其他设备发送过来的数据
extern void CoordinatorApp_SendBroadcastMessage(uint16 ClusterID,uint16 len,uint8 * str);//用广播的方式发送数据
extern void CoordinatorApp_SendGroupMessage(uint16 ClusterID,uint16 len,uint8 * str);//用组播的方式发送数据
extern void CoordinatorApp_SendP2PMessage(uint16 ClusterID,uint16 len,uint8 * str);//用点对点的方式发送数据
extern void CoordinatorWriteUART(uint8 * str, uint8 len);// add SOP and  FCS to the str and send the data to the arm