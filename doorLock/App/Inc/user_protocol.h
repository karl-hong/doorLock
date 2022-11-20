#ifndef __USER_PROTOCOL_H__
#define __USER_PROTOCOL_H__


#include <stdint.h>
#include <stdlib.h>

#define MAX_PACK_LEN			100

#define FRAME_HEADER			0xAA
#define FRAME_TAIL				0x55
#define TRANSLATE_CHAR			0xCC

#define CMD_FIX_LEN				 6
#define MIN_FRAME_LEN			(9)

/**
 *协议格式：帧头（1字节）+ 指令（1字节）+ 操作码（1字节）+ 数据区长度（2字节）+ 数据区（N字节）+ 校验码（2字节）+ 帧尾（1字节）
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
	CMD_QUERY = 0xC0,
	CMD_ACK = 0xC1,
};

enum {
	OPTION_QUERY_SINGLE_LOCK_STATUS = 0,//单板状态查询指令
	OPTION_QUERY_ALL_LOCKS_STATUS,//批量开锁指令
	OPTION_LOCK_SINGLE_DEVICE,//单板开锁指令
	OPTION_SET_ALL_DEVICES,//批量基础设置指令
	OPTION_SET_SINGLE_DEVICE,//单板基础设置指令
	OPTION_SET_ALL_DEVICES_LED,//批量指示灯闪烁设置指令
	OPTION_SET_SINGLE_DEVICE_LED,//单板指示灯闪烁设置指令
	OPTION_CLR_ALL_DEVICES_ALARM_SETTING,//批量报警清除指令
	OPTION_CLR_SINGLE_DEVICE_ALARM_SETTING,//单板报警清除指令
	OPTION_GET_SINGLE_LOCK_WEIGHT,//单板获取称重传感器重量值
	OPTION_SET_SINGLE_LOCK_DISP_CONTENT,//单板设置数据管显示内容
	OPTION_CLR_SIGNLE_LOCK_DISP_CONTENT,//单板清空数据管显示内容
	OPTION_CLR_ALL_LOCKS_DISP_CONTENT,//批量清空数码管显示内容
	OPTION_MANUAL_ALARM,//单板手动开/关报警上报
	OPTION_UNLOCK_FAULT,
};


//解包函数
PTL_STATUS ComUnpack(ComStream_t * inStream,UnPack_t *pResult);			
//打包函数
PTL_STATUS ComPack(const Pack_t*  pPack, ComStream_t* outStream);					

void user_protocol_handle(void);

void user_protocol_init(void);

void user_protocol_push_data(uint8_t *data, uint16_t size);

void user_protocol_send_data(uint8_t cmd, uint8_t optID, uint8_t *data, uint16_t size);


#endif
