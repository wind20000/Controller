#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include "global.h"


#define MS_TO_S                 (100)
#define REWRITE_TIME            (180)
#define RE_OPEN_STA_PROC_TIME   (120UL)
typedef struct 
{
    U16 rtyCount;
    U16 rewriteCount;
    U16 reOpenStaProcCount;
    
}Tim_Type;


void Bsp_Tim2_Init(void);
void Bsp_Tim3_Delay_Init(void);
void Bsp_Tim3_Delay_Ms(u16 ms);
void Bsp_Tim5_Init(void);
void Bsp_Tim_Update(void);


#endif







