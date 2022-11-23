#include "main.h"
#include "motor.h"
#include "common.h"

static void motor_set_status(uint8_t index, uint8_t status)
{
    GPIO_PinState s;
    if(status)  s = GPIO_PIN_SET;
    else        s = GPIO_PIN_RESET;

    switch(index){
        case MOTOR_A:{
            HAL_GPIO_WritePin(motorA_GPIO_Port, motorA_Pin, s);
            break;
        }

        case MOTOR_B:{
            HAL_GPIO_WritePin(motorB_GPIO_Port, motorB_Pin, s);
            break;
        }

        case MOTOR_C:{
            HAL_GPIO_WritePin(motorC_GPIO_Port, motorC_Pin, s);
            break;
        }

        case MOTOR_D:{
            HAL_GPIO_WritePin(motorD_GPIO_Port, motorD_Pin, s);
            break;
        }

        default:
            break;
    }
}

void motor_latency_ctrl(void)
{
    if(lock.motorTask.latency == 0 || lock.motorTask.direction == MOTOR_STATE_IDLE){
        lock.motorTask.latency = 0;
        return;        
    }
   
    lock.motorTask.latency --;

    if(lock.motorTask.latency == 0){
        switch(lock.motorTask.direction){
            case MOTOR_DIRECT_FORWARD:{
                if(lock.motorTask.direction >= MOTOR_STATE_4)   lock.motorTask.direction = MOTOR_STATE_1;
                else                                            lock.motorTask.direction ++;
                break;
            }

            case MOTOR_DIRECT_BACKWARD:{
                if(lock.motorTask.direction <= MOTOR_STATE_1)   lock.motorTask.direction = MOTOR_STATE_4;
                else                                            lock.motorTask.direction --;
                break;
            }
            
            default:
                break;
        }
    }
}

void motor_task(void)
{
    static uint8_t state = 0xff;

    switch(lock.motorTask.state){
        case MOTOR_STATE_1:{
            if(state != lock.motorTask.state){
                state = lock.motorTask.state;
                motor_set_status(MOTOR_A, MOTOR_H);
                motor_set_status(MOTOR_B, MOTOR_L);
                motor_set_status(MOTOR_C, MOTOR_L);
                motor_set_status(MOTOR_D, MOTOR_H);
            }
            break;
        }

        case MOTOR_STATE_2:{
            if(state != lock.motorTask.state){
                state = lock.motorTask.state;
                motor_set_status(MOTOR_A, MOTOR_L);
                motor_set_status(MOTOR_B, MOTOR_L);
                motor_set_status(MOTOR_C, MOTOR_H);
                motor_set_status(MOTOR_D, MOTOR_H);
            }
            break;
        }

        case MOTOR_STATE_3:{
            if(state != lock.motorTask.state){
                state = lock.motorTask.state;
                motor_set_status(MOTOR_A, MOTOR_L);
                motor_set_status(MOTOR_B, MOTOR_H);
                motor_set_status(MOTOR_C, MOTOR_H);
                motor_set_status(MOTOR_D, MOTOR_L);
            }
            break;
        }

        case MOTOR_STATE_4:{
            if(state != lock.motorTask.state){
                state = lock.motorTask.state;
                motor_set_status(MOTOR_A, MOTOR_H);
                motor_set_status(MOTOR_B, MOTOR_H);
                motor_set_status(MOTOR_C, MOTOR_L);
                motor_set_status(MOTOR_D, MOTOR_L);
            }
            break;
        }

        case MOTOR_STATE_IDLE:
        default:{
            if(state != lock.motorTask.state){
                state = lock.motorTask.state;
                motor_set_status(MOTOR_A, MOTOR_L);
                motor_set_status(MOTOR_B, MOTOR_L);
                motor_set_status(MOTOR_C, MOTOR_L);
                motor_set_status(MOTOR_D, MOTOR_L);
            }
            break;
        }
    }
}

void motor_set_forward(void)
{
    lock.motorTask.state = MOTOR_STATE_1;
    lock.motorTask.latency = MOTOR_LATENCY;
    lock.motorTask.direction = MOTOR_DIRECT_FORWARD;
}

void motor_set_backward(void)
{
    lock.motorTask.state = MOTOR_STATE_4;
    lock.motorTask.latency = MOTOR_LATENCY;
    lock.motorTask.direction = MOTOR_DIRECT_BACKWARD;
}

void motor_set_stop(void)
{
    lock.motorTask.state = MOTOR_STATE_IDLE;
    lock.motorTask.latency = 0;
}

