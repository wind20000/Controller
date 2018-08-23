#ifndef __BSP_RTC_H
#define __BSP_RTC_H

#include "global.h"

#define RTC_LOCATION_UPLOAD_TIME  (6)
#define RTC_VOLT_TEMP_UPLOAD      (3)
#define RTC_SAMPLE_TIME           (3) 
//#define RTC_VOLT_TEMP_UPLOAD      (VOLT_TEMP_UPLOAD/5)
//#define RTC_SAMPLE_TIME           (SAMPLE_TIME/5) 

typedef struct 
{
    u16 sample_count;
    u16 volt_temp_count;   
    u16 trs_sear_gps_count;     
}Rtc_Type;

extern Rtc_Type Rtc_Struct;

void RTC_CLOCK_Init(void);
void NO_NET_RTC_CLOCK_Init(void);
void Halt_To_Wakeup(void);

#endif





