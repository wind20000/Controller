#include "app_control.h"

void Status_Warning_Flag(u8 lock_status,u8 arrive_status,u8 lock_warning,u8 motor_warning)
{
    Location_Package_Struct.State_Union.lock=lock_status;
    Location_Package_Struct.State_Union.arrive=arrive_status;
    
    Location_Package_Struct.Warning_Union.lock=lock_warning;
    Location_Package_Struct.Warning_Union.motor=motor_warning;
}

bool Warning_Ctl(void)
{
    Tim3_Delay_Ms(100);
    Feed_The_IWDog();
    if(GET_SIGNAL0_READ!=0)
    {
        if(Location_Package_Struct.Warning_Union.lock==NORMAL) 
        {
               Fun_Flag_Struct.alarm_flag=ENABLE;  
               Status_Warning_Flag(LOCK_OFF_STATUS,NOT_ARRIVE_STATUS,RECHARGE_WARNING,NORMAL);
               Location_Package_Struct.Lock_State_Union.upload_type=WARNING_UPLOAD;

               return TRUE;
        }
    }
    return FALSE;
}

bool Lock_Arriv_Ctl(void)                                 //配置成外部中断
{   
   u8 key=0;
   key=Arrive_Scan();
   if(key==SHORT_LOCK_ARRIV)                                                      
   { 
       Foreward();                                                                //正转
       if(Locked_Rotor_Detect()==TRUE)
       {
         
          if(Arrive_Detect()==TRUE)
          {
                PWR_MASTER_DISABLE;                                              //断电中控器
                Status_Warning_Flag(LOCK_OFF_STATUS,ARRIVE_STATUS,NORMAL,NORMAL);
    //            Location_Package_Struct.Lock_State_Union.upload_type=LOCK_OFF_UPLOAD;
                Fun_Flag_Struct.lock_alarm=ENABLE;                               //正常关锁提示音  
                Location_Package_Struct.Lock_State_Union.upload_type=LOCK_OFF_UPLOAD;
                EN_Trs_Meage_Struct.trs_location_meage=ENABLE;
                Set_Task(COMM,COMM_UPLOAD);                             //关锁上传,没有发送延时1S，会触发中断2次。如果需要去掉延时1S，那么在按键返回哪里加上锁状态判断。
                return TRUE;
          }
          else
          {
               Rollback();                                 //回转
               if(Locked_Rotor_Detect()==TRUE)
               {              
                    PWR_MASTER_DISABLE;
                    Location_Package_Struct.Lock_State_Union.upload_type=WARNING_UPLOAD;
                    Status_Warning_Flag(LOCK_ON_STATUS,NOT_ARRIVE_STATUS,LOCK_OFF_WARNING,NORMAL);
                    Fun_Flag_Struct.alarm_flag=ENABLE;
           //         Location_Package_Struct.Lock_State_Union.upload_type=WARNING_UPLOAD;
                    EN_Trs_Meage_Struct.trs_location_meage=ENABLE;
                    Set_Task(COMM,COMM_UPLOAD);
                    return TRUE;
               }
        
          }
       }
       PWR_MASTER_DISABLE;
       Fun_Flag_Struct.alarm_flag=ENABLE;
       Status_Warning_Flag(LOCK_ON_STATUS,NOT_ARRIVE_STATUS,LOCK_OFF_WARNING,MOTOR_WARNING);
       Location_Package_Struct.Lock_State_Union.upload_type=WARNING_UPLOAD;
       EN_Trs_Meage_Struct.trs_location_meage=ENABLE;
       Set_Task(COMM,COMM_UPLOAD);
   }
   return FALSE;
}

bool Open_Lock_Ctl(void)
{
   Rollback();
   if(Locked_Rotor_Detect()==TRUE)
   {
          if(Arrive_Detect()==FALSE)
          {
               PWR_MASTER_ENABLE;                                                       //上电中控器
               Status_Warning_Flag(LOCK_ON_STATUS,NOT_ARRIVE_STATUS,NORMAL,NORMAL);
               Location_Package_Struct.Lock_State_Union.upload_type=LOCK_ON_UPLOAD;
               Fun_Flag_Struct.lock_alarm=ENABLE;                                      //正常开锁提示音   
         //      Location_Package_Struct.Lock_State_Union.upload_type=LOCK_ON_UPLOAD;
               return TRUE;
          }
          else
          {
               PWR_MASTER_DISABLE; 
               Fun_Flag_Struct.alarm_flag=ENABLE;
               Location_Package_Struct.Lock_State_Union.upload_type=WARNING_UPLOAD;
               Status_Warning_Flag(LOCK_OFF_STATUS,ARRIVE_STATUS,LOCK_ON_WARNING,NORMAL);
          //     Location_Package_Struct.Lock_State_Union.upload_type=WARNING_UPLOAD;
               return FALSE;
          }
   }
   PWR_MASTER_DISABLE; 
   Fun_Flag_Struct.alarm_flag=ENABLE;
   Status_Warning_Flag(LOCK_OFF_STATUS,ARRIVE_STATUS,LOCK_ON_WARNING,MOTOR_WARNING);
   Location_Package_Struct.Lock_State_Union.upload_type=WARNING_UPLOAD;
   return FALSE;
}
















