#include "app_comm_g510.h"

GPRS_Handler        g_stGPRS_Handler;
PACKET_HEAD_Type    PacketHead;

typedef void(*GPRS_State_Fun)(U8 *RxBuf); /* 定义状态处理函数*/
//typedef void(*GPRS_State_Fun)(void);
static void APP_GPRS_RefreshRetryMess(void);
static U8   APP_GPRS_EvtTraverse(U8 mode);




static void APP_GPRS_State_Init_Proc   (U8 *RxBuf);
static void APP_GPRS_State_Connect_Proc(U8 *RxBuf);
static void APP_GPRS_State_Mess_Proc(U8 *RxBuf);
static void APP_GPRS_State_Comm_Proc   (U8 *RxBuf);

static const GPRS_State_Fun APP_GPRS_StateProcTbl[] = 
{
    NULL,     //POWER_ON
    NULL,     //RESET
    APP_GPRS_State_Init_Proc,
	APP_GPRS_State_Connect_Proc,
	APP_GPRS_State_Mess_Proc,
	APP_GPRS_State_Comm_Proc,
	NULL,
    NULL,
    NULL,
    NULL,
};


/******************GPRS指令集**********************************/
const GPRS_Cmd_Data_Type AT_CmdTbl[] = 
{
    /*------cmdstr-----------------------------------revtimeout(s)--cmdtruebackstr----------retry_times */
    /************************* GPRS  network ***************************/
      {"AT\r\n",                                         4,       "OK",                     2},
      {"ATE0\r\n",                                       3,       "OK",                     3},
   //   {"AT+CPIN?\r\n",                                   3,       "READY",                  3},
      {"AT+CGREG?\r\n",   			                     3,	      "+CGREG: 0,1",         	15},

      {"AT+MIPKEEPCONF=1,240,240,5\r\n",   			     3,	      "OK",         	        3},
/*当设置为0时，原始输出格式(1字节转2字节)*/
/*当设置为1时，不转换直接输出，且不带标识头*/
/*当设置为2时，不转换直接输出，带标识头*/
      {"AT+GTSET=\"IPRFMT\",2\r\n",   			         3,	      "OK",         	        2},   //待选，

/*建立网络连接*/
      {"AT+MIPCALL=1,\"CMNET\"\r\n",                     30,      "+MIPCALL:",            3},
      {"AT+MIPCALL?\r\n",                                3,       "+MIPCALL: 1",            2},
      {"AT+MIPOPEN=1,,\"118.178.227.84\",9091,0\r\n",    20,        "+MIPOPEN: 1,1",        3},
      {"AT+MIPOPEN?\r\n",                                3,       "+MIPOPEN: 1",            2},
/* 数据发送*/
      {"AT+MIPSEND=1\r\n",                               10,      ">",                      2},
      {"DATA",                                           6,       "OK",                     1},  //需要发送的数据，待优化
      
      
      {"AT+CSQ\r\n",   			                         3,	      "+CSQ",         	3},  //待优化
      {"AT+CCID\r\n",                                    4,       "898",                    2},
      {"AT+CGSN\r\n",                                    4,       "OK",                     2},

      {NULL,                                          NULL,       "+MIPRTCP",            NULL},
      
};

/*************************初始化流程***************************/
static const U8 s_ATCmdStep_Init[] = 
{
    NULL,
    GPRS_AT,       GPRS_AT_ATE0,    /*GPRS_AT_AT_CPIN1, */  GPRS_AT_CGREG1, GPRS_AT_KEEPCONF,  
	GPRS_AT_GTSET,                                
};

/************************* TCP/IP数据链路建立连接***************************/
static const U8 s_ATCmdStep_Connnect[] = 
{
    GPRS_AT_MIPCALL_1, GPRS_AT_MIPCALL1, GPRS_AT_MIPOPEN_1, //GPRS_AT_MIPOPEN1,                               
};

/*************************数据发送流程***************************/
static const U8 s_ATCmdStep_Comm[] = 
{
    GPRS_AT_MIPSEND_1,GPRS_AT_DATA,                             
};

static const U8 s_ATCmdStep_Mess[] = 
{
    GPRS_AT_CSQ,GPRS_AT_CCID,
    //GPRS_AT_CGSN,       /*IMEI号暂不读取*/                         
};


/*校验AT指令应答结果是否正确*/
static U8 APP_GPRS_ATCmdAckCheck(U8 *RxBuf)
{   

    /*判断是否是服务器下行数据*/
    if(Check_Strstr((S8*)RxBuf, (S8*)(S8*)AT_CmdTbl[GPRS_AT_MIPRTCP].ExpResultStr, GPRS_ATCmdCB.ucRxLen) == TRUE)  
    {
        GPRS_ATCmdCB.RspType = RSP_TYPE_DATA;  /*通信数据*/
        return BACK_TRUE;   /*接收到正确应答*/

    }
    if(Check_Strstr((S8*)RxBuf, "ready", GPRS_ATCmdCB.ucRxLen) == TRUE) //GPRS模块准备就绪推送
    {
        Task_Flag_Struct.atReday_F = ENABLE;
        GPRS_ATCmdCB.RspType = RSP_TYPE_CMD;
	    return BACK_TRUE;
    }
    if(GPRS_ATCmdCB.ATCmdPt != GPRS_AT_MIPOPEN_1)/*socket连接时,也有可能回复MIPSTAT: 1,1*/
    {
        if(Check_Strstr((S8*)RxBuf, "MIPSTAT: 1,1", GPRS_ATCmdCB.ucRxLen) == TRUE)
        {
            g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;    //重新连接socket
		    g_stGPRS_Handler.StepPt = 2;
		    GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_AUTH);
        }
    }
    if(GPRS_ATCmdCB.ATCmdPt != GPRS_AT_MIPCALL1) /*由于IP获取错误，也会去查询有无IP*/
    {
        if(Check_Strstr((S8*)RxBuf, "MIPCALL: 0", GPRS_ATCmdCB.ucRxLen) == TRUE)
        
        { 
		    g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;    //重新连接网络
		    g_stGPRS_Handler.StepPt = 0;
		    GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_AUTH);
        }
    }

    
     /*判断接收的数据是否含有期望应答*/
	if(Check_Strstr((S8*)RxBuf, GPRS_ATCmdCB.ExpResultStr, GPRS_ATCmdCB.ucRxLen) == TRUE)
	{
		GPRS_ATCmdCB.RspType = RSP_TYPE_CMD;/*应答AT指令*/
	    return BACK_TRUE;                 /*接收到正确应答*/
	} 
    else
	{   
	    /*由于获取网络侧地址和建立soctet连接都是分两条返回的，第一条都为"OK"*/
	    if((GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MIPCALL_1) || GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MIPOPEN_1)
	    {
  	        if(Check_Strstr((S8*)RxBuf, "OK", 4) == TRUE)   //去掉这一句再试
  	        {
  	            return NO_BACK;   /*第一条"OK"返回正确，所以继续等待*/
  	        }
	    }
	     /*查询是否驻网成功，如不成功，那么等待3S，而不是返回错误，该条指令特殊*/
        if(GPRS_ATCmdCB.ATCmdPt == GPRS_AT_CGREG1) 
        {
            return NO_BACK;
        }
		return BACK_ERROR;   		
	}	
}

