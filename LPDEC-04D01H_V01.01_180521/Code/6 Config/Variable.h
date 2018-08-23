/*************************************************
** 版权:       杭州利尔达科技有限公司
** 文件名:     main.h
** 版本：      
** 工作环境:   
** 作者:     
** 生成日期:   
** 功能:       结构体定义  系统全局变量定义及声明  
** 相关文件:
** 修改日志:   无
*************************************************/
#ifndef __VARIABLE__H__
#define __VARIABLE__H__

#include "Main.h"

typedef unsigned char           INT8U;  // 数据类型
typedef signed char             INT8S;
typedef unsigned int            INT16U;
typedef signed int              INT16S;
typedef unsigned long           INT32U;
typedef signed long             INT32S;
typedef unsigned long long      INT64U;
typedef signed long long        INT64S;
typedef float                   FP32;
typedef double                  FP64;



#define CONST        const
#define STATIC       static
#define IMPORT       extern                   //输入 
#define IMPORT       extern                   //输入   

#define EXPORT       extern                   //输出  
 
#define EXTERN       extern
#define INLINE       inline                   //内联
#define NO_INIT      __no_init                //零初始化
#define __O          volatile                 //只写 
#define __IO         volatile                 //读写  

//////////////////////////////  系统  /////////////////////////////////////////

////////////////  系统软硬件  //////////////////
#define ON                          1                   
#define OFF                         0 
#define INVALID                     0xFF 

#define SOFT_VERSION                0x01                //固件版本x.x BCD码

////////////////  调试  ///////////////////////         //调试部分的宏定义在正式版本中都必须设置为0
#define COMM_SUM_CHECK_DEBUG_EN   0                   //协议数据和校验  置位表示禁止校验


////////////////  实时时钟  ///////////////////////
#define SYS_RTC_EN                      1        //实时时钟必须保证每半秒喂一次狗
#define SYS_RTC_MAX_CNT                 8        //中断 RTC_MAX_CNT 次则为1秒      RTC_MAX_CNT 可设置为 2或4 必须是2的倍数 因为LCD_Cnt 会& RTC_MAX_CNT

#define SYS_RTC_HARDWARE_EN             1                   //使用硬件时钟+基本定时器  每秒更新系统时间


////////////////  UART  ///////////////////////
#define UART_EN                     1

//////////////////////////////  通讯  ////////////////////////////////////////
#define COMM_EN                     1
#define COMM_RETURN_BUFF_LENGTH     70        //
#define COMM_RX_BUFF_LENGTH         40        //

//////////////// protocol  ///////////////////////
#define COMM_EN                   1
#define COMM_METER_TYPE           0x00C0   //设备类型                        

////////////////////////////////存储///////////////////////////////////////////

#define MEM_EN                      1
#define MEM_HISTORY_LENGTH          60                  //可纪录前N月的历史数据
#define MEM_FIRST_POR_DATA          SOFT_VERSION        //该数据用于识别系统是否初次上电
                                                        //初次上电则向EEPROM中各个存储位置写入初始数据 以后每次上电均需恢复这些数据
////////////////  存储地址  ///////////////////////
//wang STM8S207R8的DATAFlash起始地址0x004000h

#define MEM_FIRST_POR_DATA_ADDR         0x004002    //初次上电标识

#define MEM_PAR_INFO_ADDR               0x004006    //固定参数存储首地址
#define MEM_SYS_VALVE_STATE_ADDR        0x004010    //系统阀门状态的数据存储地址
#define MEM_SYS_RUN_STATE_ADDR          0x004020    //系统运行状态数据存储地址
#define MEM_SYS_TEMP_AND_HUD_INFO_ADDR  0x004030    //系统温湿度数据存储地址
#define MEM_SYS_FAULT_STATE_ADDR        0x004040    //系统故障数据存储地址
#define MEM_RTC_ADDR                    0x004050    //系统时间


