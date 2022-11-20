#ifndef __MOTOR_H__
#define __MOTOR_H__

enum {
    MOTOR_A = 0,
    MOTOR_B,
    MOTOR_C,
    MOTOR_D,
};

enum {
    MOTOR_L = 0,
    MOTOR_H,
};

enum {
    MOTOR_DIRECT_FORWARD = 0,
    MOTOR_DIRECT_BACKWARD,
};

enum {
    MOTOR_STATE_IDLE = 0,
    MOTOR_STATE_1,
    MOTOR_STATE_2,
    MOTOR_STATE_3,
    MOTOR_STATE_4,
};

void motor_latency_ctrl(void);
void motor_task(void);
void motor_set_forward(void);
void motor_set_backward(void);
void motor_set_stop(void);

#endif
