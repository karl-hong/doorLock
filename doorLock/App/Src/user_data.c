#include <stdio.h>
#include <string.h>
#include "user_data.h"
#include "common.h"
#include "user_protocol.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash_ex.h"  
#include "led.h"
#include "motor.h"

static uint32_t packetIndex;
static uint32_t packetSize;
static uint8_t upgradeStatus;
static uint8_t packetData[PACKET_SIZE];
/**
 * @brief 添加监听指令步骤
 * （1）在user_protocol.h中定义opt
 * （2）编写监听解析函数
 * （3）在query_cmd增加对应接口
 */

cmd_query_t query_cmd[] = {
    {OPT_CODE_SINGLE_DEV_QUERY_STATUS,        onCmdQuerySingleDevStatus},
    {OPT_CODE_SINGLE_DEV_SET_ONOFF,           onCmdSetDeviceOnOff},
    {OPT_CODE_SINGLE_DEV_BASE_SETTING,        onCmdModifyDeviceBasicSetting},
    {OPT_CODE_SINGLE_DEV_SET_LIGHT,           onCmdSetLight},
    {OPT_CODE_SINGLE_DEV_CLEAR_ALARM,         onCmdClrDevAlarmSetting},
    {OPT_CODE_SINGLE_DEV_QUERY_GSENSOR,       onCmdGetGsensorData},
    {OPT_CODE_SINGLE_DEV_QUERY_ALL_STATUS,    onCmdQuerySingleDevAllStatus},
    {OPT_CODE_SIGNLE_SET_SHAKE_CONFIG,        onCmdModifyShakeConfig},
    {OPT_CODE_SET_ADDR_BY_UID,                onCmdSetAddrByUid},
    {OPT_CODE_GET_INFO_BY_ADDR,               onCmdGetInfoByAddr},
    {OPT_CODE_SET_ADDR_BY_ADDR,               onCmdSetAddrByAddr},
    {OPT_CODE_SET_REQUEST_UPGRADE,            onCmdRequestUpgrade},
    {OPT_CODE_CLEAR_UART_BUFFER,              onCmdClearUartBuffer},
    {OPT_CODE_SINGLE_MODIFY_BAUDRATE,         onCmdModifyBaudRate},
    {0, NULL},//must end with NULL
};

cmd_query_t factory_query_cmd[] = {
    {OPT_CODE_FACTORY_QUERY,            onCmdFactoryQuery},
    {0, NULL},//must end with NULL
};

/**
 * @brief 添加发布指令步骤
 * （1）在user_protocol.h中定义opt
 * （2）修改common.h中结构体cmd_control_t，增加对应的发布指令
 * （3）编写监听解析函数
 * （4）在report_cmd增加对应接口
 */