static U8 APP_GPRS_Encrypt(u8 *hex_data_buf,u8 len_hex,u8 *new_hex_data_buf,u8 *new_len_hex)
{
  u8 len=0;
  for(u8 i=0;i<len_hex;i++,len++)
  {
    switch(hex_data_buf[i])
    {
      case TRANSFER_CHAR1:
              new_hex_data_buf[len]=TRANSFER_FLAG;
              len++;
              new_hex_data_buf[len]=TRANSFER_CHAR1_FLAG;
              break;
      case TRANSFER_CHAR2:
              new_hex_data_buf[len]=TRANSFER_FLAG;
              len++;
              new_hex_data_buf[len]=TRANSFER_CHAR2_FLAG;
              break;
      default:
              new_hex_data_buf[len]=hex_data_buf[i];
              break;
     }
  }
  *new_len_hex=len;
  if(*new_len_hex<=GPRS_RX_BUFF_LEN){
    return TRUE;
  }
  else{
    return FALSE;
  }
}

static void APP_GPRS_Data_Trope(u8 *hex_data_buf,u8 len_hex,u8 *new_hex_data_buf)
{
  u16 len_hex_cnt=0;
  u16 len_temp=0;
				
  for(len_temp=0;len_hex_cnt<len_hex;len_hex_cnt++,len_temp++)      
  {
    if(hex_data_buf[len_hex_cnt]==TRANSFER_FLAG)
    {
      switch(hex_data_buf[len_hex_cnt+1])
      {
        case TRANSFER_CHAR1_FLAG:
              new_hex_data_buf[len_temp]=TRANSFER_CHAR1;
              len_hex_cnt++;
              break;
        case TRANSFER_CHAR2_FLAG:
              new_hex_data_buf[len_temp]=TRANSFER_CHAR2;
              len_hex_cnt++;
              break;
        default:
        #if DEBUG_CONTROLLER
              BSP_LOG("0x1b error\r\n");
        #endif
              break;
      }
    }
    else
    {
       new_hex_data_buf[len_temp]=hex_data_buf[len_hex_cnt];
    }
  } 
}

static U16 APP_GPRS_GetPacket(u16 messID,U8 ctrolcode,U8 datalen,U8* Packet)
{
    U8 i=0,j=0;
    U8 check=0;
    U8 ipLen=0;
    U8 apnLen=0;
    U8 fillZeroLen=0;
    U8 newLen=0;
    U8 TxBuf[GPRS_RX_BUFF_LEN]={0};
#if DEBUG_RTY_FUN
    static U8 rtyTestCnt=0;
#endif
   
    TxBuf[i++] = FRAME_HEAD;
    TxBuf[i++] = 0x01;    //产品类型
    for(j=0;j<ADDR_REGION_LEN;j++)
    {
       TxBuf[i++] = PacketHead.addrRegion[j]&0xff;
    }
    TxBuf[i++] = ctrolcode&0xff;
    TxBuf[i++] = datalen&0xff;
    TxBuf[i++] = (messID>>8)&0xff;
    TxBuf[i++] = messID&0xff;
    switch(messID)
    {
       case AUTHENTICA_ID:
       {
           TxBuf[i++] = PacketHead.TerminalMeageFlowNum;
           for(j=0;j<6;j++)
           {
               TxBuf[i++] = GPRS_CommPacket.AuthData.iccid[j+4];
           }
           
       }
       break;
       
       case CALL_ID:
       {
           TxBuf[i++] = PacketHead.TerminalMeageFlowNum;
           //呼叫数据为空
       }
       
       break;
       case SET_PARMTE_ID:
       {
           TxBuf[i++] = PacketHead.AckMeageFlowNum;
           //返回数据内容为空
       }
       break;
       case QUERY_PARMTE_ID:
       {
            TxBuf[i++] = PacketHead.AckMeageFlowNum;
            ipLen  = strlen((char *)GPRS_NetPar.ServerIp);
            apnLen = strlen((char *)GPRS_NetPar.ServerApn);
            for(j=0;j<ipLen;j++)
            {
                TxBuf[i++] = (GPRS_NetPar.ServerIp[j]) & 0xff;
            }
            fillZeroLen = SERVER_IP_LEN-ipLen;
            for(j=0;j<fillZeroLen;j++)        //为了填满SERVER_IP_LEN(17)长度的字节
            {
                TxBuf[i++] = 0;
            }  
            
            TxBuf[i++] = (GPRS_NetPar.ServerPort>>8) & 0xff;
            TxBuf[i++] = (GPRS_NetPar.ServerPort) & 0xff;
            for(j=0;j<apnLen;j++)
            {
                TxBuf[i++] = GPRS_NetPar.ServerApn[j] & 0xff;
            }
            fillZeroLen = SERVER_APN_LEN - apnLen;
            for(j=0;j<fillZeroLen;j++)
            {
                TxBuf[i++] = 0;
            } 
       }
       break;
       case READ_MESS_ID:
       {
            TxBuf[i++] = PacketHead.AckMeageFlowNum;
            TxBuf[i++] = (GPRS_CommPacket.MessRead.DeviceState.state>>8) & 0xff;
            TxBuf[i++] = (GPRS_CommPacket.MessRead.DeviceState.state) & 0xff;
            TxBuf[i++] = (GPRS_CommPacket.MessRead.CSQ) & 0xff;
            TxBuf[i++] = (GPRS_CommPacket.MessRead.hardwareVerLen) & 0xff;
            for(j=0;j<GPRS_CommPacket.MessRead.hardwareVerLen;j++)
            {
                TxBuf[i++] = (GPRS_CommPacket.MessRead.hardwareVer[j]) & 0xff;
            }
            TxBuf[i++] = (GPRS_CommPacket.MessRead.softwareVerLen) & 0xff;
            for(j=0;j<GPRS_CommPacket.MessRead.softwareVerLen;j++)
            {
                TxBuf[i++] = (GPRS_CommPacket.MessRead.softwareVer[j]) & 0xff;
            }
       }
       break;
       case CONTROL_ID:
       {
           TxBuf[i++] = PacketHead.AckMeageFlowNum;
            //返回数据内容为空
       }
       break;
       case WRITE_ADDR_ID:
       {
          TxBuf[i++] = PacketHead.AckMeageFlowNum;
           //返回数据内容为空
       }
       break;
       case READ_ADDR_ID:
       {
          TxBuf[i++] = PacketHead.AckMeageFlowNum;
          for(j=0;j<4;j++)
          {
               TxBuf[i++] = (PacketHead.addrRegion[j]) & 0xff;
          }
       }
       break;
    }
    check = Get_Xor(TxBuf,i); //计算校验和
    TxBuf[i++] = check&0xff;
#if DEBUG_RTY_FUN
    rtyTestCnt++;
    if(rtyTestCnt <=2)
    {
        for(j=0;j<3;j++)
        {
            TxBuf[i++]=j;
        }
    } 
    if((rtyTestCnt <=8) && (rtyTestCnt >=4))
    {
         for(j=0;j<3;j++)
         {
              TxBuf[i++]=j;
         }
    }
#endif     
    TxBuf[i++] = FRAME_END;
    APP_GPRS_Encrypt(TxBuf,i,Packet,&newLen);
    return   (U16)newLen;
    
}

