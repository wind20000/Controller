#include "bsp_key.h"

KEY_Tpye KEY_Struct = {0};

static void Key_Limit(U16 usPeriod)
{
    if(KEY_Struct.ucKeyInvalid_F != DISABLE)
    {
        KEY_Struct.usKeyLimitCnt+=usPeriod;
        if(KEY_Struct.usKeyLimitCnt >= KEY_INVALID_TIME)
        {
                KEY_Struct.usKeyLimitCnt = 0;
                KEY_Struct.ucKeyInvalid_F = DISABLE;
        }
    }    
}
static U8 Bsp_Key_Scan(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
{
  /*
    U8 i=0;
    while(GPIO_ReadInputDataBit(GPIOx , GPIO_Pin) == 0)
    {
       Bsp_Tim3_Delay_Ms(20);
       i++;
       if(i>=3)  return TRUE;       
    }
  */
     
    if(GPIO_ReadInputDataBit(GPIOx , GPIO_Pin) == 0)
    {
        
        Bsp_Tim3_Delay_Ms(20);
        if(GPIO_ReadInputDataBit(GPIOx , GPIO_Pin) == 0)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

void Bsp_Key_Tim(U16 usPeriod)
{
    Key_Limit(usPeriod);
}


void Key_Tips(void)
{
    u8 keyValue = Bsp_Key_Scan(KEY_PORT,KEY_PIN);
    if(keyValue == TRUE) //按键按下，哪么置起人工呼叫事件
    {        
         KEY_LED_Struct.ucFlicker_F = ENABLE;  /*按键无效期间,按键LED灯需闪烁*/
         if(DISABLE == KEY_Struct.ucKeyInvalid_F)
         {         
            KEY_Struct.ucKeyInvalid_F = ENABLE;
            GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_CALL);
         }
         
    }
}














