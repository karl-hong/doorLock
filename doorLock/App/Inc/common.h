#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
// #include <math.h>
#include "main.h"
#include "flash_if.h"

#define DATABASE_START_ADDR         (0x0800F000)
#define DATABASE_MAGIC              (0xaaaa)

#define DEFAULT_LOCK_DELAY          (30)
#define DEFAULT_LOCK_REPLY_DELAY    (2)
#define DEFAULT_LOCK_LED_FLASH      (0)
#define DEFAULT_LOCK_REPORT         (1)
#define DEFAULT_AUTO_LOCK_FLAG      (0)
#define DEFAULT_SHAKE_THRESOLD      (50)
#define DEFAULT_SHAKE_INTERVAL      (3)
#define DEFAULT_X_REPORT_FLAG       (1)
#define DEFAULT_Y_REPORT_FLAG       (1)
#define DEFAULT_Z_REPORT_FLAG       (1)
#define DEFAULT_BAUD_RATE_INDEX     (4)
#define DEFAULT_DISABLE_REPORT_LATENCY (200)//50*100ms
#define DEFAULT_AUTO_CLOSE_DOOR_FLAG	(0)
#define DEFAULT_AUTO_CLOSE_DOOR_DELAY	(3000)//ms

#define DELAY_BASE                  (10)//100ms*10 = 1s
#define FLASH_FREQ                  (1)
#define FAULT_DECT                  (2*DELAY_BASE)//(5*DELAY_BASE)
#define MOTOR_LATENCY               (4)//selinda update //3
#define MOTOR_TIMEOUT               (2*DELAY_BASE)
#define MOTOR_DELAY_STOP_BASE		(MOTOR_LATENCY * 4)
#define MOTOR_DELAY_STOP_LATENCY	(1*MOTOR_DELAY_STOP_BASE)
#define LOCK_STOP_DEFAULT_DELAY     120//ms
#define UNLOCK_STOP_DEFAULT_DELAY   60//ms
#define REPORT_DOOR_STATE_DELAY		100//ms

<<<<<<< HEAD
#define	VERSION						(0x1004)//v16.4  -20250630
=======
#define	VERSION						(0x1004)//v16.4  -20250701
>>>>>>> eabcd00728ee50d796692c56617fd15ffa27cd53

#define BROADCAST_ADDR              (0xFF)
#define CHECK_ADDR_INVALID(addr)    (BROADCAST_ADDR != addr && addr != lock.address)
#define CHECK_ACK(addr)             (addr == lock.address)
#define IS_ADDR_INVALID(addr)        (addr != lock.address)
#define IS_UID_INVALID(uid0, uid1, uid2)        (uid0 != lock.uid0 || uid1 != lock.uid1 || uid2 != lock.uid2)

enum {
    CMD_DISABLE = 0,
    CMD_ENABLE,
};

enum {
    STATUS_REQUEST_UPGRADE = 1,
    STATUS_UPGRADE_GOING,
    STATUS_UPGRADE_SUCCESS,
};
#define PACKET_SIZE                 (32)

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
}gSensor_Data_t;

typedef struct {
    uint8_t sendCmdEnable;
    uint16_t sendCmdDelay;
}cmd_setting_t;

typedef struct {
    cmd_setting_t singleQueryStatus;
    cmd_setting_t singleSetOnOff;
    cmd_setting_t singleBasicSetting;
    cmd_setting_t singleSetLight;
    cmd_setting_t singleClrAlarm;
    cmd_setting_t singleManualAlarm;
    cmd_setting_t autoLockAlarm;
    cmd_setting_t faultAlarm;
    cmd_setting_t singleQueryGsensor;
    cmd_setting_t singleQueryAllStatus;
    cmd_setting_t shakeReport;
    cmd_setting_t singleModifyBaudRate;
	cmd_setting_t singleModifyShakeConfig;
	cmd_setting_t singleReportDoorState;
	cmd_setting_t singleRportAutoLockByDoorState;
    cmd_setting_t setAddrByUid;
    cmd_setting_t getInfoByAddr;
    cmd_setting_t setAddrByAddr;
    cmd_setting_t clearUartBuffer;
    cmd_setting_t factoryCmd;
    cmd_setting_t upgrade;
}cmd_control_t;