static void APP_GPRS_RetryMessInit(void)
{
   GPRS_RetryCtrol.rtycnt = 0;
   GPRS_RetryCtrol.rtycntEn = DISABLE;
   
   GPRS_RetryCtrol.rtyTimCount =0;
   GPRS_RetryCtrol.rtyTime = RTY_TIME_INIT;
#if CONFIG_RETRY_COPY
   GPRS_RetryCtrol.rtyLen = 0;
   memset(GPRS_RetryCtrol.rtyBuff,0,GPRS_RTY_MESS_LEN);
#endif
}

static void APP_GPRS_RefreshRetryMess(void)
{
   GPRS_RetryCtrol.rtycnt++;
   GPRS_RetryCtrol.rtycntEn = ENABLE;
   GPRS_RetryCtrol.rtyTimCount =0;
   GPRS_RetryCtrol.rtyTime = RTY_TIME_INIT;   
}

#if CONFIG_RETRY_COPY
static U16 APP_GPRS_WriteRetryIDPacket(U8* Packet)
{
/*重传时也重新组包，防止第一包数据出错，那么后续两包数据也会错误*/
/*后续重传涉及到时间数据，那么需把数据保存，重新组包时再放入*/
/*如重传时不重新组包，需把宏CONFIG_RETRY_COPY置位*/
    U16 usPacketLen=0;
	#if DEBUG_CONTROLLER
            BSP_LOG("retry send mess\r\n");
    #endif
    memcpy(Packet,GPRS_RetryCtrol.rtyBuff,GPRS_RetryCtrol.rtyLen);    //重传信息copy至发送缓存
    usPacketLen = GPRS_RetryCtrol.rtyLen;                             
    return usPacketLen;    
}

#endif

