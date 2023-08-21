#include <stdio.h>
#include "user_data.h"
#include "common.h"
#include "user_protocol.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash_ex.h"  
#include "led.h"

void onCmdQuerySingleDevStatus(uint8_t *data, uint16_t length)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;

    if(length < 12){
        printf("[%s]length error!\r\n", __FUNCTION__);
        return;
    }

    uid0 = (data[pos++] << 24);
    uid0 += (data[pos++] << 16);
    uid0 += (data[pos++] << 8);
    uid0 += data[pos++];

    uid1 = (data[pos++] << 24);
    uid1 += (data[pos++] << 16);
    uid1 += (data[pos++] << 8);
    uid1 += data[pos++];

    uid2 = (data[pos++] << 24);
    uid2 += (data[pos++] << 16);
    uid2 += (data[pos++] << 8);
    uid2 += data[pos++];

    if(lock.uid0 != uid0 || lock.uid1 != uid1 || lock.uid2 != uid2){
        printf("[%s]UID is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* send dev status here */
    lock.cmdControl.singleQueryStatus.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.singleQueryStatus.sendCmdDelay = 0;
}

void onCmdSetDeviceOnOff(uint8_t *data, uint16_t length, uint8_t ack)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;
    uint8_t lockSetState;
    uint16_t cmdLength;

    if(NULL == data){
        printf("[%s]data is null!\r\n", __FUNCTION__);
        return;
    }

    if(ack) cmdLength = 13;
    else    cmdLength = 1;

    if(cmdLength > length){
        printf("[%s]length error!\r\n", __FUNCTION__);
        return;
    }

    lockSetState = data[pos++];

    if(!ack){
        goto out;
    }

    uid0 = (data[pos++] << 24);
    uid0 += (data[pos++] << 16);
    uid0 += (data[pos++] << 8);
    uid0 += data[pos++];

    uid1 = (data[pos++] << 24);
    uid1 += (data[pos++] << 16);
    uid1 += (data[pos++] << 8);
    uid1 += data[pos++];

    uid2 = (data[pos++] << 24);
    uid2 += (data[pos++] << 16);
    uid2 += (data[pos++] << 8);
    uid2 += data[pos++];

    if(lock.uid0 != uid0 || lock.uid1 != uid1 || lock.uid2 != uid2){
        printf("[%s]UID is not matched!\r\n", __FUNCTION__);
        return;
    }
out:
    /* set dev state here */
    if(lockSetState == 0 && lock.lockState){//unlock
        motor_set_backward();
    }else if(lockSetState && !lock.lockState){//lock
        if(lock.doorDetectState1 && lock.doorDetectState2)    motor_set_forward();
    }
    /* send ack msg here */
    if(ack){
        lock.cmdControl.singleSetOnOff.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleSetOnOff.sendCmdDelay = lock.lockReplyDelay * DELAY_BASE;
    }
}

void onCmdModifyDeviceBasicSetting(uint8_t *data, uint16_t length, uint8_t ack)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;
    uint32_t autoLockDelay;
    uint8_t autoLockFlag;
    uint8_t  autoReportFlag;
    uint8_t  addr;
    uint16_t cmdLength;
    uint16_t lockStopDelay;
    uint16_t unlockStopDelay;
    uint8_t shakeThresold;
    uint8_t xReportFlag;
    uint8_t yReportFlag;
    uint8_t zReportFlag;
    uint16_t shakeReportInterval;

    if(NULL == data){
        printf("[%s]data is null!\r\n", __FUNCTION__);
        return;
    }

//    if(ack) cmdLength = 18;
//    else    cmdLength = 5;

//    if(cmdLength > length){
//        printf("[%s]length error!\r\n", __FUNCTION__);
//        return;
//    }

    if(ack) addr = data[pos++];

    autoLockDelay = data[pos++] << 16;
    autoLockDelay += data[pos++] << 8;
    autoLockDelay += data[pos++];

    autoLockFlag = data[pos++];
    
    autoReportFlag = data[pos++];

    if(!ack){
        goto get_shake;
    }

    unlockStopDelay = data[pos++] << 8;
    unlockStopDelay += data[pos++];

    lockStopDelay = data[pos++] << 8;
    lockStopDelay += data[pos++];

