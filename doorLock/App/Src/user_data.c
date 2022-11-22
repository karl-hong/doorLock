#include <stdio.h>
#include "user_data.h"
#include "common.h"
#include "user_protocol.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash_ex.h"  

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
    if(lockSetState == 0 && lock.lockDetectState){
        lock.lockTaskState = LOCK_TASK_STATE_UNLOCK;//unlock
        /* set led state here */
        // lock.ledTask.state = LED_TASK_STATE_FLASH;
    }else if(lockSetState && !lock.lockDetectState){
        lock.lockTaskState = LOCK_TASK_STATE_LOCK;//lock
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
    // uint16_t lockReplyDelay;
    uint8_t  autoReportFlag;
    uint8_t  addr;
    uint16_t cmdLength;

    if(NULL == data){
        printf("[%s]data is null!\r\n", __FUNCTION__);
        return;
    }

    if(ack) cmdLength = 18;
    else    cmdLength = 17;

    if(cmdLength > length){
        printf("[%s]length error!\r\n", __FUNCTION__);
        return;
    }

    /**/
    if(ack) addr = data[pos++];

    autoLockDelay = data[pos++] << 16;
    autoLockDelay += data[pos++] << 8;
    autoLockDelay += data[pos++];

    autoLockFlag = data[pos++];
    
    // lockReplyDelay = data[pos++] << 8;
    // lockReplyDelay += data[pos++];
    
    autoReportFlag = data[pos++];

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
    if(ack) lock.address = addr;
    lock.autoLockTime = autoLockDelay;
    lock.autoLockFlag = autoLockFlag;
    // lock.lockReplyDelay = lockReplyDelay;
    lock.autoReportFlag = autoReportFlag;
    user_database_save();
    /* send ack msg here */
    if(ack){
        lock.cmdControl.singleBasicSetting.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleBasicSetting.sendCmdDelay = 0;
    }
}

