#include "bsp_rtc.h"

Rtc_Type Rtc_Struct;

/****************************************************

外部LSE  32768Hz
32768/64/16=32Hz

160/32=5S
1920/32=60S
****************************************************/
void RTC_CLOCK_Init(void)
{
    CLK_SYSCLKSourceSwitchCmd(ENABLE);

    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    while(!CLK_GetFlagStatus(CLK_FLAG_HSIRDY));
    CLK_RTCClockConfig(CLK_RTCCLKSource_HSI ,
                       CLK_RTCCLKDiv_64
                       );
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC , ENABLE);

    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);

    RTC_ITConfig(RTC_IT_WUT , ENABLE);
    //RTC_SetWakeUpCounter(1600);   //32768/64/16=32    160/32=5
    RTC_WakeUpCmd(ENABLE);     //使能RTC从低功耗等待模式呼醒
}

void NO_NET_RTC_CLOCK_Init(void)
{
    CLK_SYSCLKSourceSwitchCmd(ENABLE);
    CLK_LSEConfig(CLK_LSE_ON);
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSE);
    while(!CLK_GetFlagStatus(CLK_FLAG_LSERDY));
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSE ,
                       CLK_RTCCLKDiv_64
                       );
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC , ENABLE);

    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);

    RTC_ITConfig(RTC_IT_WUT , ENABLE);
    RTC_SetWakeUpCounter(19200);   //32768/64/16=32    19200/32=600S
    RTC_WakeUpCmd(ENABLE);     //使能RTC从低功耗等待模式呼醒
}

























