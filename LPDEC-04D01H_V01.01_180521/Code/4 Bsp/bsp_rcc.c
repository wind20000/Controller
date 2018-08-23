#include "bsp_rcc.h"

void Bsp_Rcc_Init(void)
{
   CLK_DeInit();  
   CLK_HSICmd(ENABLE);
   CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
   CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
//   CLK_CCOConfig(CLK_CCOSource_HSI,CLK_CCODiv_1);
//   CLK_SYSCLKSourceSwitchCmd(ENABLE);
//   CLK_ClockSecuritySystemEnable();
   
} 

void Use_HSI_Clock(void)
{
   CLK_HSICmd(ENABLE);
   CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
   CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
   CLK_SYSCLKSourceSwitchCmd(ENABLE);
}

void Use_LSE_Clock(void)
{
   CLK_LSEConfig(CLK_LSE_ON);
   CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
   CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSE);
   while(!CLK_GetFlagStatus(CLK_FLAG_LSERDY));
   CLK_SYSCLKSourceSwitchCmd(ENABLE);
   CLK_ClockSecuritySystemEnable();
}