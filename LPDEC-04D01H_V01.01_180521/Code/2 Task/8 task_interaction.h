#ifndef __TASK_INTERACTION_H
#define __TASK_INTERACTION_H

#include "global.h"

#define  BATTERY_LOW_WITHOUT_CHARGE              1
#define  BATTERY_LOW_WITH_CHARGE                 2
#define  BATTERY_MID_WITHOUT_CHARGE              3
#define  BATTERY_MID_WITH_CHARGE                 4
#define  BATTERY_HIGH                            5


u8 Interaction_Task(u8 prio);
void Battery_Status_Set(u8  data);

#endif