////////////////  FLASH  ///////////////////////
#define FLASH_EN                    0




/////////////////////////////////  通用子函数  /////////////////////////////////////////
#define CLKS_PER_MS                  (F_MCLK*1000)
#define CLKS_PER_US                   F_MCLK

/////////////////////////////////  Task  //////////////////////////////////////////////
#define TASK0                         SG_Task
#define TASK1                         COMM_Task
#define TASK2                         MEM_Read_Task
#define TASK3                         MEM_Write_Task
#define TASK4                         Measure_Task
#define TASK5                         Reserve_Task
#define TASK6                         Contrl_Task
#define TASK7                         Other_Task

#define SG                            0      //wangw140928主任务-系统安全任务
#define SG_SYS_SCAN                   1
#define SG_LOW_VOLT_DETECT            2

#define COMM                          1      //wangw140928主任务-通信任务
#define COMM_DECODE                   0
#define COMM_ACTION                   1
#define COMM_RETURN_MTASK             COMM   //协议数据返回 可设置为COMM或LCD   //wang2014-6-20只涉及到优先级的问题，之前热表用LCD，是为了可以让数据实时性好
#define COMM_RETURN                   7

#define MEM_READ                      2      //wangw140928主任务-存储数据读任务
#define MEM_FIRST_POR_DATA_RECOVER    0
#define MEM_SYS_PAR_INFO_RECOVER      1
#define MEM_VALVE_STATE_RECOVER       2
#define MEM_RUN_STATE_RECOVER         3
#define MEM_SYS_TEMP_AND_HUD_RECOVER  4
#define MEM_SYS_FAULT_STATE_RECOVER   5
#define MEM_SYS_RTC_RECOVER           6


#define MEM_WRITE                     3      //wangw140928主任务-存储数据写任务
#define MEM_FIRST_POR_DATA_SAVE       0
#define MEM_SYS_PAR_INFO_SAVE         1
#define MEM_VALVE_STATE_SAVE          2
#define MEM_RUN_STATE_SAVE            3
#define MEM_SYS_TEMP_AND_HUD_SAVE     4
#define MEM_SYS_FAULT_STATE_SAVE      5
#define MEM_SYS_RTC_SAVE              6


#define MEASURE                       4       //wangw140928主任务-测量任务

#define MEASURE_RETURN_AIR_TEMP       1       //回风温度测量
#define MEASURE_HOME_HUMIDITY         2       //室内湿度测量
#define MEASURE_FAN_COIL_TEMP         3       //室内湿度测量



#define RESERVE                       5       //wangw140928主任务-预留任务


#define CONTRL                        6
#define MOTO                          6
#define SYS_MODE_AND_STATE_CTL        0       //工作模式及状态控制任务
#define COLD_VALVE_CTL                1       //冷阀控制子任务
#define HEAT_VALVE_CTL                2       //热阀控制子任务
#define FAN_SPEED_CTL                 3       //风机速度控制子任务
#define HUMDIFICA_CTL                 4       //辅助加湿控制子任务
#define HEATING_CTL                   5       //辅助加热控制子任务




#define OTHER                       7       //wangw140928主任务-其它任务
#define BEEP_TIPS                   1       //蜂鸣器控制子任务
#define LED_TIPS                    2       //LED指示灯控制子任务



/* 任务 */
typedef struct
{
    void (*go_fun)(INT8U prio);                                     //带参数的函数指针 用于执行任务函数
}SYS_TaskStruct;


#ifdef  GLOBAL
#define EXT 
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////  以下定义数据表格  /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const INT8U    SYS_TaskMapTbl[256] = { 
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0  
};



void                            SG_Task(INT8U prio);
void                            COMM_Task(INT8U prio);
void                            MEM_Read_Task(INT8U prio);
void                            MEM_Write_Task(INT8U prio);
void                            Measure_Task(INT8U prio);
void                            Reserve_Task(INT8U prio);
void                            Contrl_Task(INT8U prio);
void                            Other_Task(INT8U prio);

