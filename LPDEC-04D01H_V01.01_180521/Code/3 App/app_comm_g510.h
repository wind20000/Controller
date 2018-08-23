#ifndef __APP_COMM_G510_H
#define __APP_COMM_G510_H

#include "global.h"

#define TRAVERSE_AND_ADDEVT 1
#define TRAVERSE            0   

/* Bit defined */
#define BIT0			(0x01)
#define BIT1			(0x02)
#define BIT2			(0x04)
#define BIT3			(0x08)
#define BIT4			(0x10)
#define BIT5			(0x20)
#define BIT6			(0x40)
#define BIT7			(0x80)
/*************帧头/帧尾*************/
#define FRAME_HEAD       0x7f
#define FRAME_END        0xf7

#define TRANSFER_FLAG       0x1b
#define TRANSFER_CHAR1      0x1a
#define TRANSFER_CHAR1_FLAG 0x01
#define TRANSFER_CHAR2      0x1b
#define TRANSFER_CHAR2_FLAG 0x02

/***********************************************************************/
/*********************服务器下发控制码************************/
/*控制码第7bit 代表方向    0:服务端->终端,1:终端->服务端*/

#define WRITE_CTROL_CODE              0x20 
#define READ_CTROL_CODE               0x21
#define CONTROL_CTROL_CODE            0x22
#define UPLOAD_CTROL_CODE             0x23    

//#define RETURN_WRITE_CTROL_CODE       0xa0   
//#define RETURN_READ_CTROL_CODE        0xa1
//#define RETURN_CONTROL_CTROL_CODE     0xa2
//#define RETURN_UPLOAD_CTROL_CODE      0xa3

/*****************************************************************/

/************************包相关信息**********************************/
//登录包相关信息
#define AUTHENTICA_ID                   0xb010
#define AUTH_CTROL_CODE                 0xa3
#define AUTH_LEN                        0x09

//人工呼叫包相关信息
#define CALL_ID                         0xb080
#define CALL_CTROL_CODE                 0xa3
#define CALL_LEN                        0x03

//参数设置返回包
#define SET_PARMTE_ID                   0xb020
#define SET_PARMTE_CTROL_CODE           0xa0
#define SET_PARMTE_LEN                  0x03

//查询参数包
#define QUERY_PARMTE_ID                 0xb021
#define QUERY_PARMTE_CTROL_CODE         0xa1
#define QUERY_PARMTE_LEN                0x1d

//读信息包
#define READ_MESS_ID                    0xb022
#define READ_MESS_CTROL_CODE            0xa1
#define READ_MESS_LEN                   0x10

//控制包返回
#define CONTROL_ID                      0xb023
#define CTROL_CODE                      0xa2
#define CTROL_LEN                       0x03

//写地址返回包
#define WRITE_ADDR_ID                   0xb024
#define WRITE_ADDR_CTROL_CODE           0xa0
#define WRITE_ADDR_LEN                  0x03

//读地址返回包
#define READ_ADDR_ID                    0xb025
#define READ_ADDR_CTROL_CODE            0xa1
#define READ_ADDR_LEN                   0x07
/*****************************************************************/


/***********各个子任务的运行周期*************************/
#define SUB_TASK_PROCCTR_PER          (10U) /*10ms*/


/*******默认网络连接情况*********/	 
#define NET_PROTOCOL_DEFAULT        NET_PROTOCOL_TCP
#define SERVER_APN_DEFAULT          "cmiot"
#define SERVER_IP_DEFAULT           "118.178.227.84"
#define SERVER_PORT_DEFAULT         (8282U)
#define REPORT_PERIOD_DEFAULT       (30U)

#define SERVER_APN_LEN       (7)
#define SERVER_IP_LEN        (17)
//#define SERVER_PORT_LEN      (6)

/*********************BUFF LEN***********************/
#define GPRS_RTY_MESS_LEN         (40)
#define GPRS_RX_BUFF_LEN          (100)

#define GPRS_SendATCmdStr(pTxBuff, usLen)	        BSP_UART_Transmit(pTxBuff, usLen)


