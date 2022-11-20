#include "main.h"
#include "led.h"


void led_set_status(uint8_t led, uint8_t status)
{
    uint8_t s;
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

void light_set_status(uint8_t light, uint8_t status)
{
    uint8_t s;
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

void led_task(void)
{

}

void light_task(void)
{

}