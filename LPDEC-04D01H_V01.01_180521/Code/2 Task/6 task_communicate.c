#include "6 task_communicate.h"


u8 COMM_Task(u8 prio)
{
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio])
    {   
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        { 
            case COMM_CMD_PROC:            
            {
                APP_SubTask_CmdProc();
            }
            break;
            
            case COMM_STATE_PROC:            
            {
                APP_SubTask_StateProc();
            }
            break;

            case COMM_DECODE: 
            {
                APP_SuTask_DecodeProc();
            }

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