cmd_report_t report_cmd[] = {
    {&lock.cmdControl.singleQueryStatus.sendCmdEnable, &lock.cmdControl.singleQueryStatus.sendCmdDelay, onReportDeviceStatus},
    {&lock.cmdControl.singleSetOnOff.sendCmdEnable, &lock.cmdControl.singleSetOnOff.sendCmdDelay, onReportSetDevOnOffStatus},
    {&lock.cmdControl.singleBasicSetting.sendCmdEnable, &lock.cmdControl.singleBasicSetting.sendCmdDelay, onReportBasicSetting},
    {&lock.cmdControl.singleSetLight.sendCmdEnable, &lock.cmdControl.singleSetLight.sendCmdDelay, onReportSetLightStatus},
    {&lock.cmdControl.singleClrAlarm.sendCmdEnable, &lock.cmdControl.singleClrAlarm.sendCmdDelay, onReportClearDevAlarmSetting},
    {&lock.cmdControl.singleManualAlarm.sendCmdEnable, &lock.cmdControl.singleManualAlarm.sendCmdDelay, onReportManualAlarm},
    {&lock.cmdControl.autoLockAlarm.sendCmdEnable, &lock.cmdControl.autoLockAlarm.sendCmdDelay, onReportAutoLockAlarm},
    {&lock.cmdControl.faultAlarm.sendCmdEnable, &lock.cmdControl.faultAlarm.sendCmdDelay, onReportFaultAlarm},
    {&lock.cmdControl.singleQueryGsensor.sendCmdEnable, &lock.cmdControl.singleQueryGsensor.sendCmdDelay, onReportGsensorData},
    {&lock.cmdControl.singleQueryAllStatus.sendCmdEnable, &lock.cmdControl.singleQueryAllStatus.sendCmdDelay, onReportDeviceAllStatus},
    {&lock.cmdControl.shakeReport.sendCmdEnable, &lock.cmdControl.shakeReport.sendCmdDelay, onReportShakeAlarm},
    {&lock.cmdControl.singleModifyShakeConfig.sendCmdEnable, &lock.cmdControl.singleModifyShakeConfig.sendCmdDelay, onReportSingleModifyShakeConfig},
    {&lock.cmdControl.singleRportAutoLockByDoorState.sendCmdEnable, &lock.cmdControl.singleRportAutoLockByDoorState.sendCmdDelay, onReportSingleAutoLockByDoorState},
    {&lock.cmdControl.singleReportDoorState.sendCmdEnable, &lock.cmdControl.singleReportDoorState.sendCmdDelay, onReportSingleDoorState},
    {&lock.cmdControl.setAddrByUid.sendCmdEnable, &lock.cmdControl.setAddrByUid.sendCmdDelay, onReportSetAddrByUid},
    {&lock.cmdControl.getInfoByAddr.sendCmdEnable, &lock.cmdControl.getInfoByAddr.sendCmdDelay, onReportGetInfoByAddr},
    {&lock.cmdControl.setAddrByAddr.sendCmdEnable, &lock.cmdControl.setAddrByAddr.sendCmdDelay, onReportSetAddrByAddr},
    {&lock.cmdControl.clearUartBuffer.sendCmdEnable, &lock.cmdControl.clearUartBuffer.sendCmdDelay, onReportClearUartBuffer},
    {&lock.cmdControl.singleModifyBaudRate.sendCmdEnable, &lock.cmdControl.singleModifyBaudRate.sendCmdDelay, onReportSingleModifyBaudRate},
    {&lock.cmdControl.factoryCmd.sendCmdEnable, &lock.cmdControl.factoryCmd.sendCmdDelay, onReportFactoryCmd},
    {&lock.cmdControl.upgrade.sendCmdEnable, &lock.cmdControl.upgrade.sendCmdDelay, onReportRequestUpgrade},
};

