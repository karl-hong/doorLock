#include "sc7a20.h"
#include "i2c.h"
#include <stdio.h>
#include "common.h"

sc7a20_odr_t sc7a20_odr_table[] = {
	{1,    ODR1250},
	{3,    ODR400 }, 
	{5,    ODR200 }, 
	{10,   ODR100 }, 
	{20,   ODR50  }, 
	{40,   ODR25  }, 
	{100,  ODR10  }, 
	{1000, ODR1   },
};

struct sc7a20_data sc7a20_misc_data;
static int16_t xyzBuf[3];

static int sc7a20_i2c_write(uint16_t reg_address, uint8_t *buf, uint16_t len)
{
	int err;
	int tries = 0;
	uint8_t devAddr = (SC7A20_ACC_I2C_ADDR << 1) | 0;

	do {
		err =	i2c_transmit(devAddr, reg_address, buf, len, I2C_RETRY_DELAY);
	} while ((err != HAL_OK) && (++tries < I2C_RETRIES));

	if (err != HAL_OK) {
		err = -1;
	} else {
		err = 0;
	}
	return err;
}

static int sc7a20_register_write(uint16_t reg_address, uint8_t new_value)
{
	return sc7a20_i2c_write(reg_address, &new_value, 1);
}

static int sc7a20_i2c_read(uint16_t reg_address, uint8_t * buf, uint16_t len)
{
	int err;
	int tries = 0;
	
	uint8_t devAddr = (SC7A20_ACC_I2C_ADDR << 1) | 1;

	do {
		err = i2c_receive(devAddr, reg_address, buf, len, I2C_RETRY_DELAY);
		//printf("dev addr: %d, reg: %d, data: %d, err: %d\r\n", SC7A20_ACC_I2C_ADDR, reg_address, *buf, err);
	} while ((err != HAL_OK) && (++tries < I2C_RETRIES));

	if (err != HAL_OK) {
		err = -1;
	} else {
		err = 0;
	}

	return err;
}

static int sc7a20_register_read(uint16_t reg_address, uint8_t *buf)
{

	int err;
	err = sc7a20_i2c_read(reg_address, buf, 1);
	return err;
}

static int sc7a20_update_g_range(struct sc7a20_data *acc, uint8_t new_g_range)
{
	int err;

	uint8_t sensitivity;
	uint8_t updated_val;
	uint8_t init_val;
	uint8_t new_val;
	uint8_t mask = SC7A20_ACC_FS_MASK;

	switch (new_g_range) {
		case SC7A20_ACC_G_2G:
			sensitivity = 4;
			break;
		case SC7A20_ACC_G_4G:

			sensitivity = 3;
			break;
		case SC7A20_ACC_G_8G:

			sensitivity = 2;
			break;
		case SC7A20_ACC_G_16G:

			sensitivity = 1;
			break;		
		default:
			printf("invalid g range requested: %u\r\n", new_g_range);
			return -1;
	}

	err = sc7a20_register_read(CTRL_REG4, &init_val);
	if (err < 0) {
		printf("%s, error read register CTRL_REG4\r\n", __func__);
		return -1;
	}	

	new_val = new_g_range;
	updated_val = ((mask & new_val) | ((~mask) & init_val));

	err = sc7a20_register_write(CTRL_REG4, updated_val);
	if (err < 0)
		printf("soft power off failed\r\n");

	acc->resume_state[RES_CTRL_REG4] = updated_val;	   
	acc->sensitivity = sensitivity;

	printf("%s sensitivity %d g-range %d\r\n", __func__, sensitivity,new_g_range);

	return 0;
}

static int sc7a20_update_bdu(struct sc7a20_data *acc, uint8_t new_bdu)
{
	int err = -1;
	uint8_t buf[2];
	uint8_t updated_val;
	uint8_t init_val;
	uint8_t new_val;
	uint8_t mask = SC7A20_ACC_BDU_MASK;


	err = sc7a20_register_read(CTRL_REG4, &init_val);
	if (err < 0) {
		printf("%s, error read register CTRL_REG4\r\n", __func__);
		return -1;
	}	

	new_val = new_bdu;	
	updated_val = ((mask & new_val) | ((~mask) & init_val));

	err = sc7a20_register_write(CTRL_REG4, updated_val);
	if (err < 0)
		goto error;
	acc->resume_state[RES_CTRL_REG4] = updated_val;

	return 0;

error:
	printf("update bdu failed 0x%x,0x%x: %d\r\n",	CTRL_REG4, updated_val, err);
	return err;	
}

static int sc7a20_update_ble(struct sc7a20_data *acc, uint8_t new_ble)
{
	int err = -1;
	uint8_t buf[2];
	uint8_t updated_val;
	uint8_t init_val;
	uint8_t new_val;
	uint8_t mask = SC7A20_ACC_BLE_MASK;

	err = sc7a20_register_read(CTRL_REG4, &init_val);
	if (err < 0) {
		printf("%s, error read register CTRL_REG4\r\n", __func__);
		return -1;
	}
	
	new_val = new_ble;	
	updated_val = ((mask & new_val) | ((~mask) & init_val));
	err = sc7a20_register_write(CTRL_REG4, updated_val);

	if (err < 0)
		goto error;
	acc->resume_state[RES_CTRL_REG4] = updated_val;

	return 0;

error:
	printf("update ble failed 0x%x,0x%x: %d\r\n",	CTRL_REG4, updated_val, err);
	return err;	
}

