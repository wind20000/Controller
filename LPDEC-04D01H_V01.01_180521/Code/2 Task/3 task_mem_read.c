#include "3 task_mem_read.h"

u8 MEM_Read_Task(u8 prio) 
{
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio]) 
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {             
            case MEM_FIRST_POR_DATA_RECOVER:            
            {
              
            }
            break;             
            case MEM_SYS_PAR_INFO_RECOVER:                              
            {
//              MEM_SYS_PAR_Info_Recover();             
            }
            break;    
            case MEM_VALVE_STATE_RECOVER:                            
            {
//              MEM_Valve_State_Recover();
            }
            break;
            case MEM_RUN_STATE_RECOVER:                                  
            {
//              MEM_Run_State_Recover();
            }
            break;
            case MEM_SYS_TEMP_AND_HUD_RECOVER:                           
            {
//              MEM_SYS_TEMP_AND_HUD_Recover();
            }
            break;
            case MEM_SYS_FAULT_STATE_RECOVER:                            
            {
//              MEM_SYS_Fault_State_Recover();
            }
            break;            
            case MEM_SYS_RTC_RECOVER:                                      
            {
//              MEM_SYS_RTC_Recover();
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