static U16 APP_GPRS_WriteReturnIDPacket(U8 DataID, U8* Packet)
{
    U16 usPacketLen=0;
	switch(DataID)
	{

		case RETURN_ID_SET_PARMTE:
		{
			usPacketLen = APP_GPRS_GetPacket(SET_PARMTE_ID,SET_PARMTE_CTROL_CODE,SET_PARMTE_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send set parte mess\r\n");
            #endif
		}break;	
		case RETURN_ID_QUERY_PARMTE:
		{
			usPacketLen = APP_GPRS_GetPacket(QUERY_PARMTE_ID,QUERY_PARMTE_CTROL_CODE,QUERY_PARMTE_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send query parte mess\r\n");
            #endif
		}break;
		case RETURN_ID_READ_MESS:
		{
			usPacketLen = APP_GPRS_GetPacket(READ_MESS_ID,READ_MESS_CTROL_CODE,READ_MESS_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send read mess\r\n");
            #endif
		}break;
		case RETURN_ID_CTROL:
		{
			usPacketLen = APP_GPRS_GetPacket(CONTROL_ID,CTROL_CODE,CTROL_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send ctrol mess\r\n");
            #endif
		}break;
		case RETURN_ID_WRITE_ADDR:
		{
			usPacketLen = APP_GPRS_GetPacket(WRITE_ADDR_ID,WRITE_ADDR_CTROL_CODE,WRITE_ADDR_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send write addr mess\r\n");
            #endif
		}break;
		case RETURN_ID_READ_ADDR:
		{
			usPacketLen = APP_GPRS_GetPacket(READ_ADDR_ID,READ_ADDR_CTROL_CODE,READ_ADDR_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send read addr mess\r\n");
            #endif
		}break;
	    default:
			break;
	}
	return usPacketLen;

}
/*根据不同的数据ID,进行组帧，封装协议层的帧内容*/
static U16 APP_GPRS_WriteDataIDPacket(U8 DataID, U8* Packet)
{
    U16 usPacketLen=0;
	switch(DataID)
	{    
		case UPLOAD_ID_AUTH:
		{

			usPacketLen = APP_GPRS_GetPacket(AUTHENTICA_ID,AUTH_CTROL_CODE,AUTH_LEN,Packet);
#if CONFIG_RETRY_COPY            
            GPRS_RetryCtrol.rtyLen = usPacketLen;                   //消息长度备份
            memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //消息备份   
#endif
		    #if DEBUG_CONTROLLER
                BSP_LOG("send auth mess\r\n");
            #endif
		}break;
		
		case UPLOAD_ID_CALL:
		{
			usPacketLen = APP_GPRS_GetPacket(CALL_ID,CALL_CTROL_CODE,CALL_LEN,Packet);
#if CONFIG_RETRY_COPY
			GPRS_RetryCtrol.rtyLen = usPacketLen; 
			memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);
#endif
		    #if DEBUG_CONTROLLER
                BSP_LOG("send call mess\r\n");
            #endif
		}break;	

	    default:
			break;
	}
	return usPacketLen;
}

static void APP_GPRS_SetIP(U8 *pFrameDes)
{
    U8 i=0;
    U8 ipLen=0;
    U8 portLen=0;
    U8 data[6]={0};
    U8 *ip = GPRS_NetPar.ServerIp;
    U8 *port=NULL;
    ipLen = strlen((char*)ip);
    port = DecimalToASCII(GPRS_NetPar.ServerPort,data); //端口号转换支持1--65535
    if(port != NULL)
    {
        portLen=strlen((char*)port);
        pFrameDes[0] = '"';
        for(i=0;i<ipLen;i++)
        {
           pFrameDes[i+1]=ip[i];
        }
        pFrameDes[ipLen+1] = '"';
        pFrameDes[ipLen+2] = ',';
        for(i=0;i<portLen;i++)
        {
           pFrameDes[ipLen+3+i]=port[i];
        }
        pFrameDes[ipLen+portLen+3]=',';
        pFrameDes[ipLen+portLen+4]='0';
        pFrameDes[ipLen+portLen+5]='\r';
        pFrameDes[ipLen+portLen+6]='\n';
        pFrameDes[ipLen+portLen+7]='\0';
        return ;
    }
    //端口号转换出错，打印信息
    
}

static U8 APP_GPRS_WriteATPacket(U8 ATCmdIndex, U8 *pFrameDes)
{  
    U8 ucSendLen;
    U8 dataNum;
    char *endStr="\x1A\r\n";
	switch(ATCmdIndex)
	{
		case GPRS_AT_MIPOPEN_1:/*建立socket连接*/
		{
            strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
		    dataNum = Get_Comma(2,(u8*)pFrameDes);
		    APP_GPRS_SetIP(&pFrameDes[dataNum]);			    
		 	ucSendLen = strlen((char*)pFrameDes);
			
		}break;
		
		case GPRS_AT_DATA:/*填充数据内容*/
		{
		     memcpy(pFrameDes, g_stGPRS_Handler.TxBuf, g_stGPRS_Handler.ucTxLen);/*copy 应用层数据*/
             ucSendLen = strlen(endStr);
             memcpy(pFrameDes+g_stGPRS_Handler.ucTxLen,endStr,ucSendLen);        /*copy 结尾符*/
             ucSendLen += g_stGPRS_Handler.ucTxLen;
			
		}break;
		
		default:
		{
			strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);
			ucSendLen = strlen(AT_CmdTbl[ATCmdIndex].CmdStr);/*默认的指令符长度*/
			
		}break;
	}
	return ucSendLen;    
}


static void APP_GPRS_SendATCmd(AT_CMD_CB* pNBATCmdCB)
{
	U8 ucResendCnt;
	U8 ATCmdIndex;
	AT_CMD_CB *pAtCmd = pNBATCmdCB;
	ATCmdIndex = pAtCmd->ATCmdPt;
	if(1 == pAtCmd->Busy)
	{
		return ;/*当前忙碌状态*/	
	}
	/*根据不同的AT应答结果,设置重发次数*/
	if((BACK_TRUE == pAtCmd->ATCmdResult) || (BACK_PERTIMES_OUT == pAtCmd->ATCmdResult))
	{
		ucResendCnt = 0;/*重发清零*/	
	}
	else
	{
	    ucResendCnt = GPRS_ATCmdCB.ucResendCnt;/*备份重发次数*/;	
	}

	memset((U8*)pAtCmd, 0, sizeof(GPRS_ATCmdCB));
	
	pAtCmd->Busy         = 1;          /*设置忙绿状态*/
	pAtCmd->ATCmdPt      = ATCmdIndex; /*更新AT索引号*/
	pAtCmd->ATCmdResult  = NO_BACK;
	pAtCmd->ucResendCnt  = ucResendCnt;  
	pAtCmd->ucResendTimes = AT_CmdTbl[ATCmdIndex].ucResendTimes;
	pAtCmd->ulTimeOut    = TmrSecondsToMilliseconds(AT_CmdTbl[ATCmdIndex].usTimeOut);/*刷新超时时间*/

	/*****t填充指令内容长度******/
	pAtCmd->ucCmdStrLen = APP_GPRS_WriteATPacket(ATCmdIndex, pAtCmd->CmdStr);
	/*填充指令符期望结果*/	
	strcpy(pAtCmd->ExpResultStr, AT_CmdTbl[ATCmdIndex].ExpResultStr);
	/*发送AT指令*/	
	GPRS_SendATCmdStr(pAtCmd->CmdStr, pAtCmd->ucCmdStrLen);
	/*发送次数+1*/	
    pAtCmd->ucResendCnt++;
}

static void APP_GPRS_State_Init_Proc(U8 *RxBuf)
{
	U8 ATCmdIndex;
	if(g_stGPRS_Handler.StepPt >= INIT_STEP_NUM)
	{
		return;	
	}	 
	ATCmdIndex = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt];
	/************************AT指令应答正确·**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
	{
		/*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= INIT_STEP_NUM)
		{
			/******GPRS模块状态切换***************/
			g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;/*切换到下一个流程*/
			g_stGPRS_Handler.StepPt = 0;     /**clr step*/
			GPRS_NetPar.NetType = NET_GPRS;/*无网络连接状态*/
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_INIT;/*继续当前流程*/		
		}	
	}
/************************AT 指令达到重发次数**************************/	
	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)/*AT指令重发次数到了*/
	{
		switch(ATCmdIndex)
		{
		    case GPRS_AT:   //直接开始下一条指令
			{
			    g_stGPRS_Handler.StepPt++;
			    g_stGPRS_Handler.State = GPRS_STATE_INIT; /*继续当前流程*/	
			    
			}break;
			default:
			{
			    #if DEBUG_CONTROLLER
                      BSP_LOG("Init Reset\r\n");
                #endif
			    g_stGPRS_Handler.State = GPRS_STATE_RESET;       //进行复位
		        g_stGPRS_Handler.StepPt = 0;/**clr step*/
		        GPRS_NetPar.NetType = NET_NO_NET;
			}
				break;	
		}
	}
/************************AT指令应答错误或超时*************************/	
	else
    {
		/*没有到AT指令重发次数，所以再次发送AT指令*/
		g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*直接再次发送AT指令，然后等待应答*/
		GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt];
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/	 
    }	
}

static void APP_GPRS_State_Connect_Proc(U8 *RxBuf)
{
    U8 ATCmdIndex;
	
	if(g_stGPRS_Handler.StepPt >= CONNECT_STEP_NUM)
	{
		return;	
	}
	
	ATCmdIndex = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];
/************************AT指令应答正确·**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
	{	           
         /*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= CONNECT_STEP_NUM)
		{
			/******GPRS模块状态切换***************/
			g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;/*切换到下一个流程*/
			g_stGPRS_Handler.StepPt = 0;/**clr step*/
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;/*继续当前流程*/				
		}			
	}
/************************AT 指令达到重发次数**************************/	
	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)
	{
	    switch(ATCmdIndex)
		{
		    case GPRS_AT_MIPCALL_1:   //直接开始下一条指令
			{
			    g_stGPRS_Handler.StepPt++;
			    g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET; /*继续当前流程*/	
			    
			}break;
			default:
			{
			    #if DEBUG_CONTROLLER
                      BSP_LOG("Connect Reset\r\n");
                #endif
			    g_stGPRS_Handler.State = GPRS_STATE_RESET;       //进行复位
		        g_stGPRS_Handler.StepPt = 0;/**clr step*/
		        GPRS_NetPar.NetType = NET_NO_NET;
			}
				break;	
		}	
	}
/************************AT指令应答错误或超时*************************/	
	else
	{
		/*没有到AT指令重发次数，所以再次发送AT指令*/
		g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*直接再次发送AT指令，然后等待应答*/
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/	
		GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];
	}	

}

