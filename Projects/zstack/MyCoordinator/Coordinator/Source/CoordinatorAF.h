extern void CoordinatorApp_HandleAfIncomingMessage( afIncomingMSGPacket_t *pckt );//���������豸���͹���������
extern void CoordinatorApp_SendBroadcastMessage(uint16 ClusterID,uint16 len,uint8 * str);//�ù㲥�ķ�ʽ��������
extern void CoordinatorApp_SendGroupMessage(uint16 ClusterID,uint16 len,uint8 * str);//���鲥�ķ�ʽ��������
extern void CoordinatorApp_SendP2PMessage(uint16 ClusterID,uint16 len,uint8 * str);//�õ�Ե�ķ�ʽ��������
extern void CoordinatorWriteUART(uint8 * str, uint8 len);// add SOP and  FCS to the str and send the data to the arm