void onCmdSetLight(uint8_t *data, uint16_t length, uint8_t ack)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;
    uint8_t  lightState1;
    uint8_t  lightState2
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
        lock.lightState1 = lightState1;
        lock.lightState2 = lightState2;
        lock.cmdControl.singleSetLight.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleSetLight.sendCmdDelay = 0;
    }else{
        lock.lightState1 = lightState1;
        lock.lightState2 = lightState1;
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

void onReportDeviceStatus(void)
{
    uint8_t buffer[40];
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


void onReportDevAlarm(uint8_t alarmType)
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

    user_protocol_send_data(CMD_QUERY, OPTION_MANUAL_ALARM, buffer, pos);     
}

void onReportUnlockFault(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    buffer[pos++] = lock.address;
    buffer[pos++] = lock.faultType;
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

    user_protocol_send_data(CMD_QUERY, OPTION_UNLOCK_FAULT, buffer, pos);   
}

void onReportWeight(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    buffer[pos++] = lock.address;
    buffer[pos++] = (lock.magazineWeight >> 24) & 0xff;
    buffer[pos++] = (lock.magazineWeight >> 16) & 0xff;
    buffer[pos++] = (lock.magazineWeight >> 8) & 0xff;
    buffer[pos++] = lock.magazineWeight & 0xff;
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

    user_protocol_send_data(CMD_ACK, OPTION_GET_SINGLE_LOCK_WEIGHT, buffer, pos);     
}

void onReportMagazineNum(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    buffer[pos++] = lock.address;
    buffer[pos++] = (lock.magazineNum >> 8) & 0xff;
    buffer[pos++] = lock.magazineNum & 0xff;
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

    user_protocol_send_data(CMD_ACK, OPTION_SET_SINGLE_LOCK_DISP_CONTENT, buffer, pos);      
}

void onReportClrDisp(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    buffer[pos++] = lock.address;
    buffer[pos++] = (lock.magazineNum >> 8) & 0xff;
    buffer[pos++] = lock.magazineNum & 0xff;
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

    user_protocol_send_data(CMD_ACK, OPTION_CLR_SIGNLE_LOCK_DISP_CONTENT, buffer, pos);    
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
        lock.lockDelay = DEFAULT_LOCK_DELAY;
        lock.lockReplyDelay = DEFAULT_LOCK_REPLY_DELAY;
        lock.autoReportFlag = DEFAULT_LOCK_REPORT;
        lock.ledFlashStatus = DEFAULT_LOCK_LED_FLASH;
        user_database_save();
    }else{
        printf("Read database from flash!!!\r\n");
        lock.address = (uint8_t )readDataBase.address;
        lock.lockDelay = readDataBase.lockDelayLow;
        lock.lockDelay += (readDataBase.lockDelayHigh << 16);
        lock.lockReplyDelay = readDataBase.lockReplyDelay;
        lock.autoReportFlag = (uint8_t)readDataBase.autoReportFlag;
        lock.ledFlashStatus = (uint8_t)readDataBase.ledFlash;
    }

    printf("Chip uuid: 0x%x%x%x\r\n", lock.uid0, lock.uid1, lock.uid2);
    printf("address: 0x%X\r\n", lock.address);
    printf("autoReportFlag: 0x%X\r\n", lock.autoReportFlag);
    printf("ledFlash: 0x%X\r\n", lock.ledFlashStatus);
    printf("lockDelay: 0x%X\r\n", lock.lockDelay);
    printf("lockReplyDelay: 0x%X\r\n", lock.lockReplyDelay);
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
    writeDataBase.ledFlash = lock.ledFlashStatus;
    writeDataBase.lockDelayLow = lock.lockDelay & 0xffff;
    writeDataBase.lockDelayHigh = (lock.lockDelay >> 16) & 0xffff;
    writeDataBase.lockReplyDelay = lock.lockReplyDelay;

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
    if(lock.cmdControl.reportStatus.sendCmdEnable && !lock.cmdControl.reportStatus.sendCmdDelay){
        lock.cmdControl.reportStatus.sendCmdEnable = CMD_DISABLE;
        onReportDeviceStatus();
    }

    if(lock.cmdControl.operateResult.sendCmdEnable && !lock.cmdControl.operateResult.sendCmdDelay){
        lock.cmdControl.operateResult.sendCmdEnable = CMD_DISABLE;
        onReportDeviceOptResult();
    }

    if(lock.cmdControl.basicSetting.sendCmdEnable && !lock.cmdControl.basicSetting.sendCmdDelay){
        lock.cmdControl.basicSetting.sendCmdEnable = CMD_DISABLE;
        onReportSetDeviceResult();
    }

    if(lock.cmdControl.ledFlashSetting.sendCmdEnable && !lock.cmdControl.ledFlashSetting.sendCmdDelay){
        lock.cmdControl.ledFlashSetting.sendCmdEnable = CMD_DISABLE;
        onReportSetLedFlashStatus();
    }

    if(lock.cmdControl.alarmSetting.sendCmdEnable && !lock.cmdControl.alarmSetting.sendCmdDelay){
        lock.cmdControl.alarmSetting.sendCmdEnable = CMD_DISABLE;
        onReportClearDevAlarmSetting();
    }

    if(lock.cmdControl.reportOperateStatus.sendCmdEnable && !lock.cmdControl.reportOperateStatus.sendCmdDelay){
        lock.cmdControl.reportOperateStatus.sendCmdEnable = CMD_DISABLE;
        onReportDevAlarm(lock.lockState);
    }
		
    if(lock.cmdControl.reportWeight.sendCmdEnable && !lock.cmdControl.reportWeight.sendCmdDelay){
        lock.cmdControl.reportWeight.sendCmdEnable = CMD_DISABLE;
        onReportWeight();   
    }
    
    if(lock.cmdControl.reportMagazineNum.sendCmdEnable && !lock.cmdControl.reportMagazineNum.sendCmdDelay){
        lock.cmdControl.reportMagazineNum.sendCmdEnable = CMD_DISABLE;
        onReportMagazineNum();  
    }
    
    if(lock.cmdControl.clrDisp.sendCmdEnable && !lock.cmdControl.clrDisp.sendCmdDelay){
        lock.cmdControl.clrDisp.sendCmdEnable = CMD_DISABLE;
        onReportClrDisp();
    }

    if(lock.cmdControl.unlockFault.sendCmdEnable && !lock.cmdControl.unlockFault.sendCmdDelay){
        lock.cmdControl.unlockFault.sendCmdEnable = CMD_DISABLE;
        onReportUnlockFault();
    }
}

