

#include "led.h"
#include <blog.h>
#include <bl_gpio.h>
static uint32_t *pOut=(uint32_t *)(0x40000188);
static uint32_t tmpOut= 0x1000;
#define GPIO_LED_PIN_12_H *pOut |= tmpOut;
#define GPIO_LED_PIN_12_L *pOut &=(~tmpOut);
#define GPIO_LED_PIN 12
ledstate_s ledstate;
void led_gpio_init(void)
{
	bl_gpio_enable_output(GPIO_LED_PIN, 1, 0);
    bl_gpio_output_set(GPIO_LED_PIN, 0);
}
void ledContarl(void)
{
		//vTaskDelay(100);
		if(ledstate.ledchange==1)
		{
			 	 printf("ledStatusContarlchange %d \r\n",ledstate.ledchange);
			 	 printf("ledStatusContarllcolour %d \r\n",ledstate.ledcolour);
			vTaskDelay(300);
 		 	ledstate.ledchange=0;
  		 	//printf("start led\r\n");

			if(ledstate.ledcolour==red)
			{
	  		 	printf("red\r\n");
				ledbit1();
				ledbit0();	//不知道为啥，输出的第一个波形是错误的，所以先让他把错误波形输出
				vTaskDelay(1);
				ledred();
				ledred();
				ledred();
				ledred();
				ledred();
				ledred();
				ledred();
				ledred();
				vTaskDelay(1);
			}
			else if(ledstate.ledcolour==green)
			{
	  		 	printf("green\r\n");
				ledbit1();
				ledbit0();	//不知道为啥，输出的第一个波形是错误的，所以先让他把错误波形输出去

				vTaskDelay(1);
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				vTaskDelay(1);
			}
			else if(ledstate.ledcolour==blue)
			{
	  		 	printf("blue\r\n");
				ledbit1();
				ledbit0();	//不知道为啥，输出的第一个波形是错误的，所以先让他把错误波形输出去

				vTaskDelay(1);
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				vTaskDelay(1);
			}
		}

}

void ledStatusContarl(void* pvParameters)
{
	while (1)
	{
		//vTaskDelay(100);
		if(ledstate.ledchange==1)
		{
			 	 printf("ledStatusContarlchange %d \r\n",ledstate.ledchange);
			 	 printf("ledStatusContarllcolour %d \r\n",ledstate.ledcolour);
			vTaskDelay(300);
 		 	ledstate.ledchange=0;
  		 	//printf("start led\r\n");

			if(ledstate.ledcolour==red)
			{
	  		 	printf("red\r\n");
				ledbit1();
				ledbit0();	//不知道为啥，输出的第一个波形是错误的，所以先让他把错误波形输出
				vTaskDelay(1);
				ledred();
				ledred();
				ledred();
				ledred();
				ledred();
				ledred();
				ledred();
				ledred();
				vTaskDelay(1);
			}
			else if(ledstate.ledcolour==green)
			{
	  		 	printf("green\r\n");
				ledbit1();
				ledbit0();	//不知道为啥，输出的第一个波形是错误的，所以先让他把错误波形输出去

				vTaskDelay(1);
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				ledgreen();
				vTaskDelay(1);
			}
			else if(ledstate.ledcolour==blue)
			{
	  		 	printf("blue\r\n");
				ledbit1();
				ledbit0();	//不知道为啥，输出的第一个波形是错误的，所以先让他把错误波形输出去

				vTaskDelay(1);
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				ledblue();
				vTaskDelay(1);
			}
		}
	}
    vTaskDelete(NULL);
}
void ledred(void)
{
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();

	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit1();
	ledbit1();
	ledbit1();

	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
}
void ledgreen(void)
{
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit1();
	ledbit1();
	ledbit1();

	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();

	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
}
void ledblue(void)
{
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();

	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();

	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit1();
	ledbit1();
	ledbit1();
}







void ledbit0(void)
{
	//bl_gpio_output_set(GPIO_LED_PIN, 1);
	GPIO_LED_PIN_12_H;
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	GPIO_LED_PIN_12_L;

	//bl_gpio_output_set(GPIO_LED_PIN, 0);
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");


	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");


	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
}

void ledbit1(void)
{
	GPIO_LED_PIN_12_H;
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");


	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	GPIO_LED_PIN_12_L;

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");


	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");


	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
}
