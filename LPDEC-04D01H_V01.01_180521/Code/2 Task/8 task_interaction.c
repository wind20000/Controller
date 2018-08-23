#include "8 task_interaction.h"

u8 Interaction_Task(u8 prio)
{
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio])
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {
            case KEY_TIPS:            
            {
                Key_Tips();
            }
            break;
            default:
                break;
        }
        if(clear_task(prio,m_SYS_SubTask_prio) == TRUE)
        {
            return TRUE;
        }                                       //每执行完一个任务就返回
    }
   return FALSE;
}