/*switch state*/
#define GPRS_IsSwitch_State()                       !READ_REG(g_stGPRS_Handler.ucStateEvent)
#define GPRS_State_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.ucStateEvent, Event)
#define GPRS_State_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.ucStateEvent, Event)
#define GPRS_State_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.ucStateEvent, Event)
#if 0
/*Comm Event*/
#define GPRS_Comm_Event_IsEmpty()                  !READ_REG(g_stGPRS_Handler.ucCommEvent)
#define GPRS_Comm_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.ucCommEvent, Event)
#define GPRS_Comm_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.ucCommEvent, Event)
#define GPRS_Comm_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.ucCommEvent, Event)

/*Comm Return Event*/
#define GPRS_Comm_Return_Event_IsEmpty()                  !READ_REG(g_stGPRS_Handler.ucReturnEvent)
#define GPRS_Comm_Return_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.ucReturnEvent, Event)
#define GPRS_Comm_Return_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.ucReturnEvent, Event)
#define GPRS_Comm_Return_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.ucReturnEvent, Event)


/*Comm Retry  Event*/
#define GPRS_Retry_Event_IsEmpty()                  !READ_REG(g_stGPRS_Handler.RetryCtrol.rtyEvent)
#define GPRS_Retry_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
#define GPRS_Retry_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
#define GPRS_Retry_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
#endif

#define GPRS_Event_IsEmpty(Event)                   !READ_REG(Event)
#define GPRS_Event_Get(Event,Bit)                   READ_BIT(Event, Bit)
#define GPRS_Event_Set(Event,Bit)                   SET_BIT(Event, Bit)
#define GPRS_Event_Clr(Event,Bit)                   CLEAR_BIT(Event, Bit)



#define Get_CommEvent(Event)                        CntLeadZeros(Event)
#define Get_NextState(Event)                        CntLeadZeros(Event)


/*********GPRS模块的工作状态****************/
typedef enum 
{
    GPRS_STATE_POWER_ON = 0,
    GPRS_STATE_RESET,
    GPRS_STATE_INIT,      /*进入AT初始化流程*/
	GPRS_STATE_CONNECT_NET,/*TCP/IP数据链路建立流程*/
	GPRS_STATE_READ_MESS,
	GPRS_STATE_COMM,
	GPRS_STATE_IDLE,
	GPRS_STATE_WAIT_ACK,
	GPRS_STATE_MAX,
   
}GPRS_STATE;

/*********GPRS模块通讯的数据ID**********/
typedef enum 
{
	UPLOAD_ID_AUTH = 0,   /*发送登录包*/
	UPLOAD_ID_CALL,       /*发送人工呼叫包*/
	UPLOAD_ID_MAX,
	UPLOAD_ID_INVALID = 0xFF,

}GPRS_COMM_ID;

typedef enum 
{
	RETURN_ID_SET_PARMTE=0,
	RETURN_ID_QUERY_PARMTE,
	RETURN_ID_READ_MESS,
	RETURN_ID_CTROL,
	RETURN_ID_WRITE_ADDR,
	RETURN_ID_READ_ADDR,
	RETURN_ID_MAX,
	RETURN_ID_INVALID = 0xFF,

}GPRS_COMM_RETURN_ID;


typedef enum 
{
	COMM_Event_AUTH         = (0x01 << UPLOAD_ID_AUTH),                  /*发送登录包*/
	COMM_Event_CALL         = (0x01 << UPLOAD_ID_CALL),                  /*发送人工呼叫包*/
	
    COMM_Event_SET_PARMTE   = (0x01 << RETURN_ID_SET_PARMTE),            
    COMM_Event_QUERY_PARMTE = (0x01 << RETURN_ID_QUERY_PARMTE),
    COMM_Event_READ_MESS    = (0x01 << RETURN_ID_READ_MESS),
    COMM_Event_CTROL        = (0x01 << RETURN_ID_CTROL),
    COMM_Event_WRITE_ADDR   = (0x01 << RETURN_ID_WRITE_ADDR),
    COMM_Event_READ_ADDR    = (0x01 << RETURN_ID_READ_ADDR),
	
}GPRS_COMM_Event;

/*State switch Event*/
typedef enum 
{
	STATE_Event_COMM   = BIT0,     /*go to Comm*/
    STATE_Event_MAX,
	
}GPRS_STATE_Event;


/*******GPRS应答类型*************/
typedef enum 
{
    RSP_TYPE_CMD = 0, /*应答AT指令*/
    RSP_TYPE_DATA,    /*通信数据*/
	RSP_TYPE_MAX,

}RSP_TYPE;