static void APP_GPRS_State_Mess_Proc(U8 *RxBuf)
{
        U8 ATCmdIndex;
        U8 asi_len=0;
        char *str = "898";
        char *p   = NULL;
        if(g_stGPRS_Handler.StepPt >= MESS_STEP_NUM)
        {
            return; 
        }   
        ATCmdIndex = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
        /************************AT指令应答正确·**************************/    
        if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
        {
            switch(ATCmdIndex)
            {
                  case GPRS_AT_CSQ:
                  {
				        GPRS_CommPacket.MessRead.CSQ = Get_Uint_Data((S8*)&RxBuf[Get_Comma(1, RxBuf)] - 3);
		
			      }
                  break;	
			      case GPRS_AT_CCID:
                  {
				        //获取ICCID
				        p=strstr((char*)RxBuf,str);
				        asi_len = strlen(p);
				        if(asi_len > 20) asi_len = 20;    
				        Ascii_To_Hex(p,GPRS_CommPacket.AuthData.iccid,(U16)asi_len); 
				        
			      }
                  break;	
			      default:
				    break;
            }
            /*next Step*/
            g_stGPRS_Handler.StepPt++;
            if(g_stGPRS_Handler.StepPt >= MESS_STEP_NUM)
            {
                /******GPRS模块状态切换***************/
                g_stGPRS_Handler.State = GPRS_STATE_IDLE;/*切换到下一个流程*/
                g_stGPRS_Handler.StepPt = 0;     /**clr step*/
            }
            else
            {
                g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;/*继续当前流程*/       
            }   
        }
    /************************AT 指令达到重发次数**************************/ 
        else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)/*AT指令重发次数到了*/
        {
            #if DEBUG_CONTROLLER
                  BSP_LOG("Mess Reset\r\n");
            #endif
            g_stGPRS_Handler.State = GPRS_STATE_RESET;  //进行复位
            g_stGPRS_Handler.StepPt = 0;/**clr step*/
            GPRS_NetPar.NetType = NET_NO_NET;
        }
    /************************AT指令应答错误或超时*************************/ 
        else
        {
            /*没有到AT指令重发次数，所以再次发送AT指令*/
            g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*直接再次发送AT指令，然后等待应答*/
            GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
            GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/  
        }

}

static void APP_GPRS_State_Comm_Proc   (U8 *RxBuf)
{
    U8 ATCmdIndex;  
    static U8 errCnt=0;
	if(g_stGPRS_Handler.StepPt >= COMM_STEP_NUM)
	{
		return;	
	} 
	ATCmdIndex = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
/************************AT指令应答正确·**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
	{
		switch(ATCmdIndex)
		{
			case GPRS_AT_DATA:
            {
				
				if(RESPOND_TYPE == g_stGPRS_Handler.ucSendType)
				{
				    GPRS_Event_Clr(GPRS_EvtProc.ucRespondEvt,(0x01 << g_stGPRS_Handler.ucDataID));
				}
				else if(RTY_TYPE == g_stGPRS_Handler.ucSendType)
				{
				    GPRS_Event_Clr(GPRS_EvtProc.ucRetryEvt,(0x01 << g_stGPRS_Handler.ucDataID));
		            APP_GPRS_RefreshRetryMess();
				}
				else if(UPLOAD_TYPE == g_stGPRS_Handler.ucSendType)
				{
				    /*本次通讯完成,清除对应的事件*/
				    GPRS_Event_Clr(GPRS_EvtProc.ucUploadEvt,(0x01 << g_stGPRS_Handler.ucDataID));
				    //如在这里置起事件，则需等到应答才能发送下一条主动上传消息            
				    GPRS_Event_Set(GPRS_EvtProc.ucDelayEvt,(0x01 << g_stGPRS_Handler.ucDataID));     
                    APP_GPRS_RefreshRetryMess();
				}
				GPRS_Event_Clr(g_stGPRS_Handler.ucCommEvent,(0x01 << g_stGPRS_Handler.ucDataID));
		        errCnt=0;           /*AT指令成功，清除错误计数*/
			}break;	
			
			default:
				break;	
		}
		/*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= COMM_STEP_NUM)
		{
			/******GPRS模块状态切换***************/
			/*数据标签来判断是否还有其它数据需要发送，如果有则再次进入comm，没有则退出通讯状态*/
			if(APP_GPRS_EvtTraverse(TRAVERSE) == FALSE)
			{
                g_stGPRS_Handler.State = GPRS_STATE_IDLE;/*切换到下一个流程*/
				g_stGPRS_Handler.ucCommBusy= 0;/*Comm Complete*/
			}
			else
			{
				g_stGPRS_Handler.State = GPRS_STATE_COMM;/*继续当前流程*/		
			}
			
			g_stGPRS_Handler.StepPt = 0;/**clr step*/
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_COMM;/*继续当前流程*/		
		}		
	}
/************************AT 指令达到重发次数**************************/	
	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)
	{
	    if(GPRS_AT_DATA == ATCmdIndex)
	    {
	        g_stGPRS_Handler.State = GPRS_STATE_COMM;
	        errCnt++;
	    }
	    else if((GPRS_AT_MIPSEND_1 == ATCmdIndex) || (errCnt > 3))
	    {
            #if DEBUG_CONTROLLER
                  BSP_LOG("Comm Reset\r\n");
            #endif
            errCnt=0;
	        g_stGPRS_Handler.State = GPRS_STATE_RESET;
	        GPRS_NetPar.NetType = NET_NO_NET;
	    }
	    
		g_stGPRS_Handler.StepPt = 0;/**clr step*/
		g_stGPRS_Handler.ucCommBusy= 0;	

	}
/************************AT指令应答错误或超时*************************/	
	else
	{
		/*没有到AT指令重发次数，所以再次发送AT指令*/
		g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*直接再次发送AT指令，然后等待应答*/
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
		GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
	} 
}
/*如果服务器下发数据不加标识头，此函数可放入AT指令解析中*/

static U8 APP_GPRS_RxDataCheck(U8 *rxBuff)   
{
    U8 i=0;
    U8 rxLen=rxBuff[10]+13;
    if(rxBuff[0] != FRAME_HEAD)       return FALSE;  //帧头校验
    if(rxBuff[1] != 0x01)       return FALSE;        //产品类型校验
    for(i=0;i<4;i++)
    {
        if(rxBuff[i+2] != PacketHead.addrRegion[i])  //地址域校验
            return FALSE;
    }
    if(rxBuff[rxLen-1] != FRAME_END)   return FALSE;
    if(FALSE == Xor_Check(rxBuff,rxLen))             //和校验
    {
        return FALSE;
    }
    #if DEBUG_CONTROLLER
    BSP_LOG("DECODE TRUE\r\n");
    #endif
    return TRUE;
}
static void APP_GPRS_RxDataDecode(U8 *rxBuff)
{
    U8 i=0;
    U8 ctrolCmd=0;
    U8 *p_buf=rxBuff+14;
    U8 ctrCode  = rxBuff[9];
  //  U8 dataLen  = rxBuff[10];
    U16 command = ((U16)(rxBuff[11])<<8) + rxBuff[12]; 
    U8 flowNum= rxBuff[13];
    switch(ctrCode)
    {
         case WRITE_CTROL_CODE:             /*服务器下发写控制码*/
         {
             PacketHead.AckMeageFlowNum = flowNum;
             switch(command)
             {
                 case SET_PARMTE_ID:       //设置参数
                 {
                      for(i=0;i<SERVER_IP_LEN;i++)
                      {
                           GPRS_NetPar.ServerIp[i] = p_buf[i];
                      }
                      GPRS_NetPar.ServerPort = ((U16)(p_buf[SERVER_IP_LEN]<<8)+p_buf[SERVER_IP_LEN+1]);
                      for(i=0;i<SERVER_APN_LEN;i++)
                      {
                           GPRS_NetPar.ServerApn[i] = p_buf[SERVER_IP_LEN+2+i];
                      }
                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_SET_PARMTE);
                      Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//置起存储任务
                 }
                 break;
                 case WRITE_ADDR_ID:      //写地址
                 {
                      for(i=0;i<4;i++)
                      {
                            PacketHead.addrRegion[i] = p_buf[i];
                      }
                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_WRITE_ADDR);
                      Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//置起存储任务
                 }
                 break;
                 default:
                    break;
             }
         }
         break;
         case READ_CTROL_CODE:              /*服务器下发读控制码*/
         {
             PacketHead.AckMeageFlowNum = flowNum;
             switch(command)
             {
                 case QUERY_PARMTE_ID:      //查询参数设置
                 {
                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_QUERY_PARMTE);
                 }
                 break;
                 case READ_MESS_ID:         //读信息
                 {
                       if(g_stGPRS_Handler.ucCommBusy == 0)
                       {
                            //这里再读一次信号强度
                            g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
		                    g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;        //重新读取信息
		                    g_stGPRS_Handler.StepPt = 0;
		               }
		               GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_MESS);
                       if(BSP_RELAY_CTROL_READ ==0)
                       {
                            GPRS_CommPacket.MessRead.DeviceState.staOfEle = POWER_OFF;
                       }
                       else
                       {
                            GPRS_CommPacket.MessRead.DeviceState.staOfEle = POWER_ON; 
                       }
                 }
                 break;
                 case READ_ADDR_ID:         //读地址
                 {
                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_ADDR);
                 }
                 break;
                 default:
                    break;
             }
         }
         break;
         case CONTROL_CTROL_CODE:           /*服务器下发控制码*/
         {
             PacketHead.AckMeageFlowNum = flowNum;
             if(command == CONTROL_ID)      //控制包
             {
                 ctrolCmd =  p_buf[0];
                 if(ctrolCmd == ENABLE)   
                 {
                      //上电 控制IO口
                      BSP_RELAY_CTROL_SET;
                      GPRS_CommPacket.MessRead.DeviceState.staOfEle = POWER_ON;            
                 }
                 else
                 {
                      //断电 控制IO口
                      BSP_RELAY_CTROL_RESET;
                      GPRS_CommPacket.MessRead.DeviceState.staOfEle = POWER_OFF;           
                 }
                 Set_Task(MEM_WRITE,MEM_STORE_DYNAMIC_DATA);
                 GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_CTROL);
             }
         }
         break;
         case UPLOAD_CTROL_CODE:            /*服务器返回主动上传控制码*/
         {
             PacketHead.TerminalMeageFlowNum++;
             switch(command)
             {
                 case AUTHENTICA_ID:       //登录包返回
                 {
                    //登录成功
                    APP_GPRS_RetryMessInit();
                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_AUTH);
                 }
                 break;
                 case CALL_ID:             //人工呼叫包返回
                 {
                    //呼叫成功
                    APP_GPRS_RetryMessInit();
                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_CALL);
                 }
                 break;
                 default:
                    break;
             }
         }
         break;
         default:
                 break;
    }
}

static void APP_GPRS_EventProc(U8 surEvt,U8* objEvt)
{
    U8 ucID=0;
    if(GPRS_Event_IsEmpty(surEvt)== FALSE) 
    {
          ucID = Get_CommEvent(surEvt);
          GPRS_Event_Set(*objEvt,(1<<ucID));
    }

}
static U8 APP_GPRS_EvtTraverse(U8 mode)
{    
    if(GPRS_Event_IsEmpty(GPRS_EvtProc.ucRespondEvt)== FALSE)
    {
        if(mode == TRAVERSE_AND_ADDEVT)  APP_GPRS_EventProc(GPRS_EvtProc.ucRespondEvt,&g_stGPRS_Handler.ucCommEvent);
        return RESPOND_TYPE;
    }
    else if(GPRS_Event_IsEmpty(GPRS_EvtProc.ucDelayEvt)== FALSE)
    {        
        if(GPRS_Event_IsEmpty(GPRS_EvtProc.ucRetryEvt)== FALSE)
        {
            if(mode == TRAVERSE_AND_ADDEVT)  APP_GPRS_EventProc(GPRS_EvtProc.ucRetryEvt,&g_stGPRS_Handler.ucCommEvent);
            return RTY_TYPE;
        }
        return 0;
    }
    else if(GPRS_Event_IsEmpty(GPRS_EvtProc.ucUploadEvt)== FALSE)
    {
        if(mode == TRAVERSE_AND_ADDEVT)  APP_GPRS_EventProc(GPRS_EvtProc.ucUploadEvt,&g_stGPRS_Handler.ucCommEvent);
        return UPLOAD_TYPE;
    }
    else if(GPRS_Event_IsEmpty(g_stGPRS_Handler.ucCommEvent)== FALSE)
    {
        /*理论上程序无法运行到这里*/
        g_stGPRS_Handler.ucCommEvent = 0;
        #if DEBUG_CONTROLLER
             BSP_LOG("Event is Error \r\n");
        #endif
        return 0;
    }
    else
    {
        g_stGPRS_Handler.State = GPRS_STATE_IDLE;  /*切换至IDLE态*/
        return 0;
    }
}
static void APP_GPRS_Reset_Init(void)
{                
    
    /*延时事件添加至主动上传事件中*/
    /*需把延时事件全部添加至主动上传事件中*/ 
    APP_GPRS_EventProc(GPRS_EvtProc.ucDelayEvt,&GPRS_EvtProc.ucUploadEvt);                   
    GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_AUTH);
    GPRS_EvtProc.ucRespondEvt =0;
    GPRS_EvtProc.ucRetryEvt =0;
    GPRS_EvtProc.ucDelayEvt =0;
    g_stGPRS_Handler.ucCommEvent = 0;
    APP_GPRS_RetryMessInit();                   /*重传相关变量初始化*/
}

void APP_COMM_Init(void)
{    
#if DEBUG_TROPE_FUN
    PacketHead.TerminalMeageFlowNum = 0x19;
#endif
	g_stGPRS_Handler.State = GPRS_STATE_POWER_ON; 
	g_stGPRS_Handler.StateOld = GPRS_STATE_POWER_ON;
	g_stGPRS_Handler.StepPt = 0;
	GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_AUTH);/*上电需发送一个登录包*/
	Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
}



