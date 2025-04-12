#ifndef __USER_PROTOCOL_H__
#define __USER_PROTOCOL_H__


#include <stdint.h>
#include <stdlib.h>

#define MAX_PACK_LEN			100

#define FRAME_HEADER			0xAA
#define FRAME_TAIL				0x55
#define TRANSLATE_CHAR			0xCC

#define CMD_FIX_LEN				 5
#define MIN_FRAME_LEN			(8)

/**
 *协议格式：帧头（1字节）+ 指令（1字节）+ 操作码（1字节）+ 数据区长度（1字节）+ 数据区（N字节）+ 校验码（2字节）+ 帧尾（1字节）
 */

typedef enum tagEnumPTL_STATUS
{
	PTL_NO_ERROR = 0,		//解包正确
	PTL_CHECKSUM_ERROR,		//校验错误
	PTL_LENGTH_ERROR,		//包长度错误
	PTL_HEAD_ERROR,			//包头错误
	PTL_TAIL_ERROR,			//包尾错误
	PTL_DATA_ERROR			//数据域错误
	
}PTL_STATUS;

//解包数据结构
typedef struct tagUnpack
{
	uint16_t indexOfHead;		//包头索引
	uint16_t indexOfTail;		//包尾索引
	uint8_t  cmdID;				//命令ID，输出参数
	uint8_t  optionID;			//选项
	uint8_t  *pData;			//数据域缓存，输出参数
	uint16_t lenOfData;			//缓冲区数据长度
}UnPack_t;

typedef struct tagComStream
{
	uint8_t* pBuffer;		//数据缓存
	uint16_t nLenOfBuf;		//流长度
}ComStream_t;


typedef struct tagPack
{
	uint8_t cmdID;				//命令ID，输出参数
	uint8_t optionID;				//选项
	uint8_t* pData;				//数据域缓存，输出参数
	uint16_t nLenOfData;			//缓冲区数据长度
}Pack_t;

enum {
	CMD_QUERY = 0xD0,
	CMD_ACK = 0xD1,
};

enum {
	CMD_FACTORY_QUERY = 0xFA,
	CMD_FACTORY_ACK = 0xFB,
};

enum {
	OPT_CODE_SINGLE_DEV_QUERY_STATUS = 0x00,//单锁状态查询指令
	// OPT_CODE_MULTI_DEV_SET_ONOFF,//批量开关锁指令
	OPT_CODE_SINGLE_DEV_SET_ONOFF = 0x02,//单锁开关锁指令
	// OPT_CODE_MULTI_DEV_BASE_SETTING,//批量基础设置指令
	OPT_CODE_SINGLE_DEV_BASE_SETTING = 0x04,//单锁基础设置指令
	// OPT_CODE_MULTI_DEV_SET_LIGHT,//批量灯带开关设置指令
	OPT_CODE_SINGLE_DEV_SET_LIGHT = 0x06,//单灯带开关设置指令
	// OPT_CODE_MULTI_DEV_CLEAR_ALARM,//批量清除报警指令
	OPT_CODE_SINGLE_DEV_CLEAR_ALARM = 0x08,//单锁清除报警指令
	OPT_CODE_MANUAL_ALARM = 0x09,//单锁手动操作上报指令
	OPT_CODE_AUTO_LOCK = 0x0a,//延时自动关锁上报指令
	OPT_CODE_FAULT_ALARM = 0x0b,//开关锁异常上报指令
	OPT_CODE_SINGLE_DEV_QUERY_GSENSOR = 0x0c,//单锁振动查询指令
	OPT_CODE_SINGLE_DEV_QUERY_ALL_STATUS = 0x0d,//单锁查询所有状态
	OPT_CODE_REPORT_SHAKE_ALARM = 0x0e,//上报振动报警
	// OPT_CODE_MULTI_SET_SHAKE_CONFIG,//批量振动设置
	OPT_CODE_SIGNLE_SET_SHAKE_CONFIG = 0x10,//单锁振动设置
	OPT_CODE_REPORT_DOOR_STATE = 0x11,//单锁上报门磁状态
	OPT_CODE_REPORT_AUTO_LOCK_BY_DOOR_STATE = 0x12,//单锁上报因门磁自动上锁
	OPT_CODE_SET_ADDR_BY_UID = 0x13,//单锁通过UID设置地址
	OPT_CODE_GET_INFO_BY_ADDR = 0x14,//单锁通过地址获取信息
	OPT_CODE_SET_ADDR_BY_ADDR = 0x15,//单锁通过地址设置地址
	OPT_CODE_SET_REQUEST_UPGRADE = 0xe2,//单锁请求升级
	OPT_CODE_CLEAR_UART_BUFFER = 0xfd,//单锁清除串口缓存
	OPT_CODE_SINGLE_MODIFY_BAUDRATE = 0xfe,//单个设备修改波特率
	// OPT_CODE_MULTI_MODIFY_BAUDRATE,//批量修改波特率
};

enum {
	OPT_CODE_FACTORY_QUERY = 0x01,
};


//解包函数
PTL_STATUS ComUnpack(ComStream_t * inStream,UnPack_t *pResult);			
//打包函数
PTL_STATUS ComPack(const Pack_t*  pPack, ComStream_t* outStream);					

void user_protocol_handle(void);

void user_protocol_init(void);

void user_protocol_push_data(uint8_t *data, uint16_t size);

void user_protocol_send_data(uint8_t cmd, uint8_t optID, uint8_t *data, uint16_t size);

void user_set_clear_buffer_flag(uint8_t isClear);


#endif
