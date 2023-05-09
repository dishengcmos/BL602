#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include <blog.h>
#include <aos/yloop.h>
#include <aos/kernel.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>
#include <wifi_mgmr_ext.h>
#include <cli.h>
#include <hal_wifi.h>
#include <lwip/init.h>
#include <bl_gpio.h>
#include <hosal_i2c.h>
#include <bl_pwm.h>
#include <bl602.h>
#include <bl602_gpio.h>
#include <bl602_glb.h>
#include "tcp_example.h"
#include "led.h"
#include "main_board.h"
#include "vi5300.h"
#include "mpu42607.h"
#include "communication.h"
#include "networkdriven.h"


void iicTaskjudg(iicDevice_e iicdevice)
{
	if(iicdevice==VI5300)
		i2c_tof();
	else if(iicdevice==MPU42607)
		i2c_42607mpu();
}
void iicTask(void* para)
{
	while(1)
		iicTaskjudg(iicDevice);
	vTaskDelete(NULL);
}


void sensorInit()
{
	vTaskDelay(2000);
	led_gpio_init();

	tcpip_init(NULL, NULL);
	bl602_i2c_init();
 	vi5300Init();
//	icm42607Init();

//	pwminit();
 	ledstate.ledchange=1;
 	ledstate.ledcolour=blue;
}

void main()
{
	vTaskDelay(1000);
	sensorInit();

	/************/
  	xTaskCreate(networkproc, (char*)"main_entry", 1024, NULL, 15, NULL);
  	xTaskCreate(communprocess, (char*)"communcationprocess", 1024, NULL, 15, NULL);
  	xTaskCreate(iicTask, (char*)"iiicdrivetask", 1024, NULL, 15, NULL);
}