void onCmdQuerySingleDevStatus(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
    uint8_t pos = 0;

    addr = data[pos++];

    if(IS_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* send dev status here */
    lock.cmdControl.singleQueryStatus.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.singleQueryStatus.sendCmdDelay = 0;
}

void onCmdSetDeviceOnOff(uint8_t *data, uint8_t length)
{
 
    uint8_t pos = 0;
    uint8_t lockSetState;
    uint8_t addr = 0;

    lockSetState = data[pos++];
    addr = data[pos++];

    if(CHECK_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* set dev state here */
    if(lockSetState == 0 && lock.lockState){//unlock
        motor_set_backward();
    }else if(lockSetState && !lock.lockState){//lock
        if(lock.doorDetectState1 && lock.doorDetectState2)    motor_set_forward();
    }
    /* send ack msg here */
    if(CHECK_ACK(addr)){
        lock.cmdControl.singleSetOnOff.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleSetOnOff.sendCmdDelay = lock.lockReplyDelay * DELAY_BASE;
    }
}

void onCmdModifyDeviceBasicSetting(uint8_t *data, uint8_t length)
{
    uint8_t pos = 0;
    uint32_t autoLockDelay;
    uint8_t autoLockFlag;
    uint8_t  autoReportFlag;
    uint8_t  addr;
    uint16_t lockStopDelay;
    uint16_t unlockStopDelay;
	uint8_t autoCloseDoorEnable;
	uint16_t autoCloseDoorDelay;

	/* auto lock delay, 3bytes */
    autoLockDelay = data[pos++] << 16;
    autoLockDelay += data[pos++] << 8;
    autoLockDelay += data[pos++];

	/* auto lock flag, 1byte */
    autoLockFlag = data[pos++];

	/* auto report flag, 1byte */
    autoReportFlag = data[pos++];

	/* unlock stop delay, 2byte */
    unlockStopDelay = data[pos++] << 8;
	unlockStopDelay += data[pos++];

	/* lock stop delay, 2byte */
    lockStopDelay = data[pos++] << 8;
	lockStopDelay += data[pos++];
	
	/* auto close door */
	autoCloseDoorEnable = data[pos++];
	autoCloseDoorDelay = data[pos++] << 8;
	autoCloseDoorDelay += data[pos++];

    /* addr */
    addr = data[pos++];

    if(CHECK_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* set dev state here */
    lock.unlockStopDelay = unlockStopDelay;
    lock.lockStopDelay = lockStopDelay;
    lock.autoLockTime = autoLockDelay;
    lock.autoLockFlag = autoLockFlag;
    lock.autoReportFlag = autoReportFlag;
	lock.autoCloseDoorEnable = autoCloseDoorEnable;
	lock.autoCloseDoorDelay = autoCloseDoorDelay;
    
    user_database_save();
    /* send ack msg here */
    if(CHECK_ACK(addr)){
        lock.cmdControl.singleBasicSetting.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleBasicSetting.sendCmdDelay = 0;
    }
}

void  onCmdSetLight(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
    uint8_t pos = 0;
    uint8_t  lightState1;
    uint8_t  lightState2;

    lightState1 = data[pos++];
    lightState2 = data[pos++];
    addr = data[pos++];
    if(CHECK_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    if(lightState1) set_light1_on();
    else            set_light1_off();
    if(lightState2) set_light2_on();
    else            set_light2_off();

    /* send ack msg here */
    if(CHECK_ACK(addr)){
        lock.cmdControl.singleSetLight.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleSetLight.sendCmdDelay = 0;
    }
}

void onCmdClrDevAlarmSetting(uint8_t *data, uint8_t length)
{
    uint8_t pos = 0;
    uint8_t addr = 0;

    addr = data[pos++];
    if (CHECK_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }
    /* clear device alarm setting here */
    lock.alarmStatus = 0;
    user_database_save();
    /* send ack msg here */
    if(CHECK_ACK(addr)){
        lock.cmdControl.singleClrAlarm.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.singleClrAlarm.sendCmdDelay = 0;
    } 
}

void onCmdGetGsensorData(uint8_t *data, uint8_t length)
{
    uint8_t pos = 0;
    uint8_t addr = 0;

    addr = data[pos++];
    if(IS_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* send dev status here */
    lock.cmdControl.singleQueryGsensor.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.singleQueryGsensor.sendCmdDelay = 0;    
}

void onCmdQuerySingleDevAllStatus(uint8_t *data, uint8_t length)
{
    uint8_t pos = 0;
    uint8_t addr = 0;

    addr = data[pos++];
    if(IS_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* send dev status here */
    lock.cmdControl.singleQueryAllStatus.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.singleQueryAllStatus.sendCmdDelay = 0;
}

void onCmdModifyBaudRate(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
    uint8_t pos = 0;
    uint8_t baudRateIndex = 0;

    baudRateIndex = data[pos++];
    addr = data[pos++];

    if(IS_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    lock.baudRateIndex = baudRateIndex;
    user_database_save();

    lock.cmdControl.singleModifyBaudRate.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.singleModifyBaudRate.sendCmdDelay = 0;
}

void onCmdModifyShakeConfig(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
	uint8_t pos = 0;
	uint8_t shakeThresold;
	uint8_t xReportFlag;
	uint8_t yReportFlag;
	uint8_t zReportFlag;
	uint16_t shakeReportInterval;

	shakeThresold = data[pos++];
	xReportFlag = data[pos++];
	yReportFlag = data[pos++];
	zReportFlag = data[pos++];
	shakeReportInterval = data[pos++] << 8;
	shakeReportInterval += data[pos++];

    addr = data[pos++];
    if(CHECK_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

	/* set dev state here */
	lock.shakeThreshold = shakeThresold;
	lock.xReportFlag = xReportFlag;
	lock.yReportFlag = yReportFlag;
	lock.zReportFlag = zReportFlag;
	lock.shakeReportInterval = shakeReportInterval;
	
	user_database_save();
	/* send ack msg here */
	if(CHECK_ACK(addr)){
		lock.cmdControl.singleModifyShakeConfig.sendCmdEnable = CMD_ENABLE;
		lock.cmdControl.singleModifyShakeConfig.sendCmdDelay = 0;
	}else{
		HAL_NVIC_SystemReset();
	}
}

void onCmdSetAddrByUid(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
    uint8_t pos = 0;
    uint32_t uid0 = 0;
    uint32_t uid1 = 0;
    uint32_t uid2 = 0;

    /* UID */
    uid0 = data[pos++] << 24;
    uid0 += data[pos++] << 16;
    uid0 += data[pos++] << 8;
    uid0 += data[pos++];
    uid1 = data[pos++] << 24;
    uid1 += data[pos++] << 16;
    uid1 += data[pos++] << 8;
    uid1 += data[pos++];
    uid2 = data[pos++] << 24;
    uid2 += data[pos++] << 16;
    uid2 += data[pos++] << 8;
    uid2 += data[pos++];
    /* new address */
    addr = data[pos++];

    if(IS_UID_INVALID(uid0, uid1, uid2)){
        printf("[%s]uid is not matched!\r\n", __FUNCTION__);
        return;
    }

    if(BROADCAST_ADDR == addr){
        printf("[%s]address is invalid!\r\n", __FUNCTION__);
        return;
    }

    lock.address = addr;
    user_database_save();
    
    /* send ack msg here */
    lock.cmdControl.setAddrByUid.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.setAddrByUid.sendCmdDelay = 0;
}

void onCmdGetInfoByAddr(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
    uint8_t pos = 0;

    addr = data[pos++];
    if(IS_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* send dev status here */
    lock.cmdControl.getInfoByAddr.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.getInfoByAddr.sendCmdDelay = 0;
}

void onCmdSetAddrByAddr(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
    uint8_t pos = 0;
    uint8_t newAddr = 0;

    newAddr = data[pos++];
    addr = data[pos++];

    if(IS_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    if(BROADCAST_ADDR == newAddr){
        printf("[%s]address is invalid!\r\n", __FUNCTION__);
        return;
    }

    lock.address = newAddr;
    lock.oldAddr = addr;
    user_database_save();
    
    /* send ack msg here */
    lock.cmdControl.setAddrByAddr.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.setAddrByAddr.sendCmdDelay = 0;
}

void onCmdClearUartBuffer(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
    uint8_t pos = 0;

    addr = data[pos++];
    if(IS_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    /* clear uart buffer here */
    user_set_clear_buffer_flag(1);
    
    /* send ack msg here */
    lock.cmdControl.clearUartBuffer.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.clearUartBuffer.sendCmdDelay = 0;
}

void onCmdFactoryQuery(uint8_t *data, uint8_t length)
{
    lock.cmdControl.factoryCmd.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.factoryCmd.sendCmdDelay = 0;
}

void onCmdRequestUpgrade(uint8_t *data, uint8_t length)
{
    uint8_t addr = 0;
    uint8_t pos = 0;

    addr = data[pos++];
    upgradeStatus = data[pos++];
    packetSize = (data[pos++] << 24);
    packetSize += (data[pos++] << 16);
    packetSize += (data[pos++] << 8);
    packetSize += data[pos++];

    packetIndex = (data[pos++] << 24);
    packetIndex += (data[pos++] << 16);
    packetIndex += (data[pos++] << 8);
    packetIndex += data[pos++];

    for(uint8_t i=0;i<PACKET_SIZE;i++){
        packetData[i] = data[pos++];
    }

    if(IS_ADDR_INVALID(addr)){
        printf("[%s]address is not matched!\r\n", __FUNCTION__);
        return;
    }

    if(upgradeStatus != STATUS_REQUEST_UPGRADE && upgradeStatus != STATUS_UPGRADE_GOING){
        printf("[%s]request upgrade failed!\r\n", __FUNCTION__);
        return;
    }

    /* send ack msg here */
    lock.cmdControl.upgrade.sendCmdEnable = CMD_ENABLE;
    lock.cmdControl.upgrade.sendCmdDelay = 0;
}

void onReportDeviceStatus(void)
{
    uint8_t buffer[60];
    uint8_t pos = 0;
	uint16_t version;
    
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
	/* state auto lock enable */
	buffer[pos++] = lock.autoCloseDoorEnable;
	/* state auto lock delay */
	buffer[pos++] = (lock.autoCloseDoorDelay >> 8) & 0xff;
    buffer[pos++] = lock.autoCloseDoorDelay & 0xff;
	/* version */
	version = VERSION;
	buffer[pos++] = (version >> 8) & 0xff;
    buffer[pos++] = version & 0xff;
    /* addr */
    buffer[pos++] = lock.address;
    
    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_QUERY_STATUS, buffer, pos);       
}

void onReportSetDevOnOffStatus(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    /* lock status */
    buffer[pos++] = lock.lockState;
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_SET_ONOFF, buffer, pos);     
}

void onReportBasicSetting(void)
{
    uint8_t buffer[50];
    uint8_t pos = 0;
    
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
	/* state lock enable */
	buffer[pos++] = lock.autoCloseDoorEnable;
	/* state lock delay */
	buffer[pos++] = (lock.autoCloseDoorDelay >> 8) & 0xff;
    buffer[pos++] = lock.autoCloseDoorDelay & 0xff;
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_BASE_SETTING, buffer, pos);   
}

void onReportSetLightStatus(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    /* light1 state */
    buffer[pos++] = lock.lightState1;
    /* light2 state */
    buffer[pos++] = lock.lightState2;
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_SET_LIGHT, buffer, pos); 
}

void onReportClearDevAlarmSetting(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    /* alarm status */
    buffer[pos++] = lock.alarmStatus;
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_CLEAR_ALARM, buffer, pos);     
}

void onReportManualAlarm(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
        /* UID */
    buffer[pos++] = (lock.uid0 >> 24) & 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24) & 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24) & 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;
    
    buffer[pos++] = lock.keyDetectState;
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_QUERY, OPT_CODE_MANUAL_ALARM, buffer, pos);     
}

void onReportAutoLockAlarm(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    buffer[pos++] = lock.address;
    user_protocol_send_data(CMD_QUERY, OPT_CODE_AUTO_LOCK, buffer, pos);     
}

void onReportFaultAlarm(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    buffer[pos++] = lock.motorTask.faultType;
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_QUERY, OPT_CODE_FAULT_ALARM, buffer, pos);   
}

void onReportGsensorData(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    /* gsensor data */
    buffer[pos++] = (lock.gSensor.x >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.x & 0xff;
    buffer[pos++] = (lock.gSensor.y >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.y & 0xff;
    buffer[pos++] = (lock.gSensor.z >> 8) & 0xff;
    buffer[pos++] = lock.gSensor.z & 0xff;
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_QUERY_GSENSOR, buffer, pos);     
}

void onReportDeviceAllStatus(void)
{
    uint8_t buffer[50];
    uint8_t pos = 0;
    
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
    /* addr */
    buffer[pos++] = lock.address;
    
    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_DEV_QUERY_ALL_STATUS, buffer, pos);       
}


void onReportShakeAlarm(void)
{
    uint8_t buffer[50];
    uint8_t pos = 0;

    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_QUERY, OPT_CODE_REPORT_SHAKE_ALARM, buffer, pos);       
}

void onReportSingleModifyBaudRate(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    /* lock baudRateIndex */
    buffer[pos++] = lock.baudRateIndex;
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_MODIFY_BAUDRATE, buffer, pos); 

    HAL_Delay(100);
    HAL_NVIC_SystemReset();
}

