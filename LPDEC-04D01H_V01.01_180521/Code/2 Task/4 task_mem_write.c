#include "4 task_mem_write.h"

u8 MEM_Write_Task(u8 prio)
{   
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio]) 
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {
            case MEM_STORE_SOLID_ROMDATA:   
            {
		        Store_Solid_Romdata();
            }
                break;  
            case MEM_STORE_DYNAMIC_DATA:   
            {
		        Store_Dynamic_Data();
            }
                break;
            default:
                break; 
        }
        if(clear_task(prio,m_SYS_SubTask_prio) == TRUE)
        {
            return TRUE;
        }                                                
    }
    return FALSE;
}
                                           
