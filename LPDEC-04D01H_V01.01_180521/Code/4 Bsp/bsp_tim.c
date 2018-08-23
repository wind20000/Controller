#include "bsp_tim.h"

static U16 s_MsCount=0;;

Tim_Type Tim_Struct;

void Bsp_Tim5_Init(void)
{
 	CLK_PeripheralClockConfig(CLK_Peripheral_TIM5 , ENABLE);              //使能定时器5时钟
  	TIM5_TimeBaseInit(TIM5_Prescaler_128 , TIM5_CounterMode_Up , 16000);    //设置定时器5为128分频，向上计数，计数值为16000即为7.8125秒的计数值
  	TIM5_ITConfig(TIM5_IT_Update , ENABLE);     //使能向上计数溢出中断
  	TIM5_ARRPreloadConfig(ENABLE);  //使能定时器5自动重载功能
  	TIM5_Cmd(ENABLE);               //启动定时器5开始计数
}

void Bsp_Tim2_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2 , ENABLE);              //使能定时器2时钟
    TIM2_TimeBaseInit(TIM2_Prescaler_16 , TIM2_CounterMode_Up , 10000);    //设置定时器2为16分频，向上计数，计数值为10000即为10毫秒的计数值
    TIM2_ITConfig(TIM2_IT_Update , ENABLE);     //使能向上计数溢出中断
    TIM2_ARRPreloadConfig(ENABLE);  //使能定时器2自动重载功能
    TIM2_Cmd(ENABLE);               //启动定时器2开始计数
}


void Bsp_Tim3_Delay_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3 , ENABLE);              //使能定时器3时钟
    TIM3_TimeBaseInit(TIM3_Prescaler_1 , TIM3_CounterMode_Up , 16000);    //设置定时器3为1分频，向上计数，计数值为16000即为1毫秒的计数值
}

void Bsp_Tim3_Delay_Ms(u16 ms)
{
    TIM3_ARRPreloadConfig(ENABLE);  //使能定时器2自动重载功能
    TIM3_Cmd(ENABLE);               //启动定时器2开始计数
    while(ms--)
    {
        while( TIM3_GetFlagStatus(TIM3_FLAG_Update) == RESET); //等待计数是否达到1毫秒
        TIM3_ClearFlag(TIM3_FLAG_Update);  //计数完成1毫秒，清除相应的标志
    }
    TIM3_Cmd(DISABLE);                   //延时全部结束，关闭定时器2
}


void Bsp_Tim_Update(void)   //10MS执行周期
{
    s_MsCount++;
    if(Task_Flag_Struct.closeStaProc == DISABLE)
    {
        APP_GPRS_TimeProc(10);
    }
    Bsp_Led_Tim(10);
    Bsp_Key_Tim(10);
    if(s_MsCount >= MS_TO_S)     // 1S时间到
    {
         s_MsCount =0;
        // BSP_LED_FLICKER;         
         if(Task_Flag_Struct.writeRom_F != DISABLE || Task_Flag_Struct.writeDynamic_F != DISABLE)
         {
              Tim_Struct.rewriteCount++;
              if(Tim_Struct.rewriteCount >= 180)   //如写未成功，那么间隔3MIN，重写
              {
                   Tim_Struct.rewriteCount = 0; 
                   if(Task_Flag_Struct.writeRom_F != DISABLE)
                   {
                        Set_Task(MEM_WRITE,MEM_STORE_SOLID_ROMDATA);
                   }
                   if(Task_Flag_Struct.writeDynamic_F != DISABLE)
                   {
                        Set_Task(MEM_WRITE,MEM_STORE_DYNAMIC_DATA);
                   }
              
              }
         }
         if(Task_Flag_Struct.closeStaProc != DISABLE) //重新打开GPRS状态处理计数
         {
              Tim_Struct.reOpenStaProcCount++;
              if(Tim_Struct.reOpenStaProcCount >= RE_OPEN_STA_PROC_TIME)
              {
                   Tim_Struct.reOpenStaProcCount = 0;
                   Task_Flag_Struct.closeStaProc = DISABLE;
              }
         }
    }
}















