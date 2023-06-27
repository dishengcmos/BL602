

#include "led.h"
#include <blog.h>
#include <bl_gpio.h>
#include "bl602_mfg_media.h"
#include "usart_communication.h"
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

void ledloop(void)
{
	xTaskCreate(ledloopcreate, (char*)"ledloop", 1024, NULL, 15, NULL);
}

void ledempty(void)
{
	for(unsigned short i=0; i<2800;i++){
		__asm("nop");
	}
}
void ledRemoveImpurity(void)
{
//	ledbit0();
//	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit0();
	ledbit1();
	ledempty();
}
void ledContarl(void)
{
	__disable_irq();

	if(ledstate.ledchange==1)
	{
		//printf("ledStatusContarlchange %d \r\n",ledstate.ledchange);
		//printf("ledStatusContarllcolour %d \r\n",ledstate.ledcolour);

		ledstate.ledchange=0;
		if(ledstate.ledcolour==red)
		{
			ledRemoveImpurity();
			ledred();
			ledred();
			ledred();
			ledred();
			ledred();
			ledred();
			ledred();
			ledred();
			ledempty();
		}
		else if(ledstate.ledcolour==green)
		{
			ledRemoveImpurity();

			ledgreen();
			ledgreen();
			ledgreen();
			ledgreen();
			ledgreen();
			ledgreen();
			ledgreen();
			ledgreen();


			ledempty();
		}
		else if(ledstate.ledcolour==blue)
		{
			ledRemoveImpurity();
			ledblue();
			ledblue();
			ledblue();
			ledblue();
			ledblue();
			ledblue();
			ledblue();
			ledblue();

			ledempty();
		}
		//vTaskDelay(1000);//这个地方正式版要记得删除
	}
	__enable_irq();
}

void leddisplay(void)
{
	__disable_irq();
	ledRemoveImpurity();
	for(uint8_t j=0;j<=7;j++)
	{
		/*********green*********/
		for (uint8_t i=0;i<=7;i++)
		{
			if ((led_function.color[j].green & (0x80>>i))>=1)
			{
				ledbit1();
			}
			else
			{
				ledbit0();
			}
		}
		/*********red*********/
		for (uint8_t i=0;i<=7;i++)
		{
			if ((led_function.color[j].red & (0x80>>i))>=1)
			{
				ledbit1();
			}
			else
			{
				ledbit0();
			}
		}
		/*********red*********/
		for (uint8_t i=0;i<=7;i++)
		{
			if ((led_function.color[j].blue & (0x80>>i))>=1)
			{
				ledbit1();
			}
			else
			{
				ledbit0();
			}
		}
	}
	ledempty();
	__enable_irq();
}

void ledloopcreate(void* para)
{
	while(1)
	{
		leddisplay();
		//usart_communicat_send(data_ledred, 1);
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

	ledbit1();
	ledbit1();
	ledbit1();
	ledbit1();
	ledbit1();
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
	ledbit1();
	ledbit1();
	ledbit1();
	ledbit1();
	ledbit1();
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

void gpioledh(void )
{
	//GPIO_LED_PIN_12_H;
    bl_gpio_output_set(GPIO_LED_PIN, 1);
}

void gpioledl(void )
{
	GPIO_LED_PIN_12_L;
}


void ledrst(void)
{
	GPIO_LED_PIN_12_H
	for(unsigned short i=0; i<1000;i++)
	{
		__asm("nop");
	}
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
