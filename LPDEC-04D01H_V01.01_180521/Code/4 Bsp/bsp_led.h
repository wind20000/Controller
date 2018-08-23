#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "global.h"


#define  KLED_ROLLBACK_PERIOD        200UL
#define  KLED_ROLLBACK_TIMES         (8)

typedef enum
{
  SYS_INDI = 0,
  CONN_INDI= 1,
  DATA_SEND_INDI = 2,
  
}LED_Status_Tpye;

typedef struct
{
   U16 usCount;
   U8  ucRollbackCnt;
}LED_Tpye;

typedef struct
{
   U16 usCount;
   U8  ucFlicker_F;
   U8  ucRollbackCnt;
   
}KEY_LED_Tpye;

extern KEY_LED_Tpye         KEY_LED_Struct;
extern LED_Status_Tpye      LED_Status;

void Bsp_Led_Tim(U16 usPeriod);

#endif













