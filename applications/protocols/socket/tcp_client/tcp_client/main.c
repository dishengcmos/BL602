

/****
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <blog.h>
#include "bl_sys.h"
#include <stdio.h>
#include <cli.h>
#include <hosal_uart.h>
#include <blog.h>

void TaskUart(void *param)
{

    uint8_t data[32];

    uint8_t data1[32]={0x55,0x55,0x55,0x55};
    int ret;

    hosal_uart_dev_t uart_dev_echo = {
        .config = {
            .uart_id = 0,
            .tx_pin = 4, // TXD GPIO
            .rx_pin = 3, // RXD GPIO
            .cts_pin = 255,
            .rts_pin = 255,
            .baud_rate = 115200,
            .data_width = HOSAL_DATA_WIDTH_8BIT,
            .parity = HOSAL_NO_PARITY,
            .stop_bits = HOSAL_STOP_BITS_1,
            .mode = HOSAL_UART_MODE_POLL,
        },
    };

    hosal_uart_dev_t uart_dev_log = {
        .config = {
            .uart_id = 1,
            .tx_pin = 16, // TXD GPIO
            .rx_pin = 7,  // RXD GPIO
            .cts_pin = 255,
            .rts_pin = 255,
            .baud_rate = 115200,
            .data_width = HOSAL_DATA_WIDTH_8BIT,
            .parity = HOSAL_NO_PARITY,
            .stop_bits = HOSAL_STOP_BITS_1,
            .mode = HOSAL_UART_MODE_POLL,
        },
    };


    hosal_uart_init(&uart_dev_log);
    hosal_uart_init(&uart_dev_echo);
    blog_info("Uart Demo Start");
    while (1)
    {
        ret = hosal_uart_receive(&uart_dev_echo, data, sizeof(data));
        if (ret > 0)
        {
            //hosal_uart_send(&uart_dev_log, data, ret);
            hosal_uart_send(&uart_dev_log, data1, 5);
            hosal_uart_send(&uart_dev_echo, data, 5);
            //blog_info("Get data ");
        }
    }
}
void main(void)
{

    xTaskCreate(TaskUart, "TaskUart", 1024, NULL, 15, NULL);
}
***/
/***/

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
#include "pwmdriven.h"
#include "usart_communication.h"
#include <hosal_uart.h>



void iicTaskjudg(iicDevice_e iicdevice)
{
	if(iicdevice==VI5300)
	{
		i2c_tof();
	}
	else if(iicdevice==MPU42607)
	{
		i2c_42607mpu();
	}
}
void iicTask(void* para)//
{

	while(1)
	{	 
		//iicDevice=MPU42607;
		iicTaskjudg(iicDevice);
	}
	vTaskDelete(NULL);
}

void test()//void* para
{
	while(1)
	{
		iicTaskjudg(iicDevice);
	}
}

void sensorInit()
{
	led_gpio_init();
 	tcpip_init(NULL, NULL);
 	bl602_i2c_init();

	if(iicDevice==VI5300)
	{
		vi5300Init();
	}
	else if(iicDevice==MPU42607)
	{
		icm42607Init();
	}
	pwminit();
 	ledstate.ledchange=1;
 	ledstate.ledcolour=blue;
}

void main()
{
	vTaskDelay(2000);
 	sensorInit();
	
  	xTaskCreate(iicTask, (char*)"iiicdrivetask", 1024, NULL, 15, NULL);
  	xTaskCreate(networkproc, (char*)"main_entry", 1024, NULL, 15, NULL);
 	xTaskCreate(communprocess, (char*)"communcationprocess", 1024, NULL, 15, NULL);
  	//xTaskCreate(usartprogress, (char*)"usarttrans", 1024, NULL, 15, NULL);

  	xTaskCreate(TaskUart,(char*)"TaskUart", 1024, NULL, 15, NULL);
 	xTaskCreate(ledloopcreate, (char*)"ledloop", 1024, NULL, 15, NULL);
}