get_shake:
    shakeThresold = data[pos++];
    xReportFlag = data[pos++];
    yReportFlag = data[pos++];
    zReportFlag = data[pos++];
    shakeReportInterval = data[pos++] << 8;
    shakeReportInterval += data[pos++];


    if(!ack){
        goto out;
    }

    uid0 = (data[pos++] << 24);
    uid0 += (data[pos++] << 16);
    uid0 += (data[pos++] << 8);
    uid0 += data[pos++];

    uid1 = (data[pos++] << 24);
    uid1 += (data[pos++] << 16);
    uid1 += (data[pos++] << 8);
    uid1 += data[pos++];

    uid2 = (data[pos++] << 24);
    uid2 += (data[pos++] << 16);
    uid2 += (data[pos++] << 8);
    uid2 += data[pos++];

    printf("uid: 0x%x%x%x\r\n", uid0, uid1, uid2);

    if(lock.uid0 != uid0 || lock.uid1 != uid1 || lock.uid2 != uid2){
        printf("[%s]UID is not matched!\r\n", __FUNCTION__);
        return;
    }
out:
    /* set dev state here */
    if(ack){
        lock.address = addr;
        lock.unlockStopDelay = unlockStopDelay;
        lock.lockStopDelay = lockStopDelay;
    }
    lock.autoLockTime = autoLockDelay;
    lock.autoLockFlag = autoLockFlag;
    // lock.lockReplyDelay = lockReplyDelay;
    lock.autoReportFlag = autoReportFlag;
    lock.shakeThreshold = shakeThresold;
    lock.xReportFlag = xReportFlag;
    lock.yReportFlag = yReportFlag;
    lock.zReportFlag = zReportFlag;
    lock.shakeReportInterval = shakeReportInterval;
    
    user_database_save();
    /* send ack msg here */
    if(ack){
        lock.cmdControl.singleBasicSetting.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleBasicSetting.sendCmdDelay = 0;
    }
}

void  onCmdSetLight(uint8_t *data, uint16_t length, uint8_t ack)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;
    uint8_t  lightState1;
    uint8_t  lightState2;
    uint16_t cmdLength;

    if(NULL == data){
        printf("[%s]data is null!\r\n", __FUNCTION__);
        return;
    }

    if(ack) cmdLength = 14;
    else    cmdLength = 1;

    if(cmdLength > length){
        printf("[%s]length error!\r\n", __FUNCTION__);
        return;
    }   

    lightState1 = data[pos++];

    if(!ack){
        goto out;
    }

    lightState2 = data[pos++];

    uid0 = (data[pos++] << 24);
    uid0 += (data[pos++] << 16);
    uid0 += (data[pos++] << 8);
    uid0 += data[pos++];

    uid1 = (data[pos++] << 24);
    uid1 += (data[pos++] << 16);
    uid1 += (data[pos++] << 8);
    uid1 += data[pos++];

    uid2 = (data[pos++] << 24);
    uid2 += (data[pos++] << 16);
    uid2 += (data[pos++] << 8);
    uid2 += data[pos++];

    if(lock.uid0 != uid0 || lock.uid1 != uid1 || lock.uid2 != uid2){
        printf("[%s]UID is not matched!\r\n", __FUNCTION__);
        return;
    }
out:

    /* send ack msg here */
    if(ack){
        if(lightState1) set_light1_on();
        else            set_light1_off();
        if(lightState2) set_light2_on();
        else            set_light2_off();
        lock.cmdControl.singleSetLight.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleSetLight.sendCmdDelay = 0;
    }else{
        if(lightState1){
            set_light1_on();
            set_light2_on();
        }else{
            set_light1_off();
            set_light2_off();
        }         
    }
}

