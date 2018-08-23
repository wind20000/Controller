#include "bsp_mem.h"

void unlock_PROG(void)
{
    do{
        FLASH->PUKR = 0x56;
        FLASH->PUKR = 0xAE;
    }while((FLASH->IAPSR & 0x02)==0);
}

void lock_PROG(void)
{
    FLASH->IAPSR &= (~0x02);
}

__ramfunc void FLASH_Erase_Block(u32 Address)
{
    u8 __far *pwFlash = (__far u8 *)Address;
    FLASH->CR2 |= 0x20;   
    for(u16 i=0;i<128;i+=4)
    {        
             /* Enable erase block mode */       
             //*((__far u32*)Address) = (u32)0;
             *(pwFlash+i) = 0;
             *(pwFlash+1+i) = 0;
             *(pwFlash+2+i) = 0;
             *(pwFlash+3+i) = 0;
    }           
}
/*******************************************************************************************************/
/*******************************分界线以上函数没用到***********************************/
/*******************************************************************************************************/
/*******************************************************************************************************/



static void Unlock_DATA(void)
{
    do
    {
        FLASH->DUKR = 0xAE;                          
        FLASH->DUKR = 0x56;
    }while((FLASH->IAPSR & 0x08)==0);
}

static void Lock_DATA(void)
{
    FLASH->IAPSR &= (~0x08);
}

static bool  Erase_Page(const u8 pg_num)
{
    u8 temp;
    u16 i=0;
    u32 WriteAddr = (PAGE_QUANTITY * pg_num);
    Unlock_DATA();
    for (i=0;i<PAGE_QUANTITY;i++)
    { 
		*(volatile u8*)(EEPROM_BASE_ADDRESS+WriteAddr) = 0; 
		WriteAddr++;  
    }
    Bsp_Tim3_Delay_Ms(5);
    for(i=0;i<PAGE_QUANTITY;i++)
    {
          	temp=*(volatile u8*)(EEPROM_BASE_ADDRESS+WriteAddr);    
		    WriteAddr++; 
            if(temp !=0 )
            {
              return FALSE;   
            }
    }
    Lock_DATA();
    return TRUE;
}


static void  Read_Romdata(const u8 pg_num,Rom_Data_Type* p_rom_data_struct)
{  
	u16 i;
	u32 ReadAddr = (PAGE_QUANTITY * pg_num);    
    Unlock_DATA();
	for (i=0;i<PAGE_QUANTITY;i++)
        { 
		     p_rom_data_struct->data[i]=*(volatile u8*)(EEPROM_BASE_ADDRESS+ReadAddr); 
		     ReadAddr++;  
        }
    Lock_DATA();
}

static void  Write_Romdata(const u8 pg_num,const Rom_Data_Type* p_rom_data_struct)
{
	u16 i;
    
	u32 WriteAddr = (PAGE_QUANTITY * pg_num);
    Unlock_DATA();
	for (i=0;i<PAGE_QUANTITY;i++)
        { 
		    *(volatile u8*)(EEPROM_BASE_ADDRESS+WriteAddr) = p_rom_data_struct->data[i]; 
		    WriteAddr++;  
        }  
    Lock_DATA();
}

static void  Read_DynamicData(const u8 pg_num,Dynamic_Mess_Type* p_dynamic_mess_struct)
{  
	u16 i;
	u32 ReadAddr = (PAGE_QUANTITY * pg_num);    
    Unlock_DATA();
	for (i=0;i<DYNAMIC_DATA_LEN;i++)
    { 
		p_dynamic_mess_struct->data[i]=*(volatile u8*)(EEPROM_BASE_ADDRESS+ReadAddr); 
		ReadAddr++;  
    }
    Lock_DATA();
}

static void  Write_DynamicData(const u8 pg_num,const Dynamic_Mess_Type* p_dynamic_mess_struct)
{
	u16 i;
    
	u32 WriteAddr = (PAGE_QUANTITY * pg_num);
    Unlock_DATA();
	for (i=0;i<DYNAMIC_DATA_LEN;i++)
    { 
		*(volatile u8*)(EEPROM_BASE_ADDRESS+WriteAddr) = p_dynamic_mess_struct->data[i]; 
		WriteAddr++;  
    }  
    Lock_DATA();
}

