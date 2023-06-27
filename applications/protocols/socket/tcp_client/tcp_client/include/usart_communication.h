#ifndef __USRAT_COMMUNCATION__
#define __USRAT_COMMUNCATION__

#include <stdint.h>



typedef struct {

	struct{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	}color[8];

	uint8_t mark1;
	uint8_t mark2;

}led_function_s;

extern led_function_s led_function;
/***
typedef struct{
	uint8_t distance
	uint8_t
}
***/

void usart_log_communicat_send(const void *data, uint32_t size);

int usart_communicat_receive(void *data, uint32_t size);


void usartprogress(void* para);

void TaskUart(void *param);

void usart_echo_communicat_send(const void *data, uint32_t size);






#endif /*__USRAT_COMMUNCATION__*/
