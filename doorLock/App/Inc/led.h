#ifndef __LED_H__
#define __LED_H__

enum {
    LED_ON = 0,
    LED_OFF,
};

enum {
    LED_RED = 0,
    LED_GREEN,
};

enum{
    LIGHT_OFF = 0,
    LIGHT_ON,
};

enum {
    LIGHT_INDEX1 = 0,
    LIGHT_INDEX2,
};

void set_light1_on(void);
void set_light1_off(void);
void set_light2_on(void);
void set_light2_off(void);
void led_task(void)；

#endif