static bool _Store_DynamicData(const uint32_t pg_num, const Dynamic_Mess_Type* p_dynamic_mess_struct)
{
	Dynamic_Mess_Type   dynamic_mess_temp_struct;	
	Dynamic_Mess_Type*  p_dynamic_mess_temp_struct = &dynamic_mess_temp_struct;  
    disableInterrupts();
    Write_DynamicData(pg_num, p_dynamic_mess_struct);
    Bsp_Tim3_Delay_Ms(1);
    Read_DynamicData(pg_num, p_dynamic_mess_temp_struct);
    enableInterrupts();
	if((p_dynamic_mess_temp_struct->s_Dynamic_Data.writed1 == 0xaa)
	    && (p_dynamic_mess_temp_struct->s_Dynamic_Data.writed2 == 0xaa)
	    && (Xor_Check((uint8_t*)(&(p_dynamic_mess_temp_struct->s_Dynamic_Data.s_Device_Status)),(sizeof(Device_Status_Type)+2)) == TRUE) 
	  )
	{	
		return TRUE;
	}
		return FALSE;	  
}

static bool Write_Solid_Romdata(const uint32_t pg_num, const Rom_Data_Type* p_solid_data_struct)
{
	Rom_Data_Type   rom_data_temp_struct;	
	Rom_Data_Type* p_rom_data_temp_struct = &rom_data_temp_struct;  
    disableInterrupts();
    Write_Romdata(pg_num, p_solid_data_struct);
    Bsp_Tim3_Delay_Ms(1);
    Read_Romdata(pg_num, p_rom_data_temp_struct);
    enableInterrupts();
	if((p_rom_data_temp_struct->solid_data_cell_struct.writed1 == 0xaa)
	    && (p_rom_data_temp_struct->solid_data_cell_struct.writed2 == 0xaa)
	    && (Xor_Check((uint8_t*)(&(p_rom_data_temp_struct->solid_data_cell_struct.solid_message_data_struct)),(sizeof(Solid_Message_Data_Type)+2)) == TRUE) 
	    && (Xor_Check((uint8_t*)(&(p_rom_data_temp_struct->solid_data_cell_struct.solid_param_data_struct)),(sizeof(Solid_Parameters_Data_Type)+2)) == TRUE) 
	  )
	{	
		return TRUE;
	}
		return FALSE;	  
}


static void Refresh_Message_Data(Rom_Data_Type* p_rom_data_struct,u8 mode)
{
    u8 i=0;
    if(mode == READ_DATA)
    {
        for(i=0;i<4;i++)
        {
            SYS_HardVerMess[i] = p_rom_data_struct->solid_data_cell_struct.solid_message_data_struct.terminal_hardware[i];
        }
        for(i=0;i<4;i++)
        {
            SYS_SoftVerMess[i] = p_rom_data_struct->solid_data_cell_struct.solid_message_data_struct.terminal_software[i];
        }
    }
    else
    {
        for(i=0;i<4;i++)
        {
            p_rom_data_struct->solid_data_cell_struct.solid_message_data_struct.terminal_hardware[i] = SYS_HardVerMess[i];
        }
        for(i=0;i<4;i++)
        {
            p_rom_data_struct->solid_data_cell_struct.solid_message_data_struct.terminal_software[i] = SYS_SoftVerMess[i];
        }
    }
}