void onReportSingleModifyShakeConfig(void)
{
	uint8_t buffer[50];
	uint8_t pos = 0;
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
	/* addr */
    buffer[pos++] = lock.address;

	user_protocol_send_data(CMD_ACK, OPT_CODE_SIGNLE_SET_SHAKE_CONFIG, buffer, pos);   

	HAL_Delay(100);
    HAL_NVIC_SystemReset();
}

void onReportSingleDoorState(void)
{
	uint8_t buffer[40];
	uint8_t pos = 0;

	/* last state */
	buffer[pos++] = lock.lastDoorState;
	/* current state */
	buffer[pos++] = lock.curDoorState;
    /* addr */
	buffer[pos++] = lock.address;

	user_protocol_send_data(CMD_QUERY, OPT_CODE_REPORT_DOOR_STATE, buffer, pos); 

	lock.lastDoorState = lock.curDoorState;

}

void onReportSingleAutoLockByDoorState(void)
{
	uint8_t buffer[40];
	uint8_t pos = 0;
	/* addr */
	buffer[pos++] = lock.address;

	user_protocol_send_data(CMD_QUERY, OPT_CODE_REPORT_AUTO_LOCK_BY_DOOR_STATE, buffer, pos); 
}

void onReportSetAddrByUid(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    /* UID */
    buffer[pos++] = (lock.uid0 >> 24) & 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24) & 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24) & 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SET_ADDR_BY_UID, buffer, pos);     
}