void onCmdClrDevAlarmSetting(uint8_t *data, uint16_t length, uint8_t ack)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;
    uint16_t cmdLength; 

    if(!ack){
        goto out;
    } 

    cmdLength = 12;
    if(cmdLength > length){
       printf("[%s]length error!\r\n", __FUNCTION__);
        return;
    }

    uid0 = (data[pos++] << 24);
    uid0 += (data[pos++] << 16);
    uid0 += (data[pos++] << 8);
    uid0 += data[pos++];

    uid1 = (data[pos++] << 24);
    uid1 += (data[pos++] << 16);
    uid1 += (data[pos++] << 8);
    uid1 += data[pos++];

    uid2 = (data[pos++] << 24);
    uid2 += (data[pos++] << 16);
    uid2 += (data[pos++] << 8);
    uid2 += data[pos++];

    if(lock.uid0 != uid0 || lock.uid1 != uid1 || lock.uid2 != uid2){
        printf("[%s]UID is not matched!\r\n", __FUNCTION__);
        return;
    }
out:
    /* send ack msg here */
    if(ack){
        /* clear device alarm setting here */
        lock.alarmStatus = 0;
        lock.cmdControl.singleClrAlarm.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleClrAlarm.sendCmdDelay = 0;
    }else{
        lock.alarmStatus = 0;
    }

    user_database_save();
}

void onCmdGetGsensorData(uint8_t *data, uint16_t length)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;

    if(length < 12){
        printf("[%s]length error!\r\n", __FUNCTION__);
        return;
    }

    uid0 = (data[pos++] << 24);
    uid0 += (data[pos++] << 16);
    uid0 += (data[pos++] << 8);
    uid0 += data[pos++];

    uid1 = (data[pos++] << 24);
    uid1 += (data[pos++] << 16);
    uid1 += (data[pos++] << 8);
    uid1 += data[pos++];

    uid2 = (data[pos++] << 24);
    uid2 += (data[pos++] << 16);
    uid2 += (data[pos++] << 8);
    uid2 += data[pos++];

    if(lock.uid0 != uid0 || lock.uid1 != uid1 || lock.uid2 != uid2){
        printf("[%s]UID is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* send dev status here */
    lock.cmdControl.singleQueryGsensor.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.singleQueryGsensor.sendCmdDelay = 0;    
}

void onCmdQuerySingleDevAllStatus(uint8_t *data, uint16_t length)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;

    if(length < 12){
        printf("[%s]length error!\r\n", __FUNCTION__);
        return;
    }

    uid0 = (data[pos++] << 24);
    uid0 += (data[pos++] << 16);
    uid0 += (data[pos++] << 8);
    uid0 += data[pos++];

    uid1 = (data[pos++] << 24);
    uid1 += (data[pos++] << 16);
    uid1 += (data[pos++] << 8);
    uid1 += data[pos++];

    uid2 = (data[pos++] << 24);
    uid2 += (data[pos++] << 16);
    uid2 += (data[pos++] << 8);
    uid2 += data[pos++];

    if(lock.uid0 != uid0 || lock.uid1 != uid1 || lock.uid2 != uid2){
        printf("[%s]UID is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* send dev status here */
    lock.cmdControl.singleQueryAllStatus.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.singleQueryAllStatus.sendCmdDelay = 0;
}

void onReportDeviceStatus(void)
{
    uint8_t buffer[50];
    uint8_t pos = 0;
    /* addr */
    buffer[pos++] = lock.address;
    /* lock state */
    buffer[pos++] = lock.lockState;
    /* door detect */
    buffer[pos++] = lock.doorDetectState1;
    buffer[pos++] = lock.doorDetectState2;
    /* key detect */
    buffer[pos++] = lock.keyDetectState;
    /* auto lock time */
    buffer[pos++] = (lock.autoLockTime >> 16) & 0xff;
    buffer[pos++] = (lock.autoLockTime >> 8) & 0xff;
    buffer[pos++] = lock.autoLockTime & 0xff;
    /* auto lock flag */
    buffer[pos++] = lock.autoLockFlag;
    /* light status */
    buffer[pos++] = lock.lightState1;
    buffer[pos++] = lock.lightState2;
    /* alarm status */
    buffer[pos++] = lock.alarmStatus;
    /* auto report flag */
    buffer[pos++] = lock.autoReportFlag;
    /* shake thresold */
    buffer[pos++] = lock.shakeThreshold;
    /* x report flag */
    buffer[pos++] = lock.xReportFlag;
    /* y report flag */
    buffer[pos++] = lock.yReportFlag;
    /* z report flag */
    buffer[pos++] = lock.zReportFlag;
    /* shake report interval */
    buffer[pos++] = (lock.shakeReportInterval >> 8) & 0xff;
    buffer[pos++] = lock.shakeReportInterval & 0xff;

    /* UID */
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;
    
    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_QUERY_STATUS, buffer, pos);       
}

void onReportSetDevOnOffStatus(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    /* addr */
    buffer[pos++] = lock.address;
    /* lock status */
    buffer[pos++] = lock.lockState;
    /* UID */
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_SET_ONOFF, buffer, pos);     
}

