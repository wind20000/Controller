#include "bsp_led.h"
LED_Status_Tpye     LED_Status = SYS_INDI;
LED_Tpye            LED_Struct={0};
KEY_LED_Tpye        KEY_LED_Struct = {0};

static void Led_Flicker(U16 usPeriod,U16 halfPeriod,U8 times)
{
    U8 FlickerTimes = times*2;
    LED_Struct.usCount +=usPeriod;
    if(LED_Struct.usCount >= halfPeriod)
    {
        BSP_LED_FLICKER;
        LED_Struct.usCount = 0;        
        if(times > 0)
        {
            LED_Struct.ucRollbackCnt++;
            if(LED_Struct.ucRollbackCnt >= FlickerTimes)
            {
                LED_Struct.ucRollbackCnt = 0;
                LED_Status = SYS_INDI;         /*通讯结束后返回系统LED指示*/
            }
        }
     }
    
   
}


static void Key_Led_Flicker(U16 usPeriod)
{    
    if(ENABLE == KEY_LED_Struct.ucFlicker_F)
    {        
        KEY_LED_Struct.usCount +=usPeriod;
        if(KEY_LED_Struct.usCount >=KLED_ROLLBACK_PERIOD)
        {
            BSP_KLED_FLICKER;
            KEY_LED_Struct.usCount = 0;
            KEY_LED_Struct.ucRollbackCnt++; 
            if(KEY_LED_Struct.ucRollbackCnt >= KLED_ROLLBACK_TIMES)
            {
                 KEY_LED_Struct.ucRollbackCnt = 0;
                 KEY_LED_Struct.ucFlicker_F = DISABLE;
            }
        }
    }
    
}

static void Led_Indicate(U16 usPeriod)
{
    static U8 sucFlag=0; 
    switch(LED_Status)
    {
        case SYS_INDI:
        {
             BSP_LED_LIGHT;   
        }
        break;
        case CONN_INDI:
        {
             sucFlag = ENABLE;
             Led_Flicker(usPeriod,1000,0);   //第三项参数为0，表示闪烁次数不限
        }
        break;
        case DATA_SEND_INDI:
        {
             /*当网络连接状态过渡到通讯状态时，LED_Struct.usCount变量可能不为0*/
             if(sucFlag != DISABLE)        
             {
                LED_Struct.usCount = 0;
                sucFlag = DISABLE;
             }
             Led_Flicker(usPeriod,200,4);    
        }
        break;
        default:
        break;
     }

}


static void Key_Led_Indicate(U16 usPeriod)
{
    Key_Led_Flicker(usPeriod);
}

void Bsp_Led_Tim(U16 usPeriod)
{
    Led_Indicate(usPeriod);
    Key_Led_Indicate(usPeriod);
}