typedef struct {
    uint8_t state;
    uint8_t flashOn;
    uint16_t flashCnt;
}led_task_ctrl_t;

typedef struct {
    uint8_t state;
    uint8_t task;
    uint8_t faultType;
    uint8_t faultDectEnable;
    uint16_t faultDectLatency;
    uint16_t latency;
}motor_task_ctrl_t;

typedef struct {
    uint8_t lockDetectState1;
    uint8_t lockDetectState2;
    uint8_t doorDetectState1;
    uint8_t doorDetectState2;
    uint8_t lightState1;
    uint8_t lightState2;
    uint8_t keyDetectState;
    uint8_t lockState;
	uint8_t doorState;
	uint8_t autoCloseDoorEnable;
	uint8_t autoCloseDoorStart;
    uint8_t manulLockState;
    uint16_t lockReplyDelay;
    uint8_t  ledFlashStatus;
    uint8_t  alarmStatus;
    uint8_t  autoReportFlag;
    uint8_t  address;
    uint8_t  oldAddr;
    uint8_t autoLockFlag;
    uint8_t autoLockEnable;
    uint8_t HoldOnDetectEnable;
    uint32_t HoldOnLatencyCnt;
    uint32_t autoLockTime;
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t gSensorDelay;
    uint16_t lockStopDelay;
    uint16_t unlockStopDelay;
    uint8_t shakeThreshold;
    uint8_t xReportFlag;
    uint8_t yReportFlag;
    uint8_t zReportFlag;
    uint16_t shakeReportInterval;
    uint16_t shakeReportTimeCnt;
    uint16_t baudRateIndex;
	uint8_t disableReport;
	uint16_t disableReportLatency;
	uint16_t autoCloseDoorDelay;
	uint8_t lastDoorState;//for report
	uint8_t curDoorState;//for report
    gSensor_Data_t gSensor;
    cmd_control_t cmdControl;
    led_task_ctrl_t ledTask;
    motor_task_ctrl_t motorTask;
}lock_ctrl_t;

enum {
    LOCK_TASK_STATE_IDLE = 0,
    LOCK_TASK_STATE_UNLOCK,
    LOCK_TASK_STATE_LOCK,
};

enum {
    LED_TASK_STATE_IDLE = 0,
    LED_TASK_STATE_FLASH,
};

enum {
    LOCK_STATE_UNLOCK = 0,
    LOCK_STATE_LOCK,
};

enum {
    LED_ON = 0,
    LED_OFF,
};

enum {
    LED_RED = 0,
    LED_GREEN,
};

enum {
	DOOR_OPEN = 0,
	DOOR_CLOSE,
};

enum {
	AUTO_CLOSE_DOOR_IDLE = 0,
	AUTO_CLOSE_DOOR_START,
};

typedef struct {
    uint16_t magic;
    uint16_t address;
    uint16_t autoReportFlag;
    uint16_t lockDelayLow;
    uint16_t lockDelayHigh;
    uint16_t alarmStatus;
    uint16_t autoLockFlag;
    uint16_t lockStopDelay;
    uint16_t unlockStopDelay;
    uint16_t shakeThreshold;
    uint16_t xReportFlag;
    uint16_t yReportFlag;
    uint16_t zReportFlag;
    uint16_t shakeReportInterval;
    uint16_t baudRateIndex;
	uint16_t autoCloseDoorEnable;
	uint16_t autoCloseDoorDelay;
}database_t;

typedef struct {
    uint16_t magic;
    uint16_t address;
    uint16_t deviceCmd;
    uint16_t baudIndex;
    uint32_t upgradeFlag;
    uint32_t packetIndex;
    uint32_t packetSize;
    uint8_t packetData[PACKET_SIZE];
    uint16_t upgradeStatus;
}upgrade_t;


extern lock_ctrl_t lock;
extern uint8_t motorLatency;

void gpio_interrupt_callback(uint16_t GPIO_Pin);
void tim_interrupt_callback(void);
void lock_state_init(void);
void lock_stop_detect(void);
void autolock_task(void);
void check_door_detect1_status(void);

#endif
