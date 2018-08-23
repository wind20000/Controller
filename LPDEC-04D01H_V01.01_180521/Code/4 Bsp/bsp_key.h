#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "global.h"

#define KEY_INVALID_TIME    5000UL

typedef struct
{
   U8  ucKeyInvalid_F;  
   U16 usKeyLimitCnt;   
   
}KEY_Tpye;

void Bsp_Key_Tim(U16 usPeriod);

void Key_Tips(void);

#endif