void onReportGetInfoByAddr(void)
{
    uint8_t buffer[30];
    uint8_t pos = 0;

    /* UID */
    buffer[pos++] = (lock.uid0 >> 24) & 0xff;
    buffer[pos++] = (lock.uid0 >> 16) & 0xff;
    buffer[pos++] = (lock.uid0 >> 8) & 0xff;
    buffer[pos++] = lock.uid0 & 0xff;
    buffer[pos++] = (lock.uid1 >> 24) & 0xff;
    buffer[pos++] = (lock.uid1 >> 16) & 0xff;
    buffer[pos++] = (lock.uid1 >> 8) & 0xff;
    buffer[pos++] = lock.uid1 & 0xff;
    buffer[pos++] = (lock.uid2 >> 24) & 0xff;
    buffer[pos++] = (lock.uid2 >> 16) & 0xff;
    buffer[pos++] = (lock.uid2 >> 8) & 0xff;
    buffer[pos++] = lock.uid2 & 0xff;
    /* version */
    buffer[pos++] = (VERSION >> 8) & 0xff;
    buffer[pos++] = VERSION & 0xff;
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_GET_INFO_BY_ADDR, buffer, pos);     
}

void onReportSetAddrByAddr(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    /* addr */
    buffer[pos++] = lock.address;
    /* old addr */
    buffer[pos++] = lock.oldAddr;

    user_protocol_send_data(CMD_ACK, OPT_CODE_SET_ADDR_BY_ADDR, buffer, pos);     
}

