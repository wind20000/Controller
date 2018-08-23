#include "bsp_uart.h"


/*******************************************************************************
**函数名称：void UART1_Init(s32 int baudrate)
**功能描述：初始化USART模块
**入口参数：s32 int baudrate  -> 设置串口波特率
**输出：无
*******************************************************************************/
void Bsp_UART1_Init(u32 baudrate)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , ENABLE);  //使能USART1时钟
  GPIO_Init(GPIOC , GPIO_Pin_2 , GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOC , GPIO_Pin_3 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART1,                //设置USART1
            baudrate,               //流特率设置
            USART_WordLength_8b,    //数据长度设为8位
            USART_StopBits_1,       //1位停止位
            USART_Parity_No,        //无校验
            (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));  //设置为发送接收双模式
  //使能接收中断
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART1 , ENABLE);   //使能USART1模块
}

#if DEBUG_CONTROLLER
void Bsp_UART3_Init(u32 baudrate)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3 , ENABLE);  //使能USART2时钟
//  GPIO_Init(GPIOG , GPIO_Pin_0 , GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOG , GPIO_Pin_1 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART3,                //设置USART2
            baudrate,               //流特率设置
            USART_WordLength_8b,    //数据长度设为8位
            USART_StopBits_1,       //1位停止位
            USART_Parity_No,        //无校验
            (USART_Mode_TypeDef)(USART_Mode_Tx));  //设置为发送接收双模式
  //使能接收中断
//  USART_ITConfig(USART3, USART_IT_RXNE , ENABLE);
//  USART_ITConfig(USART2, USART_IT_IDLE , ENABLE);
  USART_Cmd(USART3 , ENABLE);   //使能USART2模块
}
#endif

void Uart1_SendData(u8  data)
{
    while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);        //判断发送数据寄存器是否为空
    USART_SendData8(USART1 , (u8)data);                     //向发送寄存器写入数据
}
#if DEBUG_CONTROLLER
void Uart3_SendData(u8  data)
{
    while(USART_GetFlagStatus(USART3 , USART_FLAG_TXE) == RESET);        //判断发送数据寄存器是否为空
    USART_SendData8(USART3 , (u8)data);                     //向发送寄存器写入数据
}
void BSP_LOG(char *data )
{
  s32  datalen;
  u16 len=0;
  char *p_buf=data;
  len=strlen(p_buf);
  for(datalen = 0 ; datalen < len ; datalen++)
  {
    Uart3_SendData(data[datalen]);
  }
}

static char* itoa(s32 num,char* str,s32 radix)
{
    /*索引表*/
    char index[]="0123456789ABCDEF";
    s32 unum;/*中间变量*/
    s32 i=0,j,k;
    /*确定unum的值*/
    if(radix==10&&num<0)/*十进制负数*/
    {
        unum=(s32)-num;
        str[i++]='-';
    }
    else unum=(s32)num;/*其他情况*/
    /*转换*/
    do{
        str[i++]=index[unum%(s32)radix];
        unum/=radix;
    }while(unum);
    str[i]='\0';
    /*逆序*/
    if(str[0]=='-')k=1;/*十进制负数*/
    else
      k=0;
    char temp;
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}
void BSP_LOG_Value(s32 data)
{
    s32 datalen;
    char buff[11];
    u16 len=0;
    itoa(data,buff,10);
    len=strlen(buff);
    for(datalen = 0 ; datalen < len ; datalen++)
    {
        Uart3_SendData(buff[datalen]);
    }
}
#endif

void BSP_UART_Transmit(U8* pTxBuff, U8 usLen)
{
    unsigned int datalen;
    
    for(datalen = 0 ; datalen < usLen ; datalen++)
    {
       Uart1_SendData(pTxBuff[datalen]);
    }

}


