#ifndef __SC7A20_H__
#define __SC7A20_H__
#include <stdint.h>

#define SAD0L			0x00
#define SAD0H			0x01

#define SC7A20_CHIP_ID					0x11

#define SC7A20_ACC_I2C_SADROOT	0x0c 

#define SC7A20_ACC_I2C_SAD_L	((SC7A20_ACC_I2C_SADROOT<<1)|SAD0L)     //0x18
#define SC7A20_ACC_I2C_SAD_H	((SC7A20_ACC_I2C_SADROOT<<1)|SAD0H)     //0x19

#define SC7A20_ACC_I2C_ADDR     SC7A20_ACC_I2C_SAD_H
#define SC7A20_ACC_I2C_NAME     "sc7a20_acc"
#define	SC7A20_ACC_DEV_NAME	    "sc7a20_acc"


/* Accelerometer Sensor Full Scale */
#define	SC7A20_ACC_FS_MASK		0x30
#define SC7A20_ACC_G_2G 			0x00
#define SC7A20_ACC_G_4G 			0x10
#define SC7A20_ACC_G_8G 			0x20
#define SC7A20_ACC_G_16G 	    0x30

/* Accelerometer Sensor Operating Mode */
#define SC7A20_ACC_ENABLE			0x01
#define SC7A20_ACC_DISABLE		0x00

#define SENSITIVITY_2G				1	/**	mg/LSB	*/
#define SENSITIVITY_4G				2	/**	mg/LSB	*/
#define SENSITIVITY_8G				4	/**	mg/LSB	*/
#define SENSITIVITY_16G				12	/**	mg/LSB	*/

#define	HIGH_RESOLUTION				0x08


/* Accelerometer Sensor BDU Mask */
#define SC7A20_ACC_BDU_MASK         0x80

/* Accelerometer Sensor BLE Mask */
#define SC7A20_ACC_BLE_MASK         0x40

/* Accelerometer Sensor HR Mask */
#define SC7A20_ACC_HR_MASK          0x08

#define WHOAMI_SC7A20_ACC	        	0x11

/* REGISTERS */
#define WHO_AM_I					0x0F	/*      WhoAmI register         */
#define CTRL_REG1					0x20
#define CTRL_REG2					0x21
#define CTRL_REG3					0x22
#define CTRL_REG4					0x23
#define CTRL_REG5					0x24
#define CTRL_REG6					0x25
#define STATUS_REG				0x27
#define XOUT_L						0x28
#define XOUT_H						0x29
#define YOUT_L						0x2a
#define YOUT_H						0x2b
#define ZOUT_L						0x2c
#define ZOUT_H						0x2d
#define INT1_CFG					0x30
#define INT1_SOURCE				0x31
#define INT1_THS					0x32
#define INT1_DURATION			0x33
#define INT2_CFG					0x34
#define INT2_SOURCE				0x35
#define INT2_THS					0x36
#define INT2_DURATION			0x37
#define CLICK_CFG					0x38
#define CLICK_SRC					0x39
#define CLICK_THS					0x3a
#define TIME_LIMIT				0x3b
#define TIME_LATENCY			0x3c
#define TIME_WINDOW				0x3d
#define ACT_THS						0x3e
#define ACT_DURATION			0x3f

#define SC7A20_ENABLE_ALL_AXES	    0x07
#define SC7A20_AC_MASK              0xF0

/* Accelerometer Sensor odr set */
#define ODR1		                0x10	 /* 1Hz output data rate */
#define ODR10		                0x20	 /* 10Hz output data rate */
#define ODR25		                0x30	 /* 25Hz output data rate */
#define ODR50		                0x40	 /* 50Hz output data rate */
#define ODR100		                0x50	 /* 100Hz output data rate */
#define ODR200		                0x60	 /* 200Hz output data rate */
#define ODR400		                0x70	 /* 400Hz output data rate */
#define ODR1250		                0x90	 /* 1250Hz output data rate */

/* INPUT_ABS CONSTANTS */
#define FUZZ			            32
#define FLAT			            32

/* RESUME STATE INDICES */
#define RES_CTRL_REG1               0
#define RES_CTRL_REG2               1
#define RES_CTRL_REG3               2
#define RES_CTRL_REG4               3
#define RES_CTRL_REG5               4
#define RES_CTRL_REG6               5

#define	RESUME_ENTRIES		        6

#define	I2C_RETRY_DELAY		        5
#define	I2C_RETRIES		            5
#define	I2C_AUTO_INCREMENT	        0x80

enum {
	ACC_STANDBY = 0,
	ACC_ACTIVED = 1,
};

struct sc7a20_data {
	int hw_initialized;
	/* hw_working=-1 means not tested yet */
	int hw_working;

	int on_before_suspend;

	int poll_interval;
	int min_interval;
	int active;
	uint8_t sensitivity;
	uint8_t shift;
	uint8_t ctrl_reg1;
	uint8_t g_range;
	uint8_t bdu;
	uint8_t ble;
	uint8_t hr;
	uint8_t resume_state[RESUME_ENTRIES];
};

extern struct sc7a20_data sc7a20_misc_data;;

typedef struct {
	unsigned int cutoff_ms;
	unsigned int mask;
}sc7a20_odr_t;



void sc7a20_init(struct sc7a20_data *acc);
int sc7a20_get_acceleration_data(struct sc7a20_data *acc, int16_t *xyz);


#endif