/*********GPRS模块操作选项***********/
typedef enum
{
	GPRS_OPER_POWER_ON = 0,
    GPRS_OPER_RESET,
	GPRS_OPER_MAX,
	
}ENUM_GPRS_OPERATION;

/*********AT执行结果***********/
typedef enum 
{
    NO_BACK = 0,
    BACK_TIMEOUT,           /*AT指令应答超时*/
    BACK_ERROR,             /*AT指令应答错误*/
	BACK_PERTIMES_OUT,      /*AT指令重发次数到了*/
	BACK_TRUE,              /*AT指令应答内容正确*/
	
}AT_RESULT_TYPE;


/**********GPRS指令集*****************/
typedef enum 
{

    GPRS_AT=0,/************************* 初始化流程***************************/
	GPRS_AT_ATE0,
//	GPRS_AT_AT_CPIN1,
    GPRS_AT_CGREG1,
    GPRS_AT_KEEPCONF,
    GPRS_AT_GTSET,
    GPRS_AT_MIPCALL_1,
	GPRS_AT_MIPCALL1,
    GPRS_AT_MIPOPEN_1,
	GPRS_AT_MIPOPEN1,
	
	GPRS_AT_MIPSEND_1,
	GPRS_AT_DATA,
	
	GPRS_AT_CSQ,
	GPRS_AT_CCID,
	GPRS_AT_CGSN,

	GPRS_AT_MIPRTCP,

}AT_CMD_Type;

typedef enum 
{
    RESPOND_TYPE=1,
	RTY_TYPE,
    UPLOAD_TYPE,    
	MAX_TYPE,
	
}SEND_TYPE;



/*******网络类型*************/
typedef enum 
{
    NET_NO_NET=0,
    NET_GPRS,
	NET_TYPE_MAX,
	
}NET_TYPE;

/*******网络协议*************/
typedef enum 
{
    NET_PROTOCOL_UDP=0,
    NET_PROTOCOL_TCP,
    NET_PROTOCOL_COAP,
    NET_PROTOCOL_MQTT,
	NET_PROTOCOL_MAX,
}NET_PROTOCOL;

typedef struct
{
	const S8* CmdStr;                /*指令符*/
	U16 usTimeOut;                   /*接收超时*/
	const S8* ExpResultStr;          /*期望结果*/
	U8 ucResendTimes;                /*重发次数*/
		
}GPRS_Cmd_Data_Type; 

/*网络参数*/
typedef struct
{
	NET_TYPE NetType;
	NET_PROTOCOL NetProtocol;
	U8 ServerApn[SERVER_APN_LEN];
	U8 ServerIp[SERVER_IP_LEN];
    U16 ServerPort;
    U16 ulReportPeriod; 
}Net_Parameters;

#define ADDR_REGION_LEN   7
typedef struct
{
//    U8 productType;                                //产品类型
    U8 AckMeageFlowNum;              //应答消息流水号
    U8 TerminalMeageFlowNum;         //流水号 
    U8 addrRegion[ADDR_REGION_LEN];  //地址域
    U8 controlCode;                  //控制码
    U8 dataLen;                      //数据长度
    U8 *bodyData;                    //数据体
    
}PACKET_HEAD_Type;

#define IMEI_LEN    6
#define ICCID_LEN   10
typedef struct
{
   // U8 imei[IMEI_LEN];
   U8 iccid[ICCID_LEN];
    
}AUTH_DATA_Type;

#define POWER_ON   1
#define POWER_OFF  0

typedef union
{
    U16 state;
    struct
    {
        U16 staOfEle        :1;
        U16 reserve         :15;
    };
    
}DEVICE_STATE_Type;


typedef struct
{
    DEVICE_STATE_Type DeviceState;   //设备状态
    U8 CSQ;                          //GPRS信号强度
    U8 hardwareVerLen;               //硬件版本号长度
    U8 hardwareVer[4];               //硬件版本号
    U8 softwareVerLen;               //软件版本号长度
    U8 softwareVer[4];               //软件版本号
    
}MESS_READ_Type;



/*********通讯的数据包类型**************/
typedef struct
{
	AUTH_DATA_Type AuthData;
	MESS_READ_Type MessRead;
		
}COMM_Packet_Type;