const SYS_TaskStruct  SYS_TaskTbl[8] = {       //任务列表 TASK0优先级最高
    TASK0,
    TASK1,
    TASK2,
    TASK3,
    TASK4,
    TASK5,
    TASK6,
    TASK7
};



#else
#define EXT  extern
EXT const    INT8U              SYS_TaskMapTbl[256];
#ifdef WIN32
EXT          SYS_TaskStruct     SYS_TaskTbl[8];
#else
EXT const    SYS_TaskStruct     SYS_TaskTbl[8]; 
#endif

#endif

//wang140820风机盘管控制器
//wang系统阀门信息
typedef union
{
   //INT8U SYS_Valve_Info;	             //wang系统阀门信息
   struct
   { 
      INT8U  Heating_State;              //wang开关电加热状态   恢复        
      INT8U  Humidification_State;       //wang开关加湿器状态   恢复
      INT8U  Cold_Valve_State;            //wang冷阀状态        恢复
      INT8U  Heat_Valve_State;	          //wang热阀状态        恢复
      INT8U  Num_Of_Valves_State;        //wang阀门数量（0-1个阀门，1-2个阀门）  恢复
 
   };
}SYS_Valve_State_Struct;

//wang141017系统运行状态  0-关 1-开
typedef union
{
   //INT8U SYS_Run_Info;	           //wang系统模式信息
   struct
   { 
      INT8U  Power_State;                 //wang总开关状态 ，开关机（0-关，1-开）     
      INT8U  Energy_Saving_Mode_State;    //wang开关节能模式状态                 恢复 
      INT8U  Work_Mode;                   //wang0-制冷，1-制热，2-通风，3-除湿   恢复      
      INT8U  Fan_Coil_Speed_Mode;         //wang0-高，1-中，2-低，3-自动         恢复
      INT8U  Fan_Coil_Speed_State;        //wang0-高，1-中，2-低  5-关           恢复
      INT8U  Auto_Power_On_State;         //wang开机定时状态  00：关此功能 01一次有效 02一直有效
      INT8U  Auto_Power_On_Time;          //wang时间点   
      INT8U  Auto_Power_Off_State;        //wang关机定时状态  00：关此功能 01一次有效 02一直有效
      INT8U  Auto_Power_Off_Time;         //wang时间点      
  
   };
}SYS_Run_State_Struct;

/* 系统运行的标志 无需存储 */
typedef union
{
  INT32U SYS_Run_Flag;			   //系统标记位1定义
  struct{ 
            INT8U  Broadcast_Comm_Flag:1;  //wang广播通讯标记位
            INT8U  Beep_Operate_Flag:1;    //wang蜂鸣器操作标志位，0：不操作 1：置起一次
            INT8U  Beep_Ring_Fre:2;        //wang蜂鸣器响的次数00：不响 01：一直响 02：响一声 03：响两声
            INT8U  Led_Blanik_Fre:3;       //wang灯的闪烁频率 00:长灭 01:长亮 02:250MS 03：1S 04:2S
            INT8U  SYS_State_Change_Flag:1; //wang握手指令是否返回数据 0：原帧返回 1：带数据返回         
  };
}SYS_Run_Flag_Struct;

//wang141017系统温湿度信息
typedef struct
{     
    INT16U Cooling_Set_Temp;                //wang制冷模式设置温度           恢复
    INT16U Heating_Set_Temp;                //wang制热模式设置温度           恢复
    INT8U  Ctl_Temp_Accuracy;               //wang控温的步进温度             恢复
    INT16U Fan_Coil_Temp;                   //wang当前风机盘管温度           恢复
    INT16S Return_Air_Temp;                 //wang当前回风温度,实际测试温度  恢复
    INT8U  Home_Humidity;                   //wang室内湿度                   恢复
    INT8S  Home_Humidity_Compensation;      //wang室内湿度补偿值             恢复
    INT16S Home_Temp;                       //wang室内温度，加入了补偿值的   恢复
    INT8S  Home_Temp_Compensation;          //wang室内温度补偿值             恢复
    INT8U  Temp_Protect_State;              //wang温度保护状态               恢复
    INT8U  High_Temp_Protect_Value;          //wang高温保护值   运算时需要乘以100  恢复
    INT8U  Low_Temp_Protect_Value;           //wang低温保护值   运算时需要乘以100  恢复 
    
}SYS_TempAndHumidity_Info_Struct;

