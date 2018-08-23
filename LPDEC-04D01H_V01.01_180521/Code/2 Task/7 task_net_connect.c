#include "7 task_net_connect.h"

u8 Net_Conn_Task(u8 prio)
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
  

