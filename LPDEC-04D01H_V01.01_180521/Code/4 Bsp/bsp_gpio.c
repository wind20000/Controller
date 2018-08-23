#include "bsp_gpio.h"

static void gpio_led_init(void)  //初始LED都点亮
{
    GPIO_Init(LED_PORT , LED_PIN , GPIO_Mode_Out_PP_Low_Fast);  
    GPIO_Init(KLED_PORT , KLED_PIN , GPIO_Mode_Out_PP_Low_Fast);
}

static void gpio_key_init(void)
{
    GPIO_Init(KEY_PORT , KEY_PIN , GPIO_Mode_In_FL_IT);            //初始化KEY
    EXTI_SetPinSensitivity(EXTI_Pin_5 , EXTI_Trigger_Falling);     //line interrupt

//    EXTI_SetPortSensitivity(EXTI_Port_G ,  EXTI_Trigger_Falling);   //port interrupt                  
//    EXTI_SetHalfPortSelection(EXTI_HalfPort_F_MSB , ENABLE);
}

static void gpio_relay_init(void)
{
    GPIO_Init(RELAY_CTROL_PORT , RELAY_CTROL_PIN , GPIO_Mode_Out_PP_Low_Fast);
}

/******** GPRS GPIO INIT ********************/
static void gpio_gprs_init(void)
{
    GPIO_Init(GPRS_BUCK_EN_PORT ,  GPRS_BUCK_EN_PIN  , GPIO_Mode_Out_PP_High_Fast);
    //初始拉低，如果拉高，那么模块上电后的at reday的推送可能由于正在读写EEPROM而接受不到
    GPIO_Init(GPRS_POWER_ON_PORT , GPRS_POWER_ON_PIN , GPIO_Mode_Out_PP_Low_Fast); 
    GPIO_Init(GPRS_WAKE_UP_PORT  , GPRS_WAKE_UP_PIN  , GPIO_Mode_Out_PP_High_Fast);
}

void Bsp_GPIO_Init(void)
{
    gpio_led_init();
    gpio_key_init();
    gpio_relay_init();
    gpio_gprs_init();
}