void onReportClearUartBuffer(void)
{
    uint8_t buffer[23];
    uint8_t pos = 0;
    
    /* addr */
    buffer[pos++] = lock.address;

    user_protocol_send_data(CMD_ACK, OPT_CODE_CLEAR_UART_BUFFER, buffer, pos);     
}

void onReportFactoryCmd(void)
{
    uint8_t buffer[50];
    uint8_t pos = 0;

    /* type */
    buffer[pos++] = CMD_ACK;

    /* addr */
    buffer[pos++] = lock.address;

    /* uid */
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

    user_protocol_send_data(CMD_FACTORY_ACK, OPT_CODE_FACTORY_QUERY, buffer, pos); 
}

void onReportRequestUpgrade(void)
{

	printf("[%s]status: %d, packet index: %d, packet num: %d\r\n", __FUNCTION__, upgradeStatus, packetIndex, packetSize);
    uint8_t buffer[50];
    uint8_t pos = 0;

    switch(upgradeStatus){
        case STATUS_REQUEST_UPGRADE:{
             if(0 != write_upgrade_flag()){
                printf("Write upgrade flag fail!");
                return;
            }
            HAL_NVIC_SystemReset();//boot再回复
            return;
        }

        case STATUS_UPGRADE_GOING:{
            if(packetSize && packetSize == packetIndex){
                upgradeStatus = STATUS_UPGRADE_SUCCESS;
            }else{
                return;
            }
            break;
        }

        case STATUS_UPGRADE_SUCCESS:break;
        default:return;
    }

    /* 地址 */
    buffer[pos++] = lock.address;
    /* 状态 */
    buffer[pos++] = upgradeStatus;
    /* 包总数 */
    buffer[pos++] = (packetSize >> 24) & 0xFF;
    buffer[pos++] = (packetSize >> 16) & 0xFF;
    buffer[pos++] = (packetSize >> 8) & 0xFF;
    buffer[pos++] = packetSize & 0xFF;
    /* 包序号 */
    buffer[pos++] = (packetIndex >> 24) & 0xFF;
    buffer[pos++] = (packetIndex >> 16) & 0xFF;
    buffer[pos++] = (packetIndex >> 8) & 0xFF;
    buffer[pos++] = packetIndex & 0xFF;
    /* 数据 */
    for(int i =0; i< PACKET_SIZE;i++){
        buffer[pos++] = packetData[i];
    }

    // printf("[%s]status: %d, packet index: %d, packet num: %d\r\n", __FUNCTION__, upgradeStatus, packetIndex, packetSize);
	user_protocol_send_data(CMD_ACK, OPT_CODE_SET_REQUEST_UPGRADE, buffer, pos);  
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
        lock.baudRateIndex = DEFAULT_BAUD_RATE_INDEX;
		lock.autoCloseDoorEnable = DEFAULT_AUTO_CLOSE_DOOR_FLAG;
		lock.autoCloseDoorDelay = DEFAULT_AUTO_CLOSE_DOOR_DELAY;

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
        lock.baudRateIndex = (readDataBase.baudRateIndex == 0xffff) ? DEFAULT_BAUD_RATE_INDEX : readDataBase.baudRateIndex;
		lock.autoCloseDoorEnable = (readDataBase.autoCloseDoorEnable == 0xffff) ? DEFAULT_AUTO_CLOSE_DOOR_FLAG : readDataBase.autoCloseDoorEnable;
		lock.autoCloseDoorDelay = (readDataBase.autoCloseDoorDelay == 0xffff) ? DEFAULT_AUTO_CLOSE_DOOR_DELAY : readDataBase.autoCloseDoorDelay;
        //lock.ledFlashStatus = (uint8_t)readDataBase.ledFlash;
    }

	lock.disableReport = 0;
	lock.disableReportLatency = 0;
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
    writeDataBase.baudRateIndex = lock.baudRateIndex;
	writeDataBase.autoCloseDoorEnable = lock.autoCloseDoorEnable;
	writeDataBase.autoCloseDoorDelay = lock.autoCloseDoorDelay;

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
    uint16_t funcNum = sizeof(report_cmd) / sizeof(cmd_report_t);
    for(uint16_t i = 0; i < funcNum; i++){
        if(*report_cmd[i].cmdEnable && !(*report_cmd[i].cmdDelay)){
            *report_cmd[i].cmdEnable = CMD_DISABLE;
            report_cmd[i].func();
        }
    }
}

