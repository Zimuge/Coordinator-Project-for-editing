#include "CoordinatorVariable.h"





const cId_t CoordinatorApp_ClusterList[CoordinatorAPP_MAX_CLUSTERS] =
{
 ENDDEVICE_DATA_CLUSTERID,             
 COORDINATOR_COMMAND_CLUSTERID,   
 COORDINATOR_DATA_CLUSTERID,
 COORDINATOR_BROADCAST_CLUSTERID 
};

const SimpleDescriptionFormat_t CoordinatorApp_SimpleDesc =
{
  CoordinatorAPP_ENDPOINT,              //  int Endpoint;
  CoordinatorAPP_PROFID,                //  uint16 AppProfId[2];
  CoordinatorAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  CoordinatorAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  CoordinatorAPP_FLAGS,                 //  int   AppFlags:4;
  CoordinatorAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)CoordinatorApp_ClusterList,  //  uint8 *pAppInClusterList;
  CoordinatorAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)CoordinatorApp_ClusterList   //  uint8 *pAppInClusterList;
};


uint8 CoordinatorApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // CoordinatorApp_Init() is called.
devStates_t CoordinatorApp_NwkState;

uint8 CoordinatorApp_TransID;  // This is the unique message ID (counter)

afAddrType_t CoordinatorApp_Broadcast_DstAddr;
afAddrType_t CoordinatorApp_Group_DstAddr;
afAddrType_t CoordinatorApp_P2PDstAddr;     //µã¶Ôµã

aps_Group_t CoordinatorApp_Group;

uint8 CoordinatorAppPeriodicCounter = 0;
uint8 CoordinatorAppFlashCounter = 0;
uint16 GroupId = 0x0001;
//uint16 DevList[256] = {0};
endPointDesc_t CoordinatorApp_epDesc;
uint16 PANID = 0;
uint8 Serial_Data[16] = {SOP};// use to translate data to the qt;
uint8 FCS;///the Serial_Data's FCS
bool   REST_PANID  = FALSE;// when change the PANID through the Uart,
//use for restet the system
AddrList_t AddrList[101] = {{0,0,0}};  //the address list of the devices