void APP_GPRS_TimeProc(U16 usPeriod)
{

//    if(GPRS_ATCmdCB.Busy == 1)   //在数据接收时，AT指令也忙碌，这边可能会有冲突，把接收时，忙碌标志去除
//	{
		/*********AT指令应答超时**************/
		if(GPRS_ATCmdCB.ulTimeOut >= usPeriod)
        {
            GPRS_ATCmdCB.ulTimeOut-=usPeriod;
            if(GPRS_ATCmdCB.ulTimeOut < usPeriod)
            {
				GPRS_ATCmdCB.ATCmdResult = BACK_TIMEOUT;/*超时*/ 
                			
            }
        }
//	}
	if(GPRS_ATCmdCB.ucByteTimeOut >= usPeriod)
	{
	    GPRS_ATCmdCB.ucByteTimeOut -= usPeriod;
	    if(GPRS_ATCmdCB.ucByteTimeOut < usPeriod)
	    {
				GPRS_ATCmdCB.RxFrameOk = 1;
                Set_Task(COMM, COMM_CMD_PROC);
	    }
	}
	/*****************GPRS模块延时计数*************************/
	if(g_stGPRS_Handler.ulDelayCnt >= usPeriod)    
	{
	    g_stGPRS_Handler.ulDelayCnt -= usPeriod;
	}
    else
    {
        g_stGPRS_Handler.ulDelayCnt = 0;
    }
	
   /*****************CPRS 模块状态处理函数*************************/	
	g_stGPRS_Handler.ulProcCtrTaskPer += usPeriod;
	if(g_stGPRS_Handler.ulProcCtrTaskPer >= SUB_TASK_PROCCTR_PER)
	{
	    g_stGPRS_Handler.ulProcCtrTaskPer -= SUB_TASK_PROCCTR_PER;
		Set_Task(COMM, COMM_STATE_PROC);
	}	
	/*****************CPRS 模块重传处理*************************/
    if(GPRS_RetryCtrol.rtycntEn != DISABLE)
    {
        GPRS_RetryCtrol.rtyTimCount+=usPeriod;
        if(GPRS_RetryCtrol.rtyTimCount >= GPRS_RetryCtrol.rtyTime)
        {
              GPRS_RetryCtrol.rtyTimCount=0;
              GPRS_RetryCtrol.rtycntEn = DISABLE;
              APP_GPRS_EventProc(GPRS_EvtProc.ucDelayEvt,&GPRS_EvtProc.ucRetryEvt);
        }
    }

}


/*模块AT指令解析任务*/
void APP_SubTask_CmdProc(void)
{
    disableInterrupts();
    GPRS_ATCmdCB.Busy = 0;   
    GPRS_ATCmdCB.ucRxLen = GPRS_ATCmdCB.ucRxCnt;
    GPRS_ATCmdCB.ucRxCnt = 0;
    GPRS_ATCmdCB.RxFrameOk = 0;
    memcpy(g_stGPRS_Handler.RxBuf,GPRS_ATCmdCB.RxBuf,GPRS_ATCmdCB.ucRxLen);
    enableInterrupts();

    GPRS_ATCmdCB.ATCmdResult = APP_GPRS_ATCmdAckCheck(g_stGPRS_Handler.RxBuf); 
    /**********应答错误直接返回***********************/ 
    if(BACK_ERROR == GPRS_ATCmdCB.ATCmdResult) 
    {
        return;
    }
    /**********应答AT指令直接返回,通过APP_SubTask_StateProc 处理***/
    if(RSP_TYPE_CMD == GPRS_ATCmdCB.RspType)
    {
        return;
    }
    /***********对服务器的下行数据进行协议解析和相应处理****************/
    if(RSP_TYPE_DATA == GPRS_ATCmdCB.RspType)
    {
        Set_Task(COMM,COMM_DECODE);
        #if DEBUG_CONTROLLER
        BSP_LOG("START DECODE\r\n");
        #endif
    }  
    GPRS_ATCmdCB.ATCmdResult = NO_BACK;


}