void onReportBasicSetting(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    /* addr */
    buffer[pos++] = lock.address;
    /* auto lock time */
    buffer[pos++] = (lock.autoLockTime >> 16) & 0xff;
    buffer[pos++] = (lock.autoLockTime >> 8) & 0xff;
    buffer[pos++] = lock.autoLockTime & 0xff;
    /* auto lock flag */
    buffer[pos++] = lock.autoLockFlag;
    /* auto report flag */
    buffer[pos++] = lock.autoReportFlag;
    /* unlock stop delay */
    buffer[pos++] = (lock.unlockStopDelay >> 8) & 0xff;
    buffer[pos++] = lock.unlockStopDelay & 0xff;
    /* lock stop delay */
    buffer[pos++] = (lock.lockStopDelay >> 8) & 0xff;
    buffer[pos++] = lock.lockStopDelay & 0xff;
    /* UID */
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_BASE_SETTING, buffer, pos);     
}

void onReportSetLightStatus(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    /* addr */
    buffer[pos++] = lock.address;
    /* light1 state */
    buffer[pos++] = lock.lightState1;
    /* light2 state */
    buffer[pos++] = lock.lightState2;
    /* UID */
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_SET_LIGHT, buffer, pos); 
}

void onReportClearDevAlarmSetting(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    /* addr */
    buffer[pos++] = lock.address;
    /* alarm status */
    buffer[pos++] = lock.alarmStatus;
    /* UID */
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_CLEAR_ALARM, buffer, pos);     
}

void onReportManualAlarm(uint8_t alarmType)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    buffer[pos++] = lock.address;
    buffer[pos++] = alarmType;
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_QUERY, OPT_CODE_MANUAL_ALARM, buffer, pos);     
}

void onReportAutoLockAlarm(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    buffer[pos++] = lock.address;
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_QUERY, OPT_CODE_AUTO_LOCK, buffer, pos);     
}

void onReportFaultAlarm(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    buffer[pos++] = lock.address;
    buffer[pos++] = lock.motorTask.faultType;
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_QUERY, OPT_CODE_FAULT_ALARM, buffer, pos);   
}

