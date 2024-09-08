#include "main.h"
#include "led.h"
#include "common.h"


static void led_set_status(uint8_t led, uint8_t status)
{
    GPIO_PinState s;
    if(status)  s = GPIO_PIN_SET;
    else        s = GPIO_PIN_RESET;
    switch(led){
        case LED_RED:{
            HAL_GPIO_WritePin(rLed_GPIO_Port, rLed_Pin, s);
            break;
        }

        case LED_GREEN:{
            HAL_GPIO_WritePin(gLed_GPIO_Port, gLed_Pin, s);
            break;
        }

        default:
            break;
    } 
}

static void light_set_status(uint8_t light, uint8_t status)
{
    GPIO_PinState s;
    if(status)  s = GPIO_PIN_SET;
    else        s = GPIO_PIN_RESET;

    switch(light){
        case LIGHT_INDEX1:{
            HAL_GPIO_WritePin(light1_GPIO_Port, light1_Pin, s);
            break;
        }

        case LIGHT_INDEX2:{
            HAL_GPIO_WritePin(light2_GPIO_Port, light2_Pin, s);
            break;
        }

        default:
            break;
    }

}

void set_light1_on(void)
{
    lock.lightState1 = LIGHT_ON;
    light_set_status(LIGHT_INDEX1, LIGHT_ON);
}

void set_light1_off(void)
{
    lock.lightState1 = LIGHT_OFF;
    light_set_status(LIGHT_INDEX1, LIGHT_OFF);
}

void set_light2_on(void)
{
    lock.lightState2 = LIGHT_ON;
    light_set_status(LIGHT_INDEX2, LIGHT_ON);
}

void set_light2_off(void)
{
    lock.lightState2 = LIGHT_OFF;
    light_set_status(LIGHT_INDEX2, LIGHT_OFF);
}


void led_task(void)
{
	switch(lock.ledTask.state){
		case LED_TASK_STATE_FLASH:{
			if(lock.ledFlashStatus){
				if(lock.lockState){
					if(lock.ledTask.flashOn)	led_set_status(LED_GREEN, LED_ON);
					else						led_set_status(LED_GREEN, LED_OFF);
					led_set_status(LED_RED, LED_OFF);
				}else{
					led_set_status(LED_GREEN, LED_OFF);
					if(lock.ledTask.flashOn)	led_set_status(LED_RED, LED_ON);
					else						led_set_status(LED_RED, LED_OFF);
				}
			}else{
				if(lock.lockState){
					led_set_status(LED_GREEN, LED_ON);
					led_set_status(LED_RED, LED_OFF);
				}else{
					led_set_status(LED_GREEN, LED_OFF);
					led_set_status(LED_RED, LED_ON);
				}
			}
			break;
		}

		case LED_TASK_STATE_IDLE:{
			if(lock.lockState){
				led_set_status(LED_GREEN, LED_ON);
				led_set_status(LED_RED, LED_OFF);
			}else{
				led_set_status(LED_GREEN, LED_OFF);
				led_set_status(LED_RED, LED_ON);
			}
			break;
		}

		default:{
			break;
		}
	}

    if(lock.lockState){//lock
        set_light1_off();
        set_light2_off();
    }else{//unlock
        set_light1_on();
        set_light2_on();
    }
}




