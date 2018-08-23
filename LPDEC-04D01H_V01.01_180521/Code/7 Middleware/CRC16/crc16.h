#ifndef CRC16_H
#define CRC16_H

#include "global.h"

#if defined(__cplusplus)
   extern "C" {
#endif

u16 FitCRC_Get16(u16 crc, u8 byte);
u16 FitCRC_Update16(u16 crc, const volatile void *data, u32 size);
u16 FitCRC_Calc16(const volatile void *data, u32 size);

#if defined(__cplusplus)
   }
#endif

#endif 
