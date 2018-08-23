#ifndef __BSP_MEM_H
#define __BSP_MEM_H

#include "global.h"

#define READ_DATA               0
#define WRITE_DATA              1

/**********************************************************************************/
#define DYNAMIC_DATA_LEN                    10
#define PAGE_QUANTITY                       256     //Ã¿Ò³×Ö½ÚÊý

#define EEPROM_BASE_ADDRESS                 0x001000

/*********************************************************************************/

#define TERMINAL_SOFTWARE_VER          {0x12,0x07,0x14,0x01}
#define TERMINAL_HARDWARE_VER          {0x12,0x06,0x06,0x01}


typedef struct
{	
	u8                                 terminal_software[4];
	u8                                 terminal_hardware[4];		
}Solid_Message_Data_Type;

typedef struct
{	
    u8                                  addr_reion[7];
	char                                server_apn[6+1];
    char                                server_ip[16+1];
    u16                                 server_port;
}Solid_Parameters_Data_Type;


typedef struct
{	
	u8                                 writed1;
	u8                                 writed2;
	Solid_Message_Data_Type            solid_message_data_struct;
	u8                                 solid_message_data_xor;
	Solid_Parameters_Data_Type         solid_param_data_struct; 
	u8                                 solid_param_data_xor;
	
}Solid_Data_Cell_Type;


typedef union 
{
	u8                                 data[PAGE_QUANTITY]; 		
	Solid_Data_Cell_Type               solid_data_cell_struct;          //¹ÌÌ¬´æ´¢

}Rom_Data_Type;

typedef struct
{	
	u8                                 staOfElc;
	
}Device_Status_Type;


typedef struct
{	
	u8                                 writed1;
	u8                                 writed2;
	Device_Status_Type                 s_Device_Status;
	u8                                 ucDeviceStatusDataXor;
	
}Dynamic_Data_Type;


typedef union 
{
	u8                                 data[DYNAMIC_DATA_LEN]; 		
	Dynamic_Data_Type                  s_Dynamic_Data;          

}Dynamic_Mess_Type;


void Dynamic_Init(void);
void Solid_Romdata_Init(void);
void Store_Solid_Romdata(void);
void Store_Dynamic_Data(void);



#endif













