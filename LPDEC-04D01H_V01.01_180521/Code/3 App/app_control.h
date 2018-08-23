#ifndef __APP_CONTROL_H
#define __APP_CONTROL_H

#include "global.h"

void Status_Warning_Flag(u8 lock_status,u8 arrive_status,u8 lock_warning,u8 motor_warning);
bool Warning_Ctl(void);
bool Lock_Arriv_Ctl(void);
bool Open_Lock_Ctl(void);
#endif
















