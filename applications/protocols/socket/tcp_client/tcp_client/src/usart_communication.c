

#include "usart_communication.h"
#include <hosal_uart.h>
#include <string.h>
#include <blog.h>

#define ID_NUMBER 0x01

led_function_s led_function;

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

uint8_t logdata[8]={0xaa,0x11,0x22,0x33,0x44,0x55,0x66,0x77};
void TaskUart(void *param)
{
    uint8_t data[11];
    int ret;

    hosal_uart_init(&uart_dev_log);
    hosal_uart_init(&uart_dev_echo);
    //blog_info("Uart Demo Start");
    static uint8_t data_sta,len,id_function;

    while (1)
    {
        ret = hosal_uart_receive(&uart_dev_echo, data, sizeof(data));
        //if (ret > 0)
        //{
        	/*****数据包解析****/
            switch (data_sta)
            {
            	case 0:
            	{
            		if (data[0] == 0xfd)
            		{
            			data_sta = 1;
            			usart_log_communicat_send(&data[0], 1);
            		}
            		break;
            	}
            	case 1:
            	{
            		if (data[1] == 0xfd)
            		{
            			data_sta = 2;
            			usart_log_communicat_send(&data[1], 1);
            		}
            		else
            		{
            			data_sta = 0;
            			usart_log_communicat_send(&logdata[0], 1);
            		}
            		break;
            	}
            	case 2:
            		if(ID_NUMBER== data[2])
            		{
            			data_sta=3;
            			usart_log_communicat_send(&data[2], 1);
            		}
            		else
            		{
            			data_sta=0;
            			usart_log_communicat_send(&logdata[0], 1);
            		}
            		break;
            	case 3:
            		len=data[3];
            		if( (len>=0) && (len<=20))
            		{
            			data_sta=4;
            			usart_log_communicat_send(&data[3], 1);
            		}
            		else
            		{
            			data_sta=0;
            			usart_log_communicat_send(&logdata[0], 1);
            		}
            		break;
            	case 4:
            		id_function = data[4];
            		if (!((id_function >= 1 && id_function <= 20) || id_function == 233)) //任何模式都不是
            		{
            	        data_sta = 0;
            			usart_log_communicat_send(&logdata[0], 1);
            		}
            		else
            		{
            			data_sta=5;
                		usart_log_communicat_send(&data[4], 1);
            		}
            		break;
            	case 5:
            	{
                    uint8_t data_sum = 0;
                    data_sta=0;
                    for(char i=0;i<=len+1;i++)//从第3个数据到最后一个, 因为从0开始计数,所以累加的数是len+2个
                    {
                    	data_sum+=data[2+i];
            			usart_log_communicat_send(&logdata[7], 1);
            			usart_log_communicat_send(&logdata[7], 1);
            			usart_log_communicat_send(&logdata[7], 1);
            			usart_log_communicat_send(&logdata[7], 1);
            			usart_log_communicat_send(&data_sum, 1);
                    }
                    if(data_sum==data[len+4])
                    {
                    	if(id_function==1)//led功能
                    	{
                    		uint8_t data_tmp=data[8];
                    		usart_log_communicat_send(&data_tmp, 1);
                    		for(uint8_t i=0;i<8;i++)
                    		{
								if((data_tmp&(0x01<<i))>=1)
								{
									led_function.color[i].red=data[5];
									led_function.color[i].green=data[6];
									led_function.color[i].blue=data[7];

			                		usart_log_communicat_send(&data[5], 1);
			                		usart_log_communicat_send(&data[6], 1);
			                		usart_log_communicat_send(&data[7], 1);
									//usart_log_communicat_send(&data, 11);
								}
                    		}
							led_function.mark1=data[8];
							led_function.mark2=data[9];
                    	}
                    	else if(id_function==2)//其他功能
                    	{
                			usart_log_communicat_send(&logdata[0], 1);

                    	}
                    }
            		break;
            	}

            	default:
            		data_sta=0;
            		break;
            }
        	//data_ledred[0]=data[0];
            //hosal_uart_send(&uart_dev_log, data, ret);
            //hosal_uart_send(&uart_dev_log, data1, 4);
            //hosal_uart_send(&uart_dev_echo, data_ledred, 5);
            //hosal_uart_send(&uart_dev_echo, data, 10);
            //blog_info("Get data ");

            /*****
        	data_ledred[0]=data[0];
            //hosal_uart_send(&uart_dev_log, data, ret);
            hosal_uart_send(&uart_dev_log, data1, 4);
            hosal_uart_send(&uart_dev_echo, data_ledred, 5);
            //blog_info("Get data ");
            *****/
        //}
    }
}

void usart_log_communicat_send(const void *data, uint32_t size)
{
    hosal_uart_send(&uart_dev_log, data, size);
}

void usart_echo_communicat_send(const void *data, uint32_t size)
{
    hosal_uart_send(&uart_dev_echo, data, size);
}

int usart_communicat_receive(void *data, uint32_t size)
{
	int ret;
    ret=hosal_uart_receive(&uart_dev_log, data, size);
	return ret;
}

void usartprogress(void* para)
{
    int ret;
	hosal_uart_init(&uart_dev_log);
	hosal_uart_init(&uart_dev_echo);

	uint8_t sendData[10] = "transegin";	

	uint8_t sendDataing[10] = {0x55,0xaa,0x55,0xaa,0x55,0xaa};
	//uint8_t databuff[20];
	//uint8_t usart_senddata[10];
	//memcpy(databuff, sendData, 10);
	//usart_communicat_send(sendData, 10);
	
    while (1)
    {
        /* Uart receive poll */
//        ret = usart_communicat_receive(sendData, sizeof(sendData));
        //usart_communicat_send(sendDataing, 10);
        //if (ret > 0)
        //{
            /* Uart send poll */
        	//usart_communicat_send(sendData, ret);
//        	usart_echo_communicat_send(sendDataing, 5);
//        	usart_log_communicat_send(sendDataing, 5);
            //blog_info("Get data ");
        //}
    }
    vTaskDelete(NULL);
}