static void Refresh_Param_Data(Rom_Data_Type* p_rom_data_struct,u8 mode)
{
    u8 i=0;
    if(mode == READ_DATA)
    {
        strcpy((S8*)GPRS_NetPar.ServerIp, p_rom_data_struct->solid_data_cell_struct.solid_param_data_struct.server_ip);
        strcpy((S8*)GPRS_NetPar.ServerApn, p_rom_data_struct->solid_data_cell_struct.solid_param_data_struct.server_apn);
        GPRS_NetPar.ServerPort     = p_rom_data_struct->solid_data_cell_struct.solid_param_data_struct.server_port;
        for(i=0;i<ADDR_REGION_LEN;i++)
        {
            PacketHead.addrRegion[i] = p_rom_data_struct->solid_data_cell_struct.solid_param_data_struct.addr_reion[i];
        }
    }
    else
    {
        strcpy(p_rom_data_struct->solid_data_cell_struct.solid_param_data_struct.server_ip, (S8*)GPRS_NetPar.ServerIp);
        strcpy(p_rom_data_struct->solid_data_cell_struct.solid_param_data_struct.server_apn, (S8*)GPRS_NetPar.ServerApn);
        p_rom_data_struct->solid_data_cell_struct.solid_param_data_struct.server_port = GPRS_NetPar.ServerPort;
        for(i=0;i<ADDR_REGION_LEN;i++)
        {
            p_rom_data_struct->solid_data_cell_struct.solid_param_data_struct.addr_reion[i] = PacketHead.addrRegion[i];
        }
    }
}

static void Refresh_Dynamic_Data(Dynamic_Mess_Type* p_dynamic_mess_struct,u8 mode)
{
    if(mode == READ_DATA)
    {
        GPRS_CommPacket.MessRead.DeviceState.staOfEle = p_dynamic_mess_struct->s_Dynamic_Data.s_Device_Status.staOfElc;
        if(GPRS_CommPacket.MessRead.DeviceState.staOfEle == POWER_ON)   
        {
              //上电 控制IO口
              BSP_RELAY_CTROL_SET;         
        }
        else
        {
              //断电 控制IO口
              BSP_RELAY_CTROL_RESET;           
        }
    }
    else
    {
        p_dynamic_mess_struct->s_Dynamic_Data.s_Device_Status.staOfElc = GPRS_CommPacket.MessRead.DeviceState.staOfEle;    
    }

}

void Solid_Romdata_Init(void)
{
     u8 errEvent=0;
     Rom_Data_Type   solid_data_struct;
     Rom_Data_Type*  p_solid_data_struct;
     u8 software_var[4] = TERMINAL_SOFTWARE_VER;
     u8 hardware_var[4] = TERMINAL_HARDWARE_VER;
     p_solid_data_struct = &solid_data_struct;
     disableInterrupts();  //close system interrupts
     Read_Romdata(0,p_solid_data_struct);
     enableInterrupts();   //open system interrupts
     if((p_solid_data_struct->solid_data_cell_struct.writed1 == 0xaa) || (p_solid_data_struct->solid_data_cell_struct.writed2 == 0xaa))
     {

         if(Xor_Check((uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_message_data_struct)),(sizeof(Solid_Message_Data_Type)+2)) == TRUE)
         {
            Refresh_Message_Data(p_solid_data_struct,READ_DATA);
            SET_BIT(errEvent,0x02);
            #if DEBUG_CONTROLLER
            BSP_LOG("Get mess success\r\n");
            #endif
            for(u8 i=0;i<4;i++)
            {
               if(software_var[i] != SYS_SoftVerMess[i] || hardware_var[i] !=SYS_HardVerMess[i])
               {
                    SYS_SoftVerMess[i]=software_var[i];
                    SYS_HardVerMess[i]=hardware_var[i];  
                    CLEAR_BIT(errEvent,0x02);
               }
               
            }
         }
         /*长度加2是由于应用层接收到数据后检验码在倒数第二位，所以校验函数里面长度会减2*/
         if(Xor_Check((uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_param_data_struct)),(sizeof(Solid_Parameters_Data_Type)+2)) == TRUE)
		 {
		    if((errEvent & 0x02) ==0x02)
		    {
			    Refresh_Param_Data(p_solid_data_struct,READ_DATA);  //如版本号不一致，哪么不更新参数信息
			}
            SET_BIT(errEvent,0x01);
            #if DEBUG_CONTROLLER
            BSP_LOG("Get param success\r\n");
            #endif
		 }
         
     }
     if((errEvent & 0x03) ==0x03)  
     {
        return;
     }
     Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
     
}

