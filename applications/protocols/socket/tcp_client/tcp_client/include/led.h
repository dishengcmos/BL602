#ifndef __LED_H__
#define __LED_H__
#include <stdint.h>
void ledbit0(void);
void ledbit1(void);
void ledred(void);
void ledgreen(void);
void ledblue(void);

enum{
	none,
	red,
	green,
	blue,
}ledcolor_e;

typedef struct{
	uint8_t ledcolour ;//0 red 1 green 2 blue
	uint8_t ledchange ;//1 led have changed flage
}ledstate_s;

extern ledstate_s ledstate;


void ledContarl(void);

void ledStatusContarl(void* pvParameters);

void led_gpio_init(void);




#endif /*__LED_H__*/
