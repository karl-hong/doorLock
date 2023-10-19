#ifndef __USER_DATA_H__
#define __USER_DATA_H__
#include <stdint.h>

void onCmdQuerySingleDevStatus(uint8_t *data, uint16_t length);

void onCmdSetDeviceOnOff(uint8_t *data, uint16_t length, uint8_t ack);

void onCmdModifyDeviceBasicSetting(uint8_t *data, uint16_t length, uint8_t ack);

void onCmdSetLight(uint8_t *data, uint16_t length, uint8_t ack);

void onCmdClrDevAlarmSetting(uint8_t *data, uint16_t length, uint8_t ack);

void onCmdGetGsensorData(uint8_t *data, uint16_t length);

void onCmdQuerySingleDevAllStatus(uint8_t *data, uint16_t length);

void onCmdModifyBaudRate(uint8_t *data, uint16_t length, uint8_t ack);

void onCmdModifyShakeConfig(uint8_t *data, uint16_t length, uint8_t ack);

void onReportDeviceStatus(void);

void onReportSetDevOnOffStatus(void);

void onReportBasicSetting(void);

void onReportSetLightStatus(void);

void onReportClearDevAlarmSetting(void);

void onReportManualAlarm(uint8_t alarmType);

void onReportAutoLockAlarm(void);

void onReportFaultAlarm(void);

void onReportGsensorData(void);

void onReportShakeAlarm(void);

void onReportSingleModifyBaudRate(void);

void onReportSingleModifyShakeConfig(void);

void user_database_init(void);

void user_database_save(void);

void user_reply_handle(void);

void printSetting(void);

#endif
