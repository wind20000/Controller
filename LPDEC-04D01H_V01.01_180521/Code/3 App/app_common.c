#include "app_common.h"

void Set_Task(u8 main_task,u8 sub_task)
{
  u16 mm,ss;
  mm = 1<<main_task;
  ss = 1<<sub_task;
  g_Maintask |= mm;
  ga_Subtask[main_task] |= ss;
}

void Clr_Task(u8 main_task,u8 sub_task)
{
    u16 ss;
    if(sub_task == TASK_ALL)
    {
        ga_Subtask[main_task] = 0;
    }
    else
    {        
        ss = 1<<sub_task;
        ga_Subtask[main_task] &= ~ss;
    }
}

u8 clear_task(u8 prio , u8 m_SYS_SubTask_prio)
{
   ga_Subtask[prio] &=~ (1<<m_SYS_SubTask_prio);
   if(ga_Subtask[prio] == 0)
   {
        g_Maintask &=~(1<<prio);
        return TRUE;
   }
   return FALSE;
}


  
/***********************************************************************
函数功能:         BCD码转8位数据 
输入参数:         待转的数据  （输入0x20,输出=20=0x14）
                  		
输出参数:         无
函数返回值说明:   转换结果
使用的资源:    	  无
***********************************************************************/
u8 BCD_2_Data8(u8 data)
{
  u8 result;
  u8 high,low;

  high = data>>4;
  low = (data&0x0f);
  result = high*10 + low;
  return result;
}

/***********************************************************************
函数功能:         8位数据转BCD码 
输入参数:         待转的数据 范围(0~99) 超过99认为是data%100
                  输入62，则输出0x62		
输出参数:         无
函数返回值说明:   转换结果
使用的资源:    	  无
***********************************************************************/

u8 Data8_2_BCD(u8 data)
{

  u8 return_data = 0;
  while(data >= 100)                                //数据最大为99 去除百位数
  {
      data = data - 100;
  }
  while(data >= 10)
  {
      data = data - 10;
      return_data = return_data + 16;
  }  
  return_data = return_data + data;

  return return_data;
}

/***********************************************************************
函数功能:         16位数据转BCD码 
输入参数:         待转的数据 范围(0~9999) 超过9999认为是data%100
                  输入162，则输出0x162		
输出参数:         无
函数返回值说明:   转换结果
使用的资源:    	  无
***********************************************************************/
u16 Data16_2_BCD(u16 data)
{

  u16 return_data = 0;
  while(data >= 10000)                                //数据最大为99 去除百位数
  {
      data = data - 10000;
  }
  while(data >= 1000)
  {
      data = data - 1000;
      return_data = return_data + 16*16*16;
  }  
  while(data >= 100)
  {
      data = data - 100;
      return_data = return_data + 16*16;
  }  
  while(data >= 10)
  {
      data = data - 10;
      return_data = return_data + 16;
  }  
  return_data = return_data + data;

  return return_data;
}

BOOL Check_Strstr(S8 *data1,S8 *data2,uint16_t len)
{
    uint16_t i=0;
    S8 *p;
    p=data2;
	
    do
    {
        if(*data1==*p)
        { 
            p++;
            if(*p=='\0')
            {
                return TRUE;
            }
            
        }
        else
        { 
            p=data2;
        }
		
        data1++;
        if(*data1=='\0')
        {
            return FALSE;
        }
        i++;
    }while(i<len);
    
    return FALSE;
}
//计算前导0
U8 CntLeadZeros(U8 ucData)
{
	U8 ucIndex;
	ucIndex = 0;
	
	while(ucIndex < 8)
	{
		if(ucData & (0x01 << ucIndex))
		{
		    return ucIndex;
		}
		ucIndex++;
	}
	return (U8)0xFF;
}

u8 Get_Comma(u8 num_comma,u8 *buf)
{
	 u8 i=0;
	 u8 j=0;
	 u8 len=0;
	 char str=',';
	 u8 location=0;
	 u8 *p_buf=buf;
	 len=strlen((char *)p_buf);
	 for(i=0;i<num_comma;i++)
	 {
		 
		  while(p_buf[j]!=str)
			{
				 j++;
				 if(j>=len)
				 {
					  return FALSE;
				 }
			}
			j+=1;
			location=j;
	 }
	 return location;
}

u8* DecimalToASCII(u16 port,u8*data)
{
    u8 num,i;
    num = port/10000+0x30;
    data[0] = num;
    num = port%10000/1000+0x30;
    data[1] = num;
    num= port%1000/100+0x30; 
    data[2] = num;
    num = port%100/10+0x30;
    data[3] = num;
    num = port%10+0x30;
    data[4] = num;
    data[5] = 0;
    for(i=0;i<5;i++)
    {
        if(data[i] != 0x30) return (&data[i]);
        
    }
    return NULL;
}

u8 Get_Xor(u8 *hex_data_buf,u16 len_hex)
{
  u8 result=hex_data_buf[0];
  
  for(u8 i=1;i<len_hex;i++){   //帧头也算进去
    result ^=hex_data_buf[i];
  }
  return result;
}


u8 Xor_Check(u8 *hex_data_buf,u8 len_hex)
{
  u8 i=0;
  u8 temp = hex_data_buf[0];
  for(i= 1; i < len_hex-2; i++)   
  {
    temp ^= hex_data_buf[i];
  }
  if((hex_data_buf[i]) == temp )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

U8 Get_Uint_Data(const S8 *buf)
{
	U8 i=0;
	U8 len=0;
	U8 int_data=0;
	U8 byte=0;
	S8 str=',';

	S8 data[20];
	len=strlen((S8 *)buf);
	while(buf[i]!=str)
	{
		data[i]=buf[i];
		i++;
		if(i>=len)
		{
			data[i]='\0';
			int_data=atoi(data);
			byte=(U8)int_data;
			return byte;
		}
	}
	data[i]='\0';
	int_data=atoi(data);
	byte=(U8)int_data;
	return byte;
}


static void asc_To_hex(char *ascs,u8 *hex)
{
  u8 h,l;
  h=l=0;
  if((ascs[0]>=0x30)&&(ascs[0]<=0x39)){
    h=ascs[0]-0x30;
  }
  else if((ascs[0]>=0x41)&&(ascs[0]<=0x46)){
    h=ascs[0]-0x37;//h=ascs_h+10-0x41;
  }
  else if((ascs[0]>=0x61)&&(ascs[0]<=0x66)){
    h=ascs[0]-0x57;//h=ascs_h+10-0x61;
  }

  if((ascs[1]>=0x30)&&(ascs[1]<=0x39)){
    l=ascs[1]-0x30;
  }
  else if((ascs[1]>=0x41)&&(ascs[1]<=0x46)){
    l=ascs[1]-0x37;//l=ascs_l+10-0x41;
  }
  else if((ascs[1]>=0x61)&&(ascs[1]<=0x66)){
    l=ascs[1]-0x57;//l=ascs_l+10-0x61;
  }
  
  *hex = (h<<4)|l;
  return;
} 

u16 Ascii_To_Hex(char *ascs,u8 *hexs,u16 length)
{
  u16 i;
  u16 j;
  
  for(i=0,j=0;i<length;i+=2){
    asc_To_hex(ascs+i,hexs+j);
    j++;
  }
  return j;
}


