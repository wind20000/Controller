/******************************************************************
** 版权:       杭州利拍档科技有限公司
** 文件名:     global.h
** 版本：
** 工作环境:   IAR 1.42
** 作者:       li
** 生成日期:   2017.8.18
** 功能:       本文件主要用于结构体定义，不需更改的宏定义以及文件包含
** 相关文件:
** 修改日志:
*******************************************************************/
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "main.h"


/********************  Task Layer  *******************/
extern    u8        g_Maintask;        //系统主任务
extern    u8        ga_Subtask[8];     //系统子任务

#define Var_Fill(var,data)      for(u16 ii = 0; ii < sizeof(var); ++ii)\
                                *(((u8*)(&var))+ii) = data              //向变量var中填充data


#define NumberOfElements(x) (sizeof(x)/sizeof((x)[0]))
#define TmrSecondsToMilliseconds( n )   ( (U32) ((n) * 1000) )

                                  
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))


#define SYS_SoftVerMessLen   GPRS_CommPacket.MessRead.softwareVerLen
#define SYS_SoftVerMess      GPRS_CommPacket.MessRead.softwareVer
#define SYS_HardVerMessLen   GPRS_CommPacket.MessRead.hardwareVerLen
#define SYS_HardVerMess      GPRS_CommPacket.MessRead.hardwareVer

typedef struct
{
  u8  writeRom_F        :1;    //如EEPROM写失败，则置起该标志
  u8  writeDynamic_F    :1;
  u8  atReday_F         :1;    //如GPRS 模块AT已准备OK,那么置起该标志
  u8  closeStaProc      :1;    //如模块复位次数到达上限，那么暂时关闭GPRS状态处理，间隔一定时间再去执行
  u8  reserve           :4;
  
}Task_Flag_Type;


extern Task_Flag_Type Task_Flag_Struct;


#endif



