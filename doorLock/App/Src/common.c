#include "main.h"
#include "common.h"
#include "led.h"
#include "motor.h"

static uint16_t timeBase = 0;

void lock_stop_detect(void)
{
	static uint8_t lastState = 0;
	uint8_t stateChange = 0;
	if(lock.lockDetectState1  && !lock.lockDetectState2){
		lock.lockState = LOCK_STATE_UNLOCK;//unlock state

	}else if(!lock.lockDetectState1 && lock.lockDetectState2){
		lock.lockState = LOCK_STATE_LOCK;//lock state
	}
	
	if(lastState != lock.lockState){
		lastState = lock.lockState;
		stateChange = 1;
	}

	if(MOTOR_TASK_FORWARD == lock.motorTask.task && lock.lockState){
		/* totally lock and stop motor */
		motor_set_stop();
		if(lock.autoLockEnable && lock.autoReportFlag){
			lock.cmdControl.autoLockAlarm.sendCmdEnable = 1;
			lock.cmdControl.autoLockAlarm.sendCmdDelay = 0;
		}
		lock.autoLockEnable = 0;
		lock.motorTask.faultDectEnable = 0;
	}else if(MOTOR_TASK_BACKWARD == lock.motorTask.task && !lock.lockState){
		/* totally unlock and stop motor */
		motor_set_stop();
		lock.autoLockEnable = 0;
		lock.motorTask.faultDectEnable = 0;
	}else if(MOTOR_TASK_IDLE == lock.motorTask.task && stateChange){
        /* manual operate alarm */
        lock.alarmStatus = LOCK_STATE_LOCK;

		if(lock.autoReportFlag){
			lock.cmdControl.singleManualAlarm.sendCmdEnable = 1;
			lock.cmdControl.singleManualAlarm.sendCmdDelay = 0;
		}

		lock.autoLockEnable = 0;
		
		/* save database */
		user_database_save();
	}
}

void gpio_interrupt_callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin){
        case doorDetect2_Pin:{
            lock.doorDetectState2 = HAL_GPIO_ReadPin(doorDetect2_GPIO_Port, doorDetect2_Pin) ? 0 : 1;
            break;
        }

        case doorDetect1_Pin:{
            lock.doorDetectState1 = HAL_GPIO_ReadPin(doorDetect1_GPIO_Port, doorDetect1_Pin) ? 0 : 1;
            break;  
        }

        case lock_Detect1_Pin:{
            lock.lockDetectState1 = HAL_GPIO_ReadPin(lock_Detect1_GPIO_Port, lock_Detect1_Pin);
            break;
        }

        case lock_Detect2_Pin:{
            lock.lockDetectState2 = HAL_GPIO_ReadPin(lock_Detect2_GPIO_Port, lock_Detect2_Pin);
            break;
        }

        case KEY_Detect_Pin:{
            lock.keyDetectState = HAL_GPIO_ReadPin(KEY_Detect_GPIO_Port, KEY_Detect_Pin);
            break;
        }

        default:
            break;
    }

    lock_stop_detect();
}

void tim_interrupt_callback(void)
{
    /* 1ms */
    timeBase ++;   
    if(timeBase >= 100){
        /* 100ms */
        timeBase = 0;

        if(lock.cmdControl.singleQueryStatus.sendCmdDelay > 0) lock.cmdControl.singleQueryStatus.sendCmdDelay --;

        if(lock.cmdControl.singleSetOnOff.sendCmdDelay > 0) lock.cmdControl.singleSetOnOff.sendCmdDelay --;

        if(lock.cmdControl.singleBasicSetting.sendCmdDelay > 0) lock.cmdControl.singleBasicSetting.sendCmdDelay --;

        if(lock.cmdControl.singleSetLight.sendCmdDelay > 0) lock.cmdControl.singleSetLight.sendCmdDelay --;

        if(lock.cmdControl.singleClrAlarm.sendCmdDelay > 0) lock.cmdControl.singleClrAlarm.sendCmdDelay --;

        if(lock.cmdControl.singleManualAlarm.sendCmdDelay > 0) lock.cmdControl.singleManualAlarm.sendCmdDelay --;

        if(lock.cmdControl.autoLockAlarm.sendCmdDelay > 0) lock.cmdControl.autoLockAlarm.sendCmdDelay --;

        if(lock.cmdControl.faultAlarm.sendCmdDelay > 0) lock.cmdControl.faultAlarm.sendCmdDelay --;

        if(lock.cmdControl.singleQueryGsensor.sendCmdDelay > 0) lock.cmdControl.singleQueryGsensor.sendCmdDelay --;

        /* auto lock detect */
        if(lock.HoldOnDetectEnable){
            lock.HoldOnLatencyCnt ++;
            if(lock.HoldOnLatencyCnt >= (lock.autoLockTime * DELAY_BASE)){
                lock.HoldOnDetectEnable = 0;
                lock.HoldOnLatencyCnt = 0;
                if(lock.lockState == LOCK_STATE_UNLOCK){
                    motor_set_forward();
                    lock.autoLockEnable = 1;
                }
            }
        }

        /* fault detect */
        if(lock.motorTask.faultDectEnable && lock.motorTask.faultDectLatency > 0){
          lock.motorTask.faultDectLatency --;
          if(lock.motorTask.faultDectLatency == 0){
            /* operate fail */
            lock.motorTask.faultDectEnable = 0;
            motor_set_stop();
            lock.autoLockEnable = 0;
            lock.cmdControl.faultAlarm.sendCmdEnable = CMD_ENABLE;
            lock.cmdControl.faultAlarm.sendCmdDelay = 0;
          }
      }
    }
    
    if(lock.motorTask.latency > 0)  lock.motorTask.latency --;

    if(lock.gSensorDelay > 0)   lock.gSensorDelay --;
}


void lock_state_init(void)
{
	lock.lockDetectState1 = HAL_GPIO_ReadPin(lock_Detect1_GPIO_Port, lock_Detect1_Pin);
	lock.lockDetectState2 = HAL_GPIO_ReadPin(lock_Detect2_GPIO_Port, lock_Detect2_Pin);
	lock.doorDetectState1 = HAL_GPIO_ReadPin(doorDetect1_GPIO_Port, doorDetect1_Pin) ? 0 : 1;
	lock.doorDetectState2 = HAL_GPIO_ReadPin(doorDetect2_GPIO_Port, doorDetect2_Pin) ? 0 : 1;
	lock.keyDetectState = HAL_GPIO_ReadPin(KEY_Detect_GPIO_Port, KEY_Detect_Pin);
}

void autolock_task(void)
{
    if(!lock.autoLockFlag || !lock.doorDetectState1 || !lock.doorDetectState2){
        lock.HoldOnDetectEnable = 0;
		lock.HoldOnLatencyCnt = 0;
        return;
    }

	if(lock.lockDetectState1  && !lock.lockDetectState2){
		if(lock.HoldOnDetectEnable == 0){
			lock.HoldOnDetectEnable = 1;
			lock.HoldOnLatencyCnt = 0;
		}
	}else if(!lock.lockDetectState1 && lock.lockDetectState2){
		lock.HoldOnDetectEnable = 0;
		lock.HoldOnLatencyCnt = 0;
	}else{
		if(lock.HoldOnDetectEnable == 0){
			lock.HoldOnDetectEnable = 1;
			lock.HoldOnLatencyCnt = 0;
		}
	}
}