void user_check_report_delay(void)
{
    uint16_t funcNum = sizeof(report_cmd) / sizeof(cmd_report_t);
    for(uint16_t i = 0; i < funcNum; i++){
        if(*report_cmd[i].cmdEnable && (*report_cmd[i].cmdDelay > 0)){
            *report_cmd[i].cmdDelay --;
        }
    }
}

void printSetting(void)
{
	printf("Chip uuid: 0x%04x%04x%04x\r\n", lock.uid0, lock.uid1, lock.uid2);
    printf("address: 0x%02X\r\n", lock.address);
    printf("autoReportFlag: 0x%02X\r\n", lock.autoReportFlag);
    printf("lockDelayEnable: 0x%02X\r\n", lock.autoLockFlag);
    printf("lockDelay: 0x%04X\r\n", lock.autoLockTime);
    printf("lockReplyDelay: 0x%02X\r\n", lock.lockReplyDelay);
	printf("baudRateIndex: 0x%02X\r\n", lock.baudRateIndex);
}

int user_write_flash(uint32_t address, uint16_t *data, uint16_t size)
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef flashEraseInitType;
    uint32_t PageError;
    int ret = 0;

    if(NULL == data)    return -1;

    HAL_FLASH_Unlock();

    flashEraseInitType.TypeErase = FLASH_TYPEERASE_PAGES;
    flashEraseInitType.PageAddress = address;
    flashEraseInitType.NbPages = 1;
    status = HAL_FLASHEx_Erase(&flashEraseInitType, &PageError);
    
    if(HAL_OK != status){
        HAL_FLASH_Lock();
        printf("Flash erase error: %d\r\n", status);
        return -1;
    }

    for(uint16_t i=0;i<size;i++){
        if(HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + 2U*i, data[i])){
            printf("Write data[%d] fail!\r\n", i);
            ret = -1;
        }
    }

    HAL_FLASH_Lock();

    return ret;
}

