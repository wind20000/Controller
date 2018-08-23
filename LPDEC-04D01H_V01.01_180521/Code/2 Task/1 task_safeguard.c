#include "1 task_safeguard.h"

u16 voltage;
u8 SG_Task(u8 prio)
{
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio])
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {
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