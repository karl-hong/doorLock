#include "main.h"
#include "common.h"
#include "led.h"
#include "motor.h"
#include "user_data.h"
#include "sc7a20.h"

static uint16_t timeBase = 0;
static uint8_t lastKeyState = 0;
static uint8_t gMotorStopEnable = 0;
static uint16_t gMotorStopLatency = 0;
static uint16_t gAutoCloseDoorCnt = 0;
static uint16_t gReportDoorStateCnt = 0;

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
		gMotorStopEnable = 1;
		gMotorStopLatency = lock.lockStopDelay;
		if(lock.autoLockEnable && lock.autoReportFlag){
			lock.cmdControl.autoLockAlarm.sendCmdEnable = 1;
			lock.cmdControl.autoLockAlarm.sendCmdDelay = 0;
		}
		lock.autoLockEnable = 0;
		lock.motorTask.faultDectEnable = 0;
		lock.cmdControl.singleSetOnOff.sendCmdDelay = 0;//立刻响应
	}else if(MOTOR_TASK_BACKWARD == lock.motorTask.task && !lock.lockState){
		/* totally unlock and stop motor */
		gMotorStopEnable = 1;
		gMotorStopLatency = lock.unlockStopDelay;
		lock.autoLockEnable = 0;
		lock.motorTask.faultDectEnable = 0;
		lock.cmdControl.singleSetOnOff.sendCmdDelay = 0;//立刻响应
	}else if(MOTOR_TASK_IDLE == lock.motorTask.task && stateChange){
        /* manual operate alarm */
        // lock.alarmStatus = LOCK_STATE_LOCK;

		// if(lock.autoReportFlag){
		// 	lock.cmdControl.singleManualAlarm.sendCmdEnable = 1;
		// 	lock.cmdControl.singleManualAlarm.sendCmdDelay = 0;
		// }

		// lock.autoLockEnable = 0;
		
		// /* save database */
		// user_database_save();
	}

    if(lastKeyState != lock.keyDetectState){
        lastKeyState = lock.keyDetectState;

        if(lock.alarmStatus != LOCK_STATE_LOCK){
           lock.alarmStatus = LOCK_STATE_LOCK; 
           /* save database */
		   user_database_save();
        }

        if(lock.autoReportFlag){
			lock.cmdControl.singleManualAlarm.sendCmdEnable = 1;
			lock.cmdControl.singleManualAlarm.sendCmdDelay = 2;
		}

        lock.autoLockEnable = 0;
    }
}

static void close_door_detect(void)
{
	uint8_t doorState;
	static uint8_t stateCnt = 0;

	if(lock.doorDetectState1 && lock.doorDetectState2){
		doorState = DOOR_CLOSE;
	}else{
		doorState = DOOR_OPEN;
	}

	if(doorState == lock.doorState){
		stateCnt = 0;
		return;
	}
	
	stateCnt ++;
	if(stateCnt < 100){
		return;
	}
	stateCnt = 0;

	lock.doorState = doorState;

	if(DOOR_OPEN == lock.doorState){
		lock.autoCloseDoorStart = AUTO_CLOSE_DOOR_IDLE;
		return;
	}

	/* door is close, check whether need to lock */
	if(!lock.autoCloseDoorEnable){
		return;//disable auto close door function
	}

	lock.autoCloseDoorStart = AUTO_CLOSE_DOOR_START;
	gAutoCloseDoorCnt = lock.autoCloseDoorDelay;
}

static void auto_close_door_task(void)
{
	if(!lock.autoCloseDoorEnable || !lock.autoCloseDoorStart){
		return;
	}
	
	if(lock.lockState == LOCK_STATE_LOCK){
		lock.autoCloseDoorStart = AUTO_CLOSE_DOOR_IDLE;
		return;
	}
	
	if(gAutoCloseDoorCnt){//ms
		gAutoCloseDoorCnt --;
	}
	
	if(0 == gAutoCloseDoorCnt){
		lock.autoCloseDoorStart = AUTO_CLOSE_DOOR_IDLE;
		if(lock.lockState == LOCK_STATE_UNLOCK){
            motor_set_forward();
            lock.autoLockEnable = 1;
			if(lock.autoReportFlag){
				lock.cmdControl.singleRportAutoLockByDoorState.sendCmdEnable = CMD_ENABLE;
				lock.cmdControl.singleRportAutoLockByDoorState.sendCmdDelay = 0;
			}
        }
	}
}

