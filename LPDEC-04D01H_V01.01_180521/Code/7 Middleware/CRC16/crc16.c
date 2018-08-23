#include "crc16.h"

u16 FitCRC_Get16(u16 crc, u8 byte)
{
   static const u16 crc_table[16] =
   {
      0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
      0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
   };
   u16 tmp;

   // compute checksum of lower four bits of byte
   tmp = crc_table[crc & 0xF];
   crc  = (crc >> 4) & 0x0FFF;
   crc  = crc ^ tmp ^ crc_table[byte & 0xF];

   // now compute checksum of upper four bits of byte
   tmp = crc_table[crc & 0xF];
   crc  = (crc >> 4) & 0x0FFF;
   crc  = crc ^ tmp ^ crc_table[(byte >> 4) & 0xF];
   
   return crc;
}

u16 FitCRC_Update16(u16 crc, const volatile void *data, u32 size)
{
   u8 *data_ptr = (u8 *)data;

   while (size)
   {
      crc = FitCRC_Get16(crc, *data_ptr);
      data_ptr++;
      size--;
   }

   return crc;
}

u16 FitCRC_Calc16(const volatile void *data, u32 size)
{
   return FitCRC_Update16(0, data, size);
}