//wang140820系统故障信息
typedef union
{
   INT8U SYS_Fault_Info;	              //wang系统故障信息
   struct
   { 
      INT8U  Home_Temp_Too_High_Fault: 1;     //wang 室温过高故障     
      INT8U  Home_Temp_Too_Low_Fault: 1;      //wang 室温过低故障     
      INT8U  Return_Air_Temp_Sensor_Fault: 1; //wang 回风温度传感器故障（0-没故障，1-故障）
      INT8U  Humidity_Sensor_Fault: 1;        //wang 湿度传感器故障（0-没故障，1-故障）
      INT8U  Fan_Fault: 1;                    //wang 风机故障（0-没故障，1-故障）            
      INT8U  Memory_Storage_Fault: 1;         //wang 存储器故障（0-没故障，1-故障）            
      INT8U  Fan_Coil_Temp_Sensor_Fault: 1;   //wang 盘管温度传感器故障（0-没故障，1-故障）

   };
}SYS_Fault_State_Struct;

//wang140820系统固定信息
typedef struct
{       
    INT8U TubeQuantity;                     //wang几管制                  
    INT8U Fan_Coil_Controller_ID;           //wang风机盘管控制器地址 
    INT8U Firmware_Version;                 //wang程序固件版本

}SYS_PAR_Info_Struct;


/*实时时钟  6字节*/
typedef struct
{
    INT8U       Second;		                                    //秒
    INT8U       Minute;    		                            //分
    INT8U       Hour;	                                            //时
    INT8U       Day;                                                //日
    INT8U       Month;                                              //月
    INT8U       Year;                                               //wang能表示的范围2000~2225年
//   INT8U       YearL;		                                    //年低位    2000~2255
//   INT8U       YearH;		                            //年高位    

}SYS_RTCStruct;                                   

//***********************************************************
//*******************End wang2013-11-11**************************
//////////////////////////////  以下定义及声明全局变量  ////////////////////////

/////////////////////////////////  系统  ///////////////////////////////////////
//wang风机盘管结构体定义，其中EXT相当于空
EXT NO_INIT  SYS_Valve_State_Struct             SYS_Valve_State;   //用于存储系统基本参数,该结构体保存到EEPROM

EXT NO_INIT  SYS_Run_State_Struct               SYS_Run_State; 

EXT NO_INIT  SYS_Run_Flag_Struct                SYS_Run_Flag;

EXT NO_INIT  SYS_TempAndHumidity_Info_Struct    SYS_TempAndHumidity_Info; //wang

EXT NO_INIT  SYS_Fault_State_Struct             SYS_Fault_State;  //wang

EXT NO_INIT  SYS_PAR_Info_Struct                SYS_PAR_Info;  //wang

EXT NO_INIT  SYS_RTCStruct                      SYS_RTC;         //实时时钟


EXT NO_INIT  INT8U              SYS_SubTask[8];      //系统子任务分支   SYS_SubTask[0]对应最高优先权  
EXT NO_INIT  INT8U              SYS_MainTask;        //系统主任务标志

/////////////  实时时钟  ///////////////
EXT NO_INIT  INT8U              SYS_RTC_Cnt;                           //秒标记 每2个标记为1S

////////////  应用时钟  ////////////////
#define LAST_GLOBAL_ADR         (&SYS_RTC_Cnt + sizeof(SYS_RTC_Cnt)-1)




#endif
