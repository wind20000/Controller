#include "bsp_var.h"

static void SYS_Mess_Ver_Init(void)
{
    u8 i=0;
    u8 software_var[4] = TERMINAL_SOFTWARE_VER;
    u8 hardware_var[4] = TERMINAL_HARDWARE_VER;
    for(i=0;i<4;i++)
    {
         SYS_HardVerMess[i] = hardware_var[i];
    }
    for(i=0;i<4;i++)
    {
         SYS_SoftVerMess[i] = software_var[i];
    }
    SYS_HardVerMessLen = 4;
    SYS_SoftVerMessLen = 4;
}

static void Net_Par_Config(void)
{
    strcpy((S8*)GPRS_NetPar.ServerIp, SERVER_IP_DEFAULT);
    strcpy((S8*)GPRS_NetPar.ServerApn, SERVER_APN_DEFAULT);
    GPRS_NetPar.ServerPort     = SERVER_PORT_DEFAULT;
    GPRS_NetPar.ulReportPeriod = REPORT_PERIOD_DEFAULT;
	GPRS_NetPar.NetProtocol    = NET_PROTOCOL_DEFAULT;
    GPRS_NetPar.NetType        = NET_NO_NET;
}

static void G510_Var_Init(void)
{
    Var_Fill(g_stGPRS_Handler,0);
    Var_Fill(PacketHead,0);
    Net_Par_Config();
}

void Bsp_Var_Init(void)
{
    G510_Var_Init();
    SYS_Mess_Ver_Init();
    for(u8 i=0;i<8;i++)
    {
       ga_Subtask[i] = 0;  //系统子任务分支   SYS_SubTask[0]对应最高优先权        
    }  
    g_Maintask = 0; 
}


