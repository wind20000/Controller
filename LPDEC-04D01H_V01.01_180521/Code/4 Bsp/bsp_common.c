#include "bsp_common.h"

Task_Flag_Type Task_Flag_Struct;


void Bsp_Peripheral_Init(void)
{
    Bsp_Rcc_Init();
    Bsp_GPIO_Init();
    Bsp_Tim2_Init();
    Bsp_Tim3_Delay_Init();
    Bsp_UART1_Init(115200);
#if DEBUG_CONTROLLER
    Bsp_UART3_Init(115200);
#endif
    IWDG_Init();
}


u8 Com_Bsp_Init(void)
{
    disableInterrupts();        //close system interrupts
    Bsp_Peripheral_Init();      // peripheral init
    enableInterrupts();         //open system interrupts
    Bsp_Var_Init();             //ver init
    Solid_Romdata_Init();       //data recover
    Dynamic_Init();
    APP_COMM_Init();            //COMM state and event init
    return TRUE;
}

































