#include "bsp_gprs.h"


void G510_Buck_EN(void)
{
    BSP_GPRS_BUCKEN_SET;
}

/*************************
    G510 Power on流程：
    ___         ___
       |_______|
         800ms
  电路转换 高->低；低->高
**************************/
void G510_Power_ON(void)
{
    BSP_GPRS_POWERON_SET;
    Bsp_Tim3_Delay_Ms(850);
    BSP_GPRS_POWERON_RESET;
}

void Bsp_G510_Init(void)
{
    G510_Buck_EN();
    G510_Power_ON();
}

/*************************
    G510 Power off流程：
    关闭所有应用接口和注销网络
    ___         ___
       |_______|
          3s
  电路转换 高->低；低->高
**************************/
void G510_Power_OFF(void)
{
    GPIO_SetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);
    Bsp_Tim3_Delay_Ms(3500);
    GPIO_ResetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);
}