void APP_SubTask_StateProc(void)
{
    static U8 ResetTimesCount=0;
    switch(g_stGPRS_Handler.State)
    {
         case GPRS_STATE_WAIT_ACK:
         {
              //如果没有结果就直接返回
 			  if(NO_BACK == GPRS_ATCmdCB.ATCmdResult)
			  {
				    return;	
			  }  
			  //AT应答超时，或者AT指令应答解析错误，则判断是否需要重发
			  if((GPRS_ATCmdCB.ATCmdResult == BACK_ERROR) || (GPRS_ATCmdCB.ATCmdResult == BACK_TIMEOUT))  
			  {
                    if(GPRS_ATCmdCB.ucResendCnt >= GPRS_ATCmdCB.ucResendTimes)
	        	    {	
	        	        GPRS_ATCmdCB.ATCmdResult = BACK_PERTIMES_OUT;/*重发次数到了*/
	        	    }			
			  }
			  //根据不同的结果，做相应处理
			  if(APP_GPRS_StateProcTbl[g_stGPRS_Handler.StateOld] != NULL)
		      {
		            APP_GPRS_StateProcTbl[g_stGPRS_Handler.StateOld](g_stGPRS_Handler.RxBuf);		
		      }
              if((BACK_TRUE == GPRS_ATCmdCB.ATCmdResult) || (BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult))/*AT指令应答内容正确*/
              {
                    /*更新StateOld*/
                    g_stGPRS_Handler.StateOld = GPRS_STATE_WAIT_ACK;
              }
              GPRS_ATCmdCB.Busy = 0;              
         }
         break;
         case GPRS_STATE_POWER_ON:
         {
              if(g_stGPRS_Handler.ulDelayCnt > 0)
              {
                    return;
              }
              if(0 == g_stGPRS_Handler.StepPt)
			  {
               /*************************
                                G510 Power on流程：
                                   ___        ___
                                     |_______|
                                         800ms
                        电路转换 高->低；低->高
                             **************************/
				    BSP_GPRS_POWERON_SET;   
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 900;   //拉低850ms
				    LED_Status = CONN_INDI;              /*正在联网LED指示*/
			  }
			  else if(1 == g_stGPRS_Handler.StepPt)
			  {
			        BSP_GPRS_POWERON_RESET;
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 1000; //等待1S后再操作GPRS模块
			  }
			  else/*next state*/
			  {
				    /*一些变量清0*/
                    GPRS_NetPar.NetType = NET_NO_NET;/*无网络状态连接*/
				    g_stGPRS_Handler.StateOld = GPRS_STATE_POWER_ON;	
				    g_stGPRS_Handler.State = GPRS_STATE_INIT;/*开始下一个流程*/
				    g_stGPRS_Handler.StepPt = 0;	    
			  }
         }
         break;
         /************该状态为GPRS模块POWER_OFF，所以执行完该流程后需POWER_ON ************/
         /**如该流程直接完成模块的复位，那么执行完该流程后,可开始AT指令流程**/
         case GPRS_STATE_RESET:  
         {
              if(g_stGPRS_Handler.ulDelayCnt > 0)
              {
                    return;
              }
              if(0 == g_stGPRS_Handler.StepPt)
			  {
				    //引脚操作
				    BSP_GPRS_BUCKEN_RESET;
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 3500; //拉低3.5S
				    LED_Status = SYS_INDI;     /*系统LED指示*/
			  }
			  else if(1 == g_stGPRS_Handler.StepPt)
			  {
			        //引脚操作
			        BSP_GPRS_BUCKEN_SET;
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 3000;
			  }
			  else/*next state*/
			  {
				    /*一些变量清0*/
                    GPRS_NetPar.NetType = NET_NO_NET;               /*无网络状态连接*/
				    g_stGPRS_Handler.StateOld = GPRS_STATE_RESET;	
				    g_stGPRS_Handler.State = GPRS_STATE_POWER_ON;   /*开始下一个流程*/
				    g_stGPRS_Handler.StepPt = 0;	
				    APP_GPRS_Reset_Init();
				    ResetTimesCount++;
				    if(ResetTimesCount >=3)
				    {
				        ResetTimesCount = 0;
				        Task_Flag_Struct.closeStaProc = ENABLE;
                        #if DEBUG_CONTROLLER
                        BSP_LOG("Close State Proc\r\n");
                        #endif
				    }
              /**这边添加复位次数计数，如超过设定复位次数，那么暂时不进行对GPRS的操作**/
              /****具体实现可关闭定时器里面的周期状态处理函数*****/
			  }
         }
         break;
         case GPRS_STATE_INIT:
         {
            if((g_stGPRS_Handler.ulDelayCnt > 0) && (Task_Flag_Struct.atReday_F == DISABLE))
            {
                    return;
            }
            if(0 == g_stGPRS_Handler.StepPt)
			{
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 20000; //20S GPRS AT准备就绪超时时间
			}
			else
			{
			        g_stGPRS_Handler.ulDelayCnt = 0;  //计数清0
			        Task_Flag_Struct.atReday_F = DISABLE; //AT就绪失能
			        g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State;
			        g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			        GPRS_ATCmdCB.SendATCmdEn = 1; /*enable send ATCmd*/
			        GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt]; 
			}
         }
         break;
         case GPRS_STATE_CONNECT_NET:
		 {				
			g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];
			
         }
         break;	
         case GPRS_STATE_READ_MESS:
         {
            g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
         }
         break;
         case GPRS_STATE_COMM:
         {
            g_stGPRS_Handler.ucSendType = APP_GPRS_EvtTraverse(TRAVERSE_AND_ADDEVT);
            if((FALSE == g_stGPRS_Handler.ucSendType) || (GPRS_Event_IsEmpty(g_stGPRS_Handler.ucCommEvent)))
            {
                break;
            }

			if(0 == g_stGPRS_Handler.StepPt)
			{
				g_stGPRS_Handler.ucCommBusy = 1;/*标记正在和服务器通讯*/
				LED_Status = DATA_SEND_INDI;    /*正在通讯LED指示*/
			}
            
			if(GPRS_AT_DATA == s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt]) 
			{   /*APP_GPRS_WriteReturnIDPacket与APP_GPRS_WriteDataIDPacket函数可以合二为一*/
			    /*但考虑到后续项目协议可能增多，最终还是得把主动上传和应答消息分开*/
				switch(g_stGPRS_Handler.ucSendType)
				{
				    case RESPOND_TYPE:
				    {
				        g_stGPRS_Handler.ucDataID = Get_CommEvent(GPRS_EvtProc.ucRespondEvt);
				        g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteReturnIDPacket(g_stGPRS_Handler.ucDataID, g_stGPRS_Handler.TxBuf);
				    }
				    break;
				    case RTY_TYPE:
				    {
				        g_stGPRS_Handler.ucDataID = Get_CommEvent(GPRS_EvtProc.ucRetryEvt);
				    #if CONFIG_RETRY_COPY
				        g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteRetryIDPacket(g_stGPRS_Handler.TxBuf);
				    #else
				        g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteDataIDPacket(g_stGPRS_Handler.ucDataID, g_stGPRS_Handler.TxBuf);
				    #endif
				    }
				    break;
				    case UPLOAD_TYPE:
				    {
				        g_stGPRS_Handler.ucDataID = Get_CommEvent(GPRS_EvtProc.ucUploadEvt);
				        g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteDataIDPacket(g_stGPRS_Handler.ucDataID, g_stGPRS_Handler.TxBuf);
				    }
				    break;
				}
			}
            //此时重发计数为3，并且为第一条指令。因为第二条指令会把重传计数加1
			if((GPRS_RetryCtrol.rtycnt >= RTY_TIMES_MAX) && (g_stGPRS_Handler.StepPt ==0))   
			{
			    g_stGPRS_Handler.StateOld = GPRS_STATE_RESET;   //复位GPRS模块
			    g_stGPRS_Handler.State = GPRS_STATE_RESET;
			    g_stGPRS_Handler.StepPt = 0;			    
                #if DEBUG_CONTROLLER
                      BSP_LOG("Rty Reset\r\n");
                #endif
			    return;
			}
			g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
         }
         break;
         case GPRS_STATE_IDLE:
		 {
			U8 NextStateOffset;
             /*判断是否有数据需要通讯*/
			if(APP_GPRS_EvtTraverse(TRAVERSE) != FALSE)
			{
			    /*有数据要通讯则设置切换到COM状态的事件*/
                GPRS_State_Event_Set(STATE_Event_COMM);
			}
            
			if(GPRS_IsSwitch_State())  //现不做处理，后续可用做低功耗
			{
                break;
			}
			
			/*获取下一个优先级高的状态*/
            NextStateOffset = Get_NextState(g_stGPRS_Handler.ucStateEvent);
			g_stGPRS_Handler.State = GPRS_STATE_COMM + NextStateOffset;
			GPRS_State_Event_Clr(0x01 << NextStateOffset);
	        g_stGPRS_Handler.StateOld = GPRS_STATE_IDLE; 
			g_stGPRS_Handler.StepPt = 0;		

		 }
         break;	
         default:
		 {
		    g_stGPRS_Handler.State = GPRS_STATE_INIT;
			g_stGPRS_Handler.StepPt = 0;
		 }
         break;
    }
    if(1 == GPRS_ATCmdCB.SendATCmdEn)
    {
          GPRS_ATCmdCB.SendATCmdEn = 0;
          APP_GPRS_SendATCmd(&GPRS_ATCmdCB);
    }

}


void APP_SuTask_DecodeProc(void)
{   
    U8  TropeBuf[GPRS_RX_BUFF_LEN]={0};
    U8  rxLen = GPRS_ATCmdCB.ucRxLen;
    U8 *rxBuff = &g_stGPRS_Handler.RxBuf[Get_Comma(2,g_stGPRS_Handler.RxBuf)];
    APP_GPRS_Data_Trope(rxBuff,rxLen,TropeBuf);
    if(APP_GPRS_RxDataCheck(TropeBuf) == FALSE)  return;   //标记错误码，待添加
    APP_GPRS_RxDataDecode(TropeBuf);
}






