void onReportGsensorData(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    /* addr */
    buffer[pos++] = lock.address;
    /* gsensor data */
    buffer[pos++] = (lock.gSensor.x >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.x & 0xff;
    buffer[pos++] = (lock.gSensor.y >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.y & 0xff;
    buffer[pos++] = (lock.gSensor.z >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.z & 0xff;
    /* UID */
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_QUERY_GSENSOR, buffer, pos);     
}

void onReportDeviceAllStatus(void)
{
    uint8_t buffer[50];
    uint8_t pos = 0;
    /* addr */
    buffer[pos++] = lock.address;
    /* lock state */
    buffer[pos++] = lock.lockState;
    /* door detect */
    buffer[pos++] = lock.doorDetectState1;
    buffer[pos++] = lock.doorDetectState2;
    /* key detect */
    buffer[pos++] = lock.keyDetectState;
    /* auto lock time */
    buffer[pos++] = (lock.autoLockTime >> 16) & 0xff;
    buffer[pos++] = (lock.autoLockTime >> 8) & 0xff;
    buffer[pos++] = lock.autoLockTime & 0xff;
    /* auto lock flag */
    buffer[pos++] = lock.autoLockFlag;
    /* light status */
    buffer[pos++] = lock.lightState1;
    buffer[pos++] = lock.lightState2;
    /* alarm status */
    buffer[pos++] = lock.alarmStatus;
    /* auto report flag */
    buffer[pos++] = lock.autoReportFlag;
    /* gsensor data */
    buffer[pos++] = (lock.gSensor.x >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.x & 0xff;
    buffer[pos++] = (lock.gSensor.y >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.y & 0xff;
    buffer[pos++] = (lock.gSensor.z >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.z & 0xff;
    /* shake thresold */
    buffer[pos++] = lock.shakeThreshold;
    /* x report flag */
    buffer[pos++] = lock.xReportFlag;
    /* y report flag */
    buffer[pos++] = lock.yReportFlag;
    /* z report flag */
    buffer[pos++] = lock.zReportFlag;
    /* shake report interval */
    buffer[pos++] = (lock.shakeReportInterval >> 8) & 0xff;
    buffer[pos++] = lock.shakeReportInterval & 0xff;
    /* unlock stop delay */
    buffer[pos++] = (lock.unlockStopDelay >> 8) & 0xff;
    buffer[pos++] = lock.unlockStopDelay & 0xff;
    /* lock stop delay */
    buffer[pos++] = (lock.lockStopDelay >> 8) & 0xff;
    buffer[pos++] = lock.lockStopDelay & 0xff;
    /* UID */
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;
    
    user_protocol_send_data(CMD_QUERY, OPT_CODE_SINGLE_DEV_QUERY_ALL_STATUS, buffer, pos);       
}


void onReportShakeAlarm(void)
{
    uint8_t buffer[50];
    uint8_t pos = 0;

    /* addr */
    buffer[pos++] = lock.address;
    /* UID */
    buffer[pos++] = (lock.uid0 >> 24)& 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24)& 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24)& 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;

    user_protocol_send_data(CMD_ACK, OPT_CODE_REPORT_SHAKE_ALARM, buffer, pos);       
}

uint16_t user_read_flash(uint32_t address)
{
    return *(__IO uint16_t*)address;
}

void user_database_init(void)
{
    uint16_t i;
    uint16_t lenOfDataBase = sizeof(database_t) / sizeof(uint16_t);
    database_t readDataBase;
    uint16_t *pData = (uint16_t *)&readDataBase;

    memset((uint8_t *)&lock, 0, sizeof(lock));

    printf("\r\n******************** App Start ********************\r\n");

    lock.uid0 = HAL_GetUIDw0();
    lock.uid1 = HAL_GetUIDw1();
    lock.uid2 = HAL_GetUIDw2();

    for(i=0;i<lenOfDataBase;i++){
        pData[i] = user_read_flash(DATABASE_START_ADDR + 2U*i);
    }

    if(DATABASE_MAGIC != readDataBase.magic){
        printf("Init Database!!!\r\n");
        lock.autoLockTime = DEFAULT_LOCK_DELAY;
        lock.lockReplyDelay = DEFAULT_LOCK_REPLY_DELAY;
        lock.autoReportFlag = DEFAULT_LOCK_REPORT;
        lock.ledFlashStatus = DEFAULT_LOCK_LED_FLASH;
        lock.autoLockFlag = DEFAULT_AUTO_LOCK_FLAG;
        lock.lockStopDelay = LOCK_STOP_DEFAULT_DELAY;
        lock.unlockStopDelay = UNLOCK_STOP_DEFAULT_DELAY;
        lock.alarmStatus = lock.keyDetectState;
        lock.shakeThreshold = DEFAULT_SHAKE_THRESOLD;
        lock.xReportFlag = DEFAULT_X_REPORT_FLAG;
        lock.yReportFlag = DEFAULT_Y_REPORT_FLAG;
        lock.zReportFlag = DEFAULT_Z_REPORT_FLAG;
        lock.shakeReportInterval = DEFAULT_SHAKE_INTERVAL;

  
        user_database_save();
    }else{
        printf("Read database from flash!!!\r\n");
        lock.address = (uint8_t )readDataBase.address;
        lock.autoLockTime = readDataBase.lockDelayLow;
        lock.autoLockTime += (readDataBase.lockDelayHigh << 16);
        lock.lockReplyDelay = DEFAULT_LOCK_REPLY_DELAY;
        lock.autoReportFlag = (uint8_t)readDataBase.autoReportFlag;
        lock.alarmStatus = readDataBase.alarmStatus;
		lock.autoLockFlag = readDataBase.autoLockFlag ? 1 : 0;
        lock.lockStopDelay = readDataBase.lockStopDelay;
        lock.unlockStopDelay = readDataBase.unlockStopDelay;
        if(lock.lockStopDelay  == 0xffff)   lock.lockStopDelay = LOCK_STOP_DEFAULT_DELAY;
        if(lock.unlockStopDelay == 0xffff)  lock.unlockStopDelay = UNLOCK_STOP_DEFAULT_DELAY;

        lock.shakeThreshold = (readDataBase.shakeThreshold == 0xffff) ? DEFAULT_SHAKE_THRESOLD : readDataBase.shakeThreshold;
        lock.xReportFlag = (readDataBase.xReportFlag == 0xffff) ? DEFAULT_X_REPORT_FLAG : readDataBase.xReportFlag;
        lock.yReportFlag = (readDataBase.yReportFlag == 0xffff) ? DEFAULT_Y_REPORT_FLAG : readDataBase.yReportFlag;
        lock.zReportFlag = (readDataBase.zReportFlag == 0xffff) ? DEFAULT_Z_REPORT_FLAG : readDataBase.zReportFlag;
        lock.shakeReportInterval = (readDataBase.shakeReportInterval == 0xffff) ? DEFAULT_SHAKE_INTERVAL : readDataBase.shakeReportInterval;
        //lock.ledFlashStatus = (uint8_t)readDataBase.ledFlash;
    }

    printf("Chip uuid: 0x%04x%04x%04x\r\n", lock.uid0, lock.uid1, lock.uid2);
    printf("address: 0x%02X\r\n", lock.address);
    printf("autoReportFlag: 0x%02X\r\n", lock.autoReportFlag);
    printf("lockDelayEnable: 0x%02X\r\n", lock.autoLockFlag);
    printf("lockDelay: 0x%04X\r\n", lock.autoLockTime);
    printf("lockReplyDelay: 0x%02X\r\n", lock.lockReplyDelay);
}

void user_database_save(void)
{
    uint16_t i;
    uint16_t lenOfDataBase = sizeof(database_t) / sizeof(uint16_t);
    database_t writeDataBase;
    uint16_t *pData = (uint16_t *)&writeDataBase;
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef flashEraseInitType;
    uint32_t PageError;

    writeDataBase.magic = DATABASE_MAGIC;
    writeDataBase.address = lock.address;
    writeDataBase.autoReportFlag = lock.autoReportFlag;
    //writeDataBase.ledFlash = lock.ledFlashStatus;
    writeDataBase.lockDelayLow = lock.autoLockTime & 0xffff;
    writeDataBase.lockDelayHigh = (lock.autoLockTime >> 16) & 0xffff;
    writeDataBase.alarmStatus = lock.alarmStatus;
    writeDataBase.autoLockFlag = lock.autoLockFlag;
    writeDataBase.lockStopDelay = lock.lockStopDelay;
    writeDataBase.unlockStopDelay = lock.unlockStopDelay;

    writeDataBase.shakeThreshold = lock.shakeThreshold;
    writeDataBase.xReportFlag = lock.xReportFlag;
    writeDataBase.yReportFlag = lock.yReportFlag;
    writeDataBase.zReportFlag = lock.zReportFlag;
    writeDataBase.shakeReportInterval = lock.shakeReportInterval;

    HAL_FLASH_Unlock();

    flashEraseInitType.TypeErase = FLASH_TYPEERASE_PAGES;
    flashEraseInitType.PageAddress = DATABASE_START_ADDR;
    flashEraseInitType.NbPages = 1;
    status = HAL_FLASHEx_Erase(&flashEraseInitType, &PageError);
    
    if(HAL_OK != status){
        HAL_FLASH_Lock();
        printf("Flash erase error: %d\r\n", status);
        return;
    }

    for(i=0;i<lenOfDataBase;i++){
       if(HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, DATABASE_START_ADDR + 2U*i, pData[i])){
            printf("[%s]write data[%d] fail!\r\n", __FUNCTION__, i);
       } 
    }

    HAL_FLASH_Lock();
}

void user_reply_handle(void)
{
    if(lock.cmdControl.singleQueryStatus.sendCmdEnable && !lock.cmdControl.singleQueryStatus.sendCmdDelay){
        lock.cmdControl.singleQueryStatus.sendCmdEnable = CMD_DISABLE;
        onReportDeviceStatus();
    }

    if(lock.cmdControl.singleSetOnOff.sendCmdEnable && !lock.cmdControl.singleSetOnOff.sendCmdDelay){
        lock.cmdControl.singleSetOnOff.sendCmdEnable = CMD_DISABLE;
        onReportSetDevOnOffStatus();
    }

    if(lock.cmdControl.singleBasicSetting.sendCmdEnable && !lock.cmdControl.singleBasicSetting.sendCmdDelay){
        lock.cmdControl.singleBasicSetting.sendCmdEnable = CMD_DISABLE;
        onReportBasicSetting();
    }

    if(lock.cmdControl.singleSetLight.sendCmdEnable && !lock.cmdControl.singleSetLight.sendCmdDelay){
        lock.cmdControl.singleSetLight.sendCmdEnable = CMD_DISABLE;
        onReportSetLightStatus();
    }

    if(lock.cmdControl.singleClrAlarm.sendCmdEnable && !lock.cmdControl.singleClrAlarm.sendCmdDelay){
        lock.cmdControl.singleClrAlarm.sendCmdEnable = CMD_DISABLE;
        onReportClearDevAlarmSetting();
    }

    if(lock.cmdControl.singleManualAlarm.sendCmdEnable && !lock.cmdControl.singleManualAlarm.sendCmdDelay){
        lock.cmdControl.singleManualAlarm.sendCmdEnable = CMD_DISABLE;
        //onReportManualAlarm(lock.lockState);
        printf("keyDetectState: %d\r\n", lock.keyDetectState);
        onReportManualAlarm(lock.keyDetectState);
    }
		
    if(lock.cmdControl.autoLockAlarm.sendCmdEnable && !lock.cmdControl.autoLockAlarm.sendCmdDelay){
        lock.cmdControl.autoLockAlarm.sendCmdEnable = CMD_DISABLE;
        onReportAutoLockAlarm();   
    }
    
    if(lock.cmdControl.faultAlarm.sendCmdEnable && !lock.cmdControl.faultAlarm.sendCmdDelay){
        lock.cmdControl.faultAlarm.sendCmdEnable = CMD_DISABLE;
        onReportFaultAlarm();  
    }
    
    if(lock.cmdControl.singleQueryGsensor.sendCmdEnable && !lock.cmdControl.singleQueryGsensor.sendCmdDelay){
        lock.cmdControl.singleQueryGsensor.sendCmdEnable = CMD_DISABLE;
        onReportGsensorData();
    }

    if(lock.cmdControl.singleQueryAllStatus.sendCmdEnable && !lock.cmdControl.singleQueryAllStatus.sendCmdDelay){
        lock.cmdControl.singleQueryAllStatus.sendCmdEnable = CMD_DISABLE;
        onReportDeviceAllStatus();
    }

    if(lock.cmdControl.shakeReport.sendCmdEnable && !lock.cmdControl.shakeReport.sendCmdDelay){
        lock.cmdControl.shakeReport.sendCmdEnable = CMD_DISABLE;
        onReportShakeAlarm();
    }

}