static int sc7a20_update_hr(struct sc7a20_data *acc, uint8_t new_hr)
{
	int err = -1;
	uint8_t buf[2];
	uint8_t updated_val;
	uint8_t init_val;
	uint8_t new_val;
	uint8_t mask = SC7A20_ACC_HR_MASK;

	err = sc7a20_register_read(CTRL_REG4, &init_val);
	if (err < 0) {
		printf("%s, error read register CTRL_REG4\r\n", __func__);
		return -1;
	}

	new_val = new_hr;	
	updated_val = ((mask & new_val) | ((~mask) & init_val));

	err = sc7a20_register_write(CTRL_REG4, updated_val);
	if (err < 0)
		goto error;
	
	acc->resume_state[RES_CTRL_REG4] = updated_val;

	return 0;

error:
	printf("update hr failed  0x%x,0x%x: %d\r\n",	CTRL_REG4, updated_val, err);
	return err;	
}


static int sc7a20_update_odr(struct sc7a20_data *acc, int poll_interval_ms)
{
	int err = -1;
	int i;
	uint8_t buf[2];
	uint8_t new_value;
	uint16_t table_size =  sizeof(sc7a20_odr_table) / sizeof(sc7a20_odr_t);

	for (i = table_size - 1; i >= 0; i--) {
		if (sc7a20_odr_table[i].cutoff_ms <= poll_interval_ms)
			break;
	}

	new_value = sc7a20_odr_table[i].mask;

	new_value |= SC7A20_ENABLE_ALL_AXES;

	err = sc7a20_register_write(CTRL_REG1, new_value);
	if (err < 0)
		return err;

	acc->resume_state[RES_CTRL_REG1] = new_value;
	acc->ctrl_reg1 = new_value;

	return 0;
}

static void sc7a20_set_interrupt(void)
{
	uint8_t interrupt_thresold = 0;
	
	interrupt_thresold = 0x50;//0x40;//0x60;//0x50;
	
	/* High Interrupt Triggler */
	sc7a20_register_write(CTRL_REG6, 0x00);
	/* Interrup thresold */
	sc7a20_register_write(CLICK_THS, interrupt_thresold);
	/* Time limit */
	sc7a20_register_write(TIME_LIMIT, 0x7f);
	/* Time latency */
	sc7a20_register_write(TIME_LATENCY, 0x05);
	/* x/y/z Interrupt Detect Enable */
	sc7a20_register_write(CLICK_CFG, 0x15);
	/* enable x/y/z direction interrupt, low event */
	sc7a20_register_write(INT1_CFG, 0x15); 
	/* Enable Interrupt */
	sc7a20_register_write(CTRL_REG3, 0x80);
}

void sc7a20_init(struct sc7a20_data *acc)
{
	uint8_t chipId = 0;
	uint8_t rc;
	
	rc = sc7a20_register_read(WHO_AM_I, &chipId);
	if(0 != rc){
		printf("Err: sc7a20 read chip id fail!\r\n");
		return;
	}
	
	if(SC7A20_CHIP_ID != chipId){
		printf("sc7a20 wrong chip id!\r\n");
		return;
	}
	
	acc->resume_state[RES_CTRL_REG1] = SC7A20_ENABLE_ALL_AXES;
	acc->resume_state[RES_CTRL_REG2] = 0x00;
	acc->resume_state[RES_CTRL_REG3] = 0x00;
	acc->resume_state[RES_CTRL_REG4] = 0x00;
	acc->resume_state[RES_CTRL_REG5] = 0x00;
	acc->resume_state[RES_CTRL_REG6] = 0x00;
	
	acc->g_range     = SC7A20_ACC_G_2G;
	acc->sensitivity = SENSITIVITY_2G;
	/* set g range */
	rc = sc7a20_update_g_range(acc, acc->g_range);
	if(0 > rc){
		printf("update_g_range failed\r\n");
		goto error1;
	}
	
	/* set bdu */
	acc->bdu = 0x80;
	rc = sc7a20_update_bdu(acc, acc->bdu);
	if(0 > rc){
		printf("update_bdu failed\r\n");
		goto error1;
	}
	
	/* set ble */
	acc->ble = 0x00;
	rc = sc7a20_update_ble(acc, acc->ble);
	if(0 > rc){
		printf("update_ble failed\r\n");
		goto error1;
	}
	
	/* set hr */
	acc->hr = 0x08;
	rc = sc7a20_update_hr(acc, acc->hr);
	if(0 > rc){
		printf("update_hr failed\r\n");
		goto error1;
	}
	
	/* set odr */
	acc->min_interval  = 20;
	acc->poll_interval = 20;
	rc = sc7a20_update_odr(acc, acc->poll_interval);
	if(0 > rc){
		printf("update_odr failed\r\n");
		goto error1;
	}
	
	sc7a20_set_interrupt();
	
	acc->hw_initialized = 1;
	printf("sc7a20 hw init done!\r\n");
	
	return;
error1:
	acc->hw_initialized = 0;
	printf("sc7a20 init failed\r\n");
	return;
}

