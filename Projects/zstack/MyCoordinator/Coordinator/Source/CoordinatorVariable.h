
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"
#include "OnBoard.h"
#include "ZComDef.h"
#include "ZGlobals.h"
#include "hal_lcd.h"

/* HAL */
#include"MT.h"
#include "MT_UART.h"

/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define CoordinatorAPP_ENDPOINT           20

#define CoordinatorAPP_PROFID             0x0F08
#define CoordinatorAPP_DEVICEID           0x0001
#define CoordinatorAPP_DEVICE_VERSION     0
#define CoordinatorAPP_FLAGS              0

#define CoordinatorAPP_MAX_CLUSTERS     4//2
#define ENDDEVICE_DATA_CLUSTERID        1
#define COORDINATOR_COMMAND_CLUSTERID   2
#define COORDINATOR_DATA_CLUSTERID      3
#define COORDINATOR_BROADCAST_CLUSTERID 4
  
#define CMD_SET_DEVICEID  10
#define CMD_GET_RSSI      15 
#define CMD_SET_PANID     16

#define DEVICEID_MAX      100
#define DEVICEID_MIN      0

#define SOP 0xfe   // the sop of the serial data---------Zimuge


// Send Message Timeout
#define CoordinatorAPP_SEND_PERIODIC_MSG_TIMEOUT   5000     // Every 5 seconds
#define CoordinatorAPP_REST_MSG_TIMEOUT            3000     // Every 3 seconds

// Application Events (OSAL) - These are bit weighted definitions.
#define CoordinatorAPP_TICK_ALIVE_MSG_EVT          0x0001
 
#define CoordinatorAPP_REST_MSG_EVT                0x0002

// Group ID for Flash Command

  
// Flash Command Duration - in milliseconds
#define CoordinatorAPP_FLASH_DURATION               1000



extern const cId_t CoordinatorApp_ClusterList[CoordinatorAPP_MAX_CLUSTERS]; 
extern const SimpleDescriptionFormat_t CoordinatorApp_SimpleDesc;


extern uint8 CoordinatorApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // CoordinatorApp_Init() is called.
extern devStates_t CoordinatorApp_NwkState;

extern uint8 CoordinatorApp_TransID;  // This is the unique message ID (counter)

extern afAddrType_t CoordinatorApp_Broadcast_DstAddr;
extern afAddrType_t CoordinatorApp_Group_DstAddr;
extern afAddrType_t CoordinatorApp_P2PDstAddr;     //µã¶Ôµã

extern aps_Group_t CoordinatorApp_Group;


extern uint8 CoordinatorAppPeriodicCounter;
extern uint8 CoordinatorAppFlashCounter;
extern uint16 GroupId;
//extern uint16 DevList[256];
extern uint16 PANID;
extern bool   REST_PANID;// when change the PANID through the Uart,
//use for restet the system

extern endPointDesc_t CoordinatorApp_epDesc;

extern uint8 Serial_Data[16];// use to translate data to the qt;
extern uint8 FCS;///the Serial_Data's FCS

typedef struct 
{
  uint16 addr;// the addresss of the device
  bool status;// is the device of the address still alive
  uint8  tick_alive; // the tick use to keep the addr alive
} AddrList_t;

extern AddrList_t AddrList[101];  //the address list of the devices
   