int write_upgrade_flag(void)
{
    upgrade_t upgradeData;
    uint16_t *pData = NULL;
    uint16_t lenOfDataBase = sizeof(upgrade_t) / sizeof(uint16_t);

    upgradeData.magic = DATABASE_MAGIC;
    upgradeData.address = lock.address;
    upgradeData.deviceCmd = CMD_QUERY;
    upgradeData.baudIndex = lock.baudRateIndex;
    upgradeData.upgradeFlag = APP_UPGREQ_IS_VALID;

    pData = (uint16_t *)&upgradeData;

    return user_write_flash(APP_UPGRADE_FLAG_ADDRESS, pData, lenOfDataBase);
}

void sync_boot_env(void)
{
    uint16_t i;
    uint16_t lenOfDataBase = sizeof(upgrade_t) / sizeof(uint16_t);
    upgrade_t readDataBase;
    uint16_t *pData = (uint16_t *)&readDataBase;

    for(i=0;i<lenOfDataBase;i++){
        pData[i] = user_read_flash(APP_UPGRADE_FLAG_ADDRESS + 2U*i);
    }

    if(DATABASE_MAGIC == readDataBase.magic && readDataBase.upgradeStatus == STATUS_UPGRADE_SUCCESS){
        /* sync boot env */
        upgradeStatus = STATUS_UPGRADE_SUCCESS;
        packetSize = readDataBase.packetSize;
        packetIndex = readDataBase.packetIndex;
        for(uint8_t i=0;i<PACKET_SIZE;i++){
            packetData[i] = readDataBase.packetData[i];
        }
        /* send msg */
        lock.cmdControl.upgrade.sendCmdEnable = CMD_ENABLE;
        lock.cmdControl.upgrade.sendCmdDelay = 0;
        /* clear upgrade status */
        readDataBase.upgradeStatus = 0;
        user_write_flash(APP_UPGRADE_FLAG_ADDRESS, pData, lenOfDataBase);
        printf("Upgrade done!!!!!\r\n");
    }
}