int sc7a20_get_acceleration_data(struct sc7a20_data *acc, int16_t *xyz)
{
	static uint8_t state = 0;
	static uint8_t errCnt = 0;
	int err = -1;
	/* Data bytes from hardware xL, xH, yL, yH, zL, zH */
	uint8_t  buf[2];
	static uint8_t  acc_data[6];
	int16_t hw_d[3] = { 0 };
	
	if(errCnt >= 5){
		goto error;
	}
	
	switch(state){
		case 0:{
			/* check status reg */
			err = sc7a20_register_read(STATUS_REG, buf);
			if(0 > err){
				printf("state %d: read data failed!\r\n", state);
				errCnt ++;
				return err;
			}
			
			if((buf[0] & 0x0f) != 0x0f){
				xyz[0] = xyzBuf[0];
				xyz[1] = xyzBuf[1];
				xyz[2] = xyzBuf[2];
				printf("state %d: data not ready!\r\n", state);
				return err;
			}
			/* goto next state */
			state ++;
			errCnt = 0;
			
			return err;
		}
		
		case 1:{
			/* read XOUT_L reg */
			err = sc7a20_register_read(XOUT_L, buf);
			
			if(0 > err){
				printf("state %d: read data failed!\r\n", state);
				errCnt ++;
				return err;
			}
			/* get data */
			acc_data[0] = buf[0];
			
			/* goto next state */
			state ++;
			errCnt = 0;
			
			return err;
		}
		
		case 2:{
			/* read XOUT_H reg */
			err = sc7a20_register_read(XOUT_H, buf);
			
			if(0 > err){
				printf("state %d: read data failed!\r\n", state);
				errCnt ++;
				return err;
			}
			
			/* get data */
			acc_data[1] = buf[0];	
			
			/* goto next state */
			state ++;
			errCnt = 0;
			
			return err;
		}
		
		case 3:{
			/* read YOUT_L reg */
			err = sc7a20_register_read(YOUT_L, buf);
			
			if(0 > err){
				printf("state %d: read data failed!\r\n", state);
				errCnt ++;
				return err;
			}
			
			/* get data */
			acc_data[2] = buf[0];
			
			/* goto next state */
			state ++;
			errCnt = 0;
			
			return err;
		}
		
		case 4:{
			/* read YOUT_H reg */
			err = sc7a20_register_read(YOUT_H, buf);
			
			if(0 > err){
				printf("state %d: read data failed!\r\n", state);
				errCnt ++;
				return err;
			}
			
			/* get data */
			acc_data[3] = buf[0];
			
			/* goto next state */
			state ++;
			errCnt = 0;
			
			return err;			
		}
		
		case 5:{
			/* read ZOUT_L reg */
			err = sc7a20_register_read(ZOUT_L, buf);
			
			if(0 > err){
				printf("state %d: read data failed!\r\n", state);
				errCnt ++;
				return err;
			}
			
			/* get data */
			acc_data[4] = buf[0];
			
			/* goto next state */
			state ++;
			errCnt = 0;
			
			return err;	
		}
		
		case 6:{
			/* read ZOUT_H  reg */
			err = sc7a20_register_read(ZOUT_H, buf);
			
			if(0 > err){
				printf("state %d: read data failed!\r\n", state);
				errCnt ++;
				return err;
			}
			
			/* get data */
			acc_data[5] = buf[0];
			
			/* goto next state */
			state = 0;
			errCnt = 0;
			
			break;
		}
		
		default:{
			state = 0;
			errCnt = 0;
			return 0;
		}
	}

	/* get all data */
	hw_d[0] =  ((acc_data[1] << 8) & 0xff00) | acc_data[0];
	hw_d[1] =  ((acc_data[3] << 8) & 0xff00) | acc_data[2];
	hw_d[2] =  ((acc_data[5] << 8) & 0xff00) | acc_data[4];

	hw_d[0] = ((int16_t) hw_d[0] ) >> acc->sensitivity;
	hw_d[1] = ((int16_t) hw_d[1] ) >> acc->sensitivity;
	hw_d[2] = ((int16_t) hw_d[2] ) >> acc->sensitivity;

	xyz[0] = hw_d[0];
	xyz[1] = hw_d[1];
	xyz[2] = hw_d[2];

	xyzBuf[0] = xyz[0];
	xyzBuf[1] = xyz[1];
	xyzBuf[2] = xyz[2];

	return err;
	
error:
	errCnt = 0;
	state = 0;
	sc7a20_init(&sc7a20_misc_data);
	return -1;
}

void sc7a20_interrupt_handle(void)
{
	Sensor.gSensorState = 1;
	//printf("sc7a20!!!\r\n");
}













