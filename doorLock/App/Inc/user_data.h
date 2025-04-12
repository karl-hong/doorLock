#ifndef __USER_DATA_H__
#define __USER_DATA_H__
#include <stdint.h>

typedef struct {
    uint8_t opt;
    void (*func)(uint8_t *data, uint8_t length);
}cmd_query_t;

typedef struct {
    uint8_t  *cmdEnable;
    uint16_t *cmdDelay;
    void (*func)(void);
}cmd_report_t;

extern cmd_query_t query_cmd[];
extern cmd_query_t factory_query_cmd[];
extern cmd_report_t report_cmd[];

void onCmdQuerySingleDevStatus(uint8_t *data, uint8_t length);

void onCmdSetDeviceOnOff(uint8_t *data, uint8_t length);

void onCmdModifyDeviceBasicSetting(uint8_t *data, uint8_t length);

void onCmdSetLight(uint8_t *data, uint8_t length);

void onCmdClrDevAlarmSetting(uint8_t *data, uint8_t length);

void onCmdGetGsensorData(uint8_t *data, uint8_t length);

void onCmdQuerySingleDevAllStatus(uint8_t *data, uint8_t length);

void onCmdModifyBaudRate(uint8_t *data, uint8_t length);

void onCmdModifyShakeConfig(uint8_t *data, uint8_t length);

void onCmdSetAddrByUid(uint8_t *data, uint8_t length);

void onCmdGetInfoByAddr(uint8_t *data, uint8_t length);

void onCmdSetAddrByAddr(uint8_t *data, uint8_t length);

void onCmdClearUartBuffer(uint8_t *data, uint8_t length);

void onCmdFactoryQuery(uint8_t *data, uint8_t length);

void onReportDeviceStatus(void);

void onReportSetDevOnOffStatus(void);

void onReportDeviceAllStatus(void);

void onReportBasicSetting(void);

void onReportSetLightStatus(void);

void onReportClearDevAlarmSetting(void);

void onReportManualAlarm(void);

void onReportAutoLockAlarm(void);

void onReportFaultAlarm(void);

void onReportGsensorData(void);

void onReportShakeAlarm(void);

void onReportSingleModifyBaudRate(void);

void onReportSingleModifyShakeConfig(void);

void onReportSingleDoorState(void);

void onReportSingleAutoLockByDoorState(void);

void onReportSetAddrByUid(void);

void onReportGetInfoByAddr(void);

void onReportSetAddrByAddr(void);

void onReportClearUartBuffer(void);

void onReportFactoryCmd(void);

void user_database_init(void);

void user_database_save(void);

void user_reply_handle(void);

void user_check_report_delay(void);

void printSetting(void);

#endif