void gpio_interrupt_callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin){
        case doorDetect2_Pin:{
            lock.doorDetectState2 = HAL_GPIO_ReadPin(doorDetect2_GPIO_Port, doorDetect2_Pin) ? 0 : 1;
            break;
        }

        // case doorDetect1_Pin:{
        //     lock.doorDetectState1 = HAL_GPIO_ReadPin(doorDetect1_GPIO_Port, doorDetect1_Pin) ? 0 : 1;
        //     break;  
        // }

        case lock_Detect1_Pin:{
            lock.lockDetectState1 = HAL_GPIO_ReadPin(lock_Detect1_GPIO_Port, lock_Detect1_Pin);
            break;
        }

        case lock_Detect2_Pin:{
            lock.lockDetectState2 = HAL_GPIO_ReadPin(lock_Detect2_GPIO_Port, lock_Detect2_Pin);
            break;
        }

        case KEY_Detect_Pin:{
            lock.keyDetectState = HAL_GPIO_ReadPin(KEY_Detect_GPIO_Port, KEY_Detect_Pin) ? 0 : 1;
            break;
        }

        case G_INT_Pin:{
            sc7a20_interrupt_handle();
            return;
        }

        default:
            break;
    }

    lock_stop_detect();
}

void check_door_detect1_status(void)
{
    //static uint8_t lastState = 0;
    lock.doorDetectState1 = HAL_GPIO_ReadPin(doorDetect1_GPIO_Port, doorDetect1_Pin) ? 0 : 1;
}

void check_door_state_change(void)
{
	if(lock.cmdControl.singleReportDoorState.sendCmdEnable == CMD_DISABLE){
		lock.curDoorState = ((!lock.doorDetectState2) < 1) + (!lock.doorDetectState1);
		if(lock.curDoorState != lock.lastDoorState){
			gReportDoorStateCnt ++;
			if(gReportDoorStateCnt >= REPORT_DOOR_STATE_DELAY){
				gReportDoorStateCnt = 0;
				lock.cmdControl.singleReportDoorState.sendCmdEnable = CMD_ENABLE;
            	lock.cmdControl.singleReportDoorState.sendCmdDelay = 0;
			}
		}else{
			gReportDoorStateCnt = 0;
		}
	}
}

void tim_interrupt_callback(void)
{
    /* 1ms */
    timeBase ++;   
    if(timeBase >= 100){
        /* 100ms */
        timeBase = 0;

        user_check_report_delay();

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

		if(lock.disableReportLatency){
			lock.disableReportLatency --;
			if(0 ==lock.disableReportLatency && lock.disableReport) lock.disableReport = 0;
		}
    }
    
    if(lock.motorTask.latency > 0)  lock.motorTask.latency --;

    if(lock.gSensorDelay > 0)   lock.gSensorDelay --;
		
    if(gMotorStopEnable && (gMotorStopLatency > 0)){
        gMotorStopLatency --;
        if(!gMotorStopLatency){
            gMotorStopEnable = 0;
            motor_set_stop();
        }
    }

    if(lock.shakeReportTimeCnt > 0) lock.shakeReportTimeCnt --;
		
//		if(motorLatency > 0)	motorLatency --;

	close_door_detect();
	auto_close_door_task();
	check_door_state_change();
}


void lock_state_init(void)
{
	lock.lockDetectState1 = HAL_GPIO_ReadPin(lock_Detect1_GPIO_Port, lock_Detect1_Pin);
	lock.lockDetectState2 = HAL_GPIO_ReadPin(lock_Detect2_GPIO_Port, lock_Detect2_Pin);
	lock.doorDetectState1 = HAL_GPIO_ReadPin(doorDetect1_GPIO_Port, doorDetect1_Pin) ? 0 : 1;
	lock.doorDetectState2 = HAL_GPIO_ReadPin(doorDetect2_GPIO_Port, doorDetect2_Pin) ? 0 : 1;
	lock.keyDetectState = HAL_GPIO_ReadPin(KEY_Detect_GPIO_Port, KEY_Detect_Pin) ? 0 : 1;
	lastKeyState = lock.keyDetectState;

	if(lock.doorDetectState1 && lock.doorDetectState2){
		lock.doorState = DOOR_CLOSE;
	}else{
		lock.doorState = DOOR_OPEN;
	}

	lock.lastDoorState = ((!lock.doorDetectState2) < 1) + (!lock.doorDetectState1);
	lock.curDoorState = lock.lastDoorState;
	gReportDoorStateCnt = 0;
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

