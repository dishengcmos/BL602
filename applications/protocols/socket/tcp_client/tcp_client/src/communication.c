
#include "communication.h"
#include <string.h>
 #include "vi5300.h"
 #include "mpu42607.h"
#include <blog.h>
#include "networkdriven.h"

#include <wifi_mgmr_ext.h>

#include "led.h"

static char temp_account_pass_buff[50];

iicDevice_e iicDevice;

receiveFlag_s receiveFlag;

enum{
	//前2为连续的包头
	STA_HEAD0,
	STA_HEAD1,
	//数据长度
	STA_NUM,
	//有效数据
	STA_DATA,
};

enum{
	VERSION,
	INTERFACE=1,
	INTERFACE2,
	INTERFACE3,
	IPADRESS,
	WIFI_ACCOUNT_PASSWORD,

};//CMD


char tempTcpServerip[20];

void communicat(char* buff)
{

	static uint8_t status = STA_HEAD0;
	static uint8_t packLen = 0;

	switch (status)
	{
		//前二位是包头，第三位是长度，第四接收长度的数据
		case STA_HEAD0:
		{
			if (buff[0] == 0xfd)
			{
				printf("DATA0=%x\r\n",buff[0]);
				status = STA_HEAD1;
			}
		}
		break;
		case STA_HEAD1:
		{
			printf("DATA1=%x\r\n",buff[1]);
			if (buff[1] == 0xfd)
			{
				status = STA_NUM;
			}
			else
			{
				status = STA_HEAD0;
			}
		}
		break;
		case STA_NUM:
		{
			printf("DATA2=%x\r\n",buff[2]);
			packLen = buff[2];
			if(packLen == 0)
			{
				status = STA_HEAD0;
			}
			else
			{
				status = STA_DATA;
			}
		}
		break;
		case STA_DATA:
		{
			printf("DATA3=%x\r\n",buff[3]);
			switch (buff[3])
			{
				case INTERFACE:
				{
					static uint8_t changedata;
					changedata = buff[4];
					iicDevice=changedata;
					if(changedata==0)
						vi5300Init();
					else
						icm42607Init();
					status = STA_HEAD0;
					memset(buff, 0, 50);
				}
				break;
				case IPADRESS:
				{
					receiveFlag.ip=ON;
					memcpy(tcpServerip, buff+4, 40);
					status = STA_HEAD0;
					memset(buff, 0, 50);
				}
				break;
				case WIFI_ACCOUNT_PASSWORD:
				{
					printf("into WIFI_ACCOUNT_PASSWORD\r\n");
					receiveFlag.accou_passw=ON;
					memcpy(temp_account_pass_buff, buff+4, 50);
					status = STA_HEAD0;
					memset(buff, 0, 50);
				}
				break;
				default:
					status = STA_HEAD0;
				break;
			}
		}
		break;
		default:
			status = STA_HEAD0;
		break;
	}
}
void accountpassbuffprocess(char* accpasbuff)
{
	char accountbuff[20];
	char passbuff[20];
	uint8_t i=0,j=0;

	for(i=0;accpasbuff[i]!=' ';i++)//用空格分隔
	{
		accountbuff[i]=accpasbuff[i];
	}
	j=i;
	accountbuff[j]='\0';

	for(i=j+1;i<=40;i++)//用空格分隔
	{
		passbuff[i-j-1]=accpasbuff[i];
	}

	printf("1Flaga_ccou_passw=%d\r\n",receiveFlag.accou_passw);
	printf("accou_passw has open\r\n");
	printf("\r\n accou=%s\r\n",accountbuff);
	printf("\r\n passw=%s\r\n",passbuff);
	memcpy(routerssid,accountbuff,j);
	memcpy(routerpwd,passbuff,40-j);
	printf("\r\n routerssid=%s\r\n",routerssid);
	printf("\r\n routerpwd=%s\r\n",routerpwd);

}

void processaccountpass(void)
{
	if(receiveFlag.accou_passw==ON)
	{
		accountpassbuffprocess(temp_account_pass_buff);

		receiveFlag.accou_passw=OFF;


		wifi_mgmr_sta_disconnect();

	}
}
void communprocess(void* para)
{
	while(1)
	{
		//gpioledl();
		communicat(g_tcp_buff);
		processIP();
		processaccountpass();
	}
    vTaskDelete(NULL);
}

