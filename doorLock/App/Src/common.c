#include "main.h"
#include "common.h"

void gpio_interrupt_callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin){
        case doorDetect2_Pin:{
            lock.doorDetectState2 = HAL_GPIO_ReadPin(doorDetect2_GPIO_Port, doorDetect2_Pin);
            break;
        }

        case doorDetect1_Pin:{
            lock.doorDetectState1 = HAL_GPIO_ReadPin(doorDetect1_GPIO_Port, doorDetect1_Pin);
            break;  
        }

        case lock_Detect1_Pin:{
            lock.lockDetectState1 = HAL_GPIO_ReadPin(lock_Detect1_GPIO_Port, lock_Detect1_Pin);
            break;
        }

        case lock_Detect2_Pin:{
            lock.lockDetectState2 = HAL_GPIO_ReadPin(lock_Detect2_GPIO_Port, lock_Detect2_Pin);
            break;
        }

        case KEY_Detect_Pin:{
            lock.keyDetectState = HAL_GPIO_ReadPin(KEY_Detect_GPIO_Port, KEY_Detect_Pin);
            break;
        }

        default:
            break;
    }

}

void tim_interrupt_callback(void)
{
    
}