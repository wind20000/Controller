#ifndef __APP_COMMON_H
#define __APP_COMMON_H

#include "global.h"

#define TASK_ALL                       0xff

void Set_Task(u8 main_task,u8 sub_task);
void Clr_Task(u8 main_task,u8 sub_task);
u8 clear_task(u8 prio , u8 m_SYS_SubTask_prio);
u8 BCD_2_Data8(u8 data);
u8 Data8_2_BCD(u8 data);
u16 Data16_2_BCD(u16 data);

BOOL Check_Strstr(S8 *data1,S8 *data2,uint16_t len);
U8 CntLeadZeros(U8 ucData);
u8 Get_Comma(u8 num_comma,u8 *buf);
u8* DecimalToASCII(u16 port,u8*data);
u8 Get_Xor(u8 *hex_data_buf,u16 len_hex);
u8 Xor_Check(u8 *data,u8 length);
U8 Get_Uint_Data(const S8 *buf);
u16 Ascii_To_Hex(char *ascs,u8 *hexs,u16 length);


#endif


















