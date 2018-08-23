#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "global.h"

/******  LED GPIO  DEFINE  OUTPUT ******/
#define  LED_PORT                              GPIOC
#define  LED_PIN                               GPIO_Pin_7
#define  BSP_LED_LIGHT                         GPIO_ResetBits(LED_PORT ,LED_PIN);  
#define  BSP_LED_DARK                          GPIO_SetBits(LED_PORT ,LED_PIN);
#define  BSP_LED_FLICKER                       GPIO_ToggleBits(LED_PORT, LED_PIN);

#define  KLED_PORT                              GPIOD
#define  KLED_PIN                               GPIO_Pin_7
#define  BSP_KLED_LIGHT                         GPIO_ResetBits(KLED_PORT ,KLED_PIN);  
#define  BSP_KLED_DARK                          GPIO_SetBits(KLED_PORT ,KLED_PIN);
#define  BSP_KLED_FLICKER                       GPIO_ToggleBits(KLED_PORT, KLED_PIN);


/****** KEY GPIO  ******/
#define  KEY_PORT                              GPIOG
#define  KEY_PIN                               GPIO_Pin_5
#define  BSP_KEY_READ                          GPIO_ReadInputDataBit(KEY_PORT , KEY_PIN)

/********¼ÌµçÆ÷¿ØÖÆ*******/
#define  RELAY_CTROL_PORT                      GPIOB
#define  RELAY_CTROL_PIN                       GPIO_Pin_1
#define  BSP_RELAY_CTROL_SET                   GPIO_SetBits(RELAY_CTROL_PORT ,RELAY_CTROL_PIN);                                           
#define  BSP_RELAY_CTROL_RESET                 GPIO_ResetBits(RELAY_CTROL_PORT ,RELAY_CTROL_PIN);
#define  BSP_RELAY_CTROL_READ                  GPIO_ReadInputDataBit(RELAY_CTROL_PORT , RELAY_CTROL_PIN)


                                                
/*****  GPRS GPIO ******/
#define  GPRS_BUCK_EN_PORT                     GPIOG
#define  GPRS_BUCK_EN_PIN                      GPIO_Pin_4
#define  BSP_GPRS_BUCKEN_SET                   GPIO_SetBits(GPRS_BUCK_EN_PORT ,GPRS_BUCK_EN_PIN);                                           
#define  BSP_GPRS_BUCKEN_RESET                 GPIO_ResetBits(GPRS_BUCK_EN_PORT ,GPRS_BUCK_EN_PIN);

#define  GPRS_POWER_ON_PORT                    GPIOE
#define  GPRS_POWER_ON_PIN                     GPIO_Pin_6
#define  BSP_GPRS_POWERON_SET                  GPIO_SetBits(GPRS_POWER_ON_PORT ,GPRS_POWER_ON_PIN);                                           
#define  BSP_GPRS_POWERON_RESET                GPIO_ResetBits(GPRS_POWER_ON_PORT ,GPRS_POWER_ON_PIN);

#define  GPRS_WAKE_UP_PORT                     GPIOB
#define  GPRS_WAKE_UP_PIN                      GPIO_Pin_6

                                                
void Bsp_GPIO_Init(void);

#endif










