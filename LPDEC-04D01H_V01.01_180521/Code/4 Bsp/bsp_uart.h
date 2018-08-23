#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "global.h"

void Uart1_SendData(u8  data);
void Uart3_SendData(u8  data);
void Bsp_UART1_Init(u32 baudrate);
void Bsp_UART3_Init(u32 baudrate);
void BSP_LOG(char *data );
void BSP_LOG_Value(s32 data);
void BSP_UART_Transmit(U8* pTxBuff, U8 usLen);

#endif