#define RTY_TIME_INIT      (30000UL)  /*30*1000 ms 即30s*/
#define RTY_TIMES_MAX      (3)
typedef struct
{
    U8  rtycnt;                                         //重传计数
    U8  rtycntEn;                                       //重传计数使能
#if CONFIG_RETRY_COPY
    U8  rtyBuff[GPRS_RTY_MESS_LEN];                     //重传消息备份
    U8  rtyLen;                                         //重传消息长度
#endif
    U16 rtyTimCount;                                    //重传定时器计数
    U16 rtyTime;                                        //重传时间
}Retry_Ctr_Type;

typedef struct
{
    U8 ucRespondEvt;
    U8 ucDelayEvt;
    U8 ucRetryEvt;
    U8 ucUploadEvt;
    
}Event_Type;

/*******AT指令通讯控制块**********/
typedef struct
{
    U8 SendATCmdEn;                 /*enable send ATCmd*/
	U8 ATCmdPt;                     /*当前AT命令索引*/
	U8 ucResendCnt;                 /*重发计数*/	
	U8 ucResendTimes;               /*重发次数*/
	U32 ulTimeOut;                  /*接收超时*/
	U8 ATCmdResult;                 /*当前AT指令通讯结果*/
	BOOL Busy;                      /*MCU与模块通讯忙碌标志*/
	U8 CmdStr[GPRS_RX_BUFF_LEN];    /*当前发送的指令符*/
	U8 ucCmdStrLen;                 /*当前指令长度*/
	S8 ExpResultStr[20];               /*当前发送的指令符的期望结果*/
	U16 ucByteTimeOut;               /*字节超时*/
	U8 RxBuf[GPRS_RX_BUFF_LEN];     /*接收数据缓存*/
	U8 ucRxCnt;                     /*接收数据计数*/
	U8 ucRxLen;                     /*接收数据长度*/
	U8 RxFrameOk;                   /*指示一帧接收完整*/
    U8 RspType;                     /*NB模块应答类型*/

}AT_CMD_CB; 

/*******GPRS模块控制块**********/
typedef struct
{
	U8 State;                       /*模块当前状态*/
	U8 StateOld;                    /*模块上一个状态*/
	U8 StepPt;                      /*step ponit*/
	U16 ulDelayCnt;                 /*延时计数*/
	U16 ulProcCtrTaskPer;           /*状态函数处理计数*/
	U8  ucCommEvent;                /*标记需要发送的数据包*/
	U8  ucSendType;                 /*标记发送类型*/
	U8 ucStateEvent;                /*当前的事件状态*/
	U8 ucDataID;                    /*标记正在发送的消息ID*/
	U8 ucCommBusy;                  /*通信忙碌标记*/
	U8 RxBuf[GPRS_RX_BUFF_LEN];     /*应用层接收到的数据*/
	U8 TxBuf[GPRS_RX_BUFF_LEN];     /*应用层需要发送的数据*/
	U8 ucTxLen;                     /*应用层发送数据长度*/              
	Net_Parameters   NetPar;        /*模块网络连接信息*/
    COMM_Packet_Type CommPacket;    /*通讯数据包*/
    Retry_Ctr_Type   RetryCtrol;    /*重发控制*/
    Event_Type       EvtProc;       /*事件处理*/
    AT_CMD_CB        ATCmdCB;       /*******AT指令通讯控制块**********/
	
}GPRS_Handler;

#define GPRS_NetPar          (g_stGPRS_Handler.NetPar)
#define GPRS_RetryCtrol      (g_stGPRS_Handler.RetryCtrol)
#define GPRS_ATCmdCB         (g_stGPRS_Handler.ATCmdCB)
#define GPRS_CommPacket      (g_stGPRS_Handler.CommPacket)
#define GPRS_EvtProc         (g_stGPRS_Handler.EvtProc)

/*************定义各个流程的step*******************/
#define INIT_STEP_NUM        NumberOfElements(s_ATCmdStep_Init)
#define CONNECT_STEP_NUM     NumberOfElements(s_ATCmdStep_Connnect)
#define MESS_STEP_NUM        NumberOfElements(s_ATCmdStep_Mess)
#define COMM_STEP_NUM        NumberOfElements(s_ATCmdStep_Comm)

extern PACKET_HEAD_Type      PacketHead;
extern GPRS_Handler          g_stGPRS_Handler;

void APP_GPRS_TimeProc(U16 usPeriod);
void APP_GPRS_NetParConfig(void);
void APP_COMM_Init(void);


void APP_SubTask_CmdProc(void);
void APP_SubTask_StateProc(void);
void APP_SuTask_DecodeProc(void);







#endif