void Dynamic_Init(void)
{
    Dynamic_Mess_Type   dynamic_mess_struct;
	Dynamic_Mess_Type*  p_dynamic_mess_struct = &dynamic_mess_struct;
    disableInterrupts();  //close system interrupts
    Read_DynamicData(1,p_dynamic_mess_struct);
    enableInterrupts();   //open system interrupts
    if((p_dynamic_mess_struct->s_Dynamic_Data.writed1 == 0xaa) || (p_dynamic_mess_struct->s_Dynamic_Data.writed2 == 0xaa))
     {

         /*长度加2是由于应用层接收到数据后检验码在倒数第二位，所以校验函数里面长度会减2*/
         if(Xor_Check((uint8_t*)(&(p_dynamic_mess_struct->s_Dynamic_Data.s_Device_Status)),(sizeof(Device_Status_Type)+2)) == TRUE)
		 {
		   
			Refresh_Dynamic_Data(p_dynamic_mess_struct,READ_DATA);
			
            #if DEBUG_CONTROLLER
            BSP_LOG("Get dynamic success\r\n");
            #endif
		 }
         
     }
}

void Store_Solid_Romdata(void)
{
    u8 reValue=0;
	Rom_Data_Type   solid_data_struct;
	Rom_Data_Type* p_solid_data_struct = &solid_data_struct;	
	
	p_solid_data_struct->solid_data_cell_struct.writed1 = 0xaa;
	p_solid_data_struct->solid_data_cell_struct.writed2 = 0xaa;
	
	Refresh_Message_Data(p_solid_data_struct,WRITE_DATA);
	p_solid_data_struct->solid_data_cell_struct.solid_message_data_xor = Get_Xor((uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_message_data_struct)), sizeof(Solid_Message_Data_Type));
	
	Refresh_Param_Data(p_solid_data_struct,WRITE_DATA);
	p_solid_data_struct->solid_data_cell_struct.solid_param_data_xor = Get_Xor((uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_param_data_struct)), sizeof(Solid_Parameters_Data_Type));
	disableInterrupts();
	Erase_Page(0);
	enableInterrupts();
	Feed_The_IWDog();
	reValue = Write_Solid_Romdata(0, p_solid_data_struct);
	if(FALSE == reValue)
	{
	     Task_Flag_Struct.writeRom_F = ENABLE;   //重写使能
	}
	else
	{
	     Task_Flag_Struct.writeRom_F = DISABLE; 
	     #if DEBUG_CONTROLLER
            BSP_LOG("write Romdata success\r\n");
         #endif
	}
}

void Store_Dynamic_Data(void)
{
    u8 reValue=0;
	Dynamic_Mess_Type   dynamic_mess_struct;
	Dynamic_Mess_Type*  p_dynamic_mess_struct = &dynamic_mess_struct;	
	
	p_dynamic_mess_struct->s_Dynamic_Data.writed1 = 0xaa;
	p_dynamic_mess_struct->s_Dynamic_Data.writed2 = 0xaa;
	
	Refresh_Dynamic_Data(p_dynamic_mess_struct,WRITE_DATA);
	p_dynamic_mess_struct->s_Dynamic_Data.ucDeviceStatusDataXor = Get_Xor((uint8_t*)(&(p_dynamic_mess_struct->s_Dynamic_Data.s_Device_Status)), sizeof(Device_Status_Type));
	
	disableInterrupts();
	Erase_Page(1);
	enableInterrupts();
	Feed_The_IWDog();
	reValue = _Store_DynamicData(1, p_dynamic_mess_struct);
	if(FALSE == reValue)
	{
	     Task_Flag_Struct.writeDynamic_F = ENABLE;   //重写使能
	}
	else
	{
	     Task_Flag_Struct.writeDynamic_F = DISABLE; 
	     #if DEBUG_CONTROLLER
            BSP_LOG("write dynamic success\r\n");
         #endif
	}

}


