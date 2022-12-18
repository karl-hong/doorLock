#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdint.h>

#define DATABASE_START_ADDR         (0x0800F000)
#define DATABASE_MAGIC              (0xaaaa)

#define DEFAULT_LOCK_DELAY          (30)
#define DEFAULT_LOCK_REPLY_DELAY    (2)
#define DEFAULT_LOCK_LED_FLASH      (0)
#define DEFAULT_LOCK_REPORT         (1)

#define DELAY_BASE                  (10)//100ms*10 = 1s
#define FLASH_FREQ                  (1)
#define FAULT_DECT                  (5*DELAY_BASE)
#define MOTOR_LATENCY               (3)
#define MOTOR_TIMEOUT               (2*DELAY_BASE)

enum {
    CMD_DISABLE = 0,
    CMD_ENABLE,
};

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
    uint8_t manulLockState;
    uint16_t lockReplyDelay;
    uint8_t  ledFlashStatus;
    uint8_t  alarmStatus;
    uint8_t  autoReportFlag;
    uint8_t  address;
    uint8_t autoLockFlag;
    uint8_t autoLockEnable;
    uint8_t HoldOnDetectEnable;
    uint32_t HoldOnLatencyCnt;
    uint32_t autoLockTime;
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t gSensorDelay;
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

typedef struct {
    uint16_t magic;
    uint16_t address;
    uint16_t autoReportFlag;
    uint16_t lockDelayLow;
    uint16_t lockDelayHigh;
    uint16_t alarmStatus;
}database_t;


extern lock_ctrl_t lock;
extern uint8_t motorLatency;

void gpio_interrupt_callback(uint16_t GPIO_Pin);
void tim_interrupt_callback(void);
void lock_state_init(void);
void lock_stop_detect(void);
void autolock_task(void);

#endif
