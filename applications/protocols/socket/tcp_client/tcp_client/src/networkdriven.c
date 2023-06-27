
#include "networkdriven.h"
#include <aos/yloop.h>
#include <aos/kernel.h>
#include <hal_wifi.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>
#include <wifi_mgmr_ext.h>
#include <cli.h>
#include <hal_wifi.h>
#include "tcp_example.h"
#include <blog.h>
#include "vi5300.h"
#include "communication.h"

#include "led.h"

//client
//#define ROUTER_SSID "orange"
//#define ROUTER_PWD "12345678"
//This is Ai-Thinker Remote TCP Server: http://tt.ai-thinker.com:8000/ttcloud
//#define TCP_SERVER_IP "192.168.101.2"
//#define tcpServerip "192.168.101.2"
//char tcpServerip[]={'192','.','168','.','101','.','2','\0'};

#define TCP_SERVER_PORT 7878
#define NUMSERVERIP 50
char tcpServerip[NUMSERVERIP]="192.168.101.2";
char routerssid[NUMSERVERIP]= "orange";
char routerpwd[NUMSERVERIP]="12345678";
//extern char tempTcpServerip[NUMSERVERIP];
char g_tcp_buff[50];

static wifi_conf_t conf =
{
    .country_code = "CN",
};	
	
/**	
 * @brief network_connect
 *        wifi station mode connect start
 * @param ssid
 * @param password
 */
static void network_connect(char* ssid, char* password)
{
    wifi_interface_t wifi_interface;

    wifi_interface = wifi_mgmr_sta_enable();
    wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
}
/**									
 * @brief							
 * 									
 * @param arg		
 */					
static void tcp_client_task(void* arg) 
{					
 	//char t_tof_char;
    puts("tcp client task run\r\n");
    int socketfd;	
    int ret = 0;	
    char* tcp_buff = pvPortMalloc(50);
    memset(tcp_buff, 0, 50);
    socketfd = tcp_client_init(tcpServerip, TCP_SERVER_PORT);
    if (!tcp_client_connect(socketfd))
	{
        blog_info("%s:tcp client connect OK\r\n", __func__);
        printf("%s:tcp client connect OK\r\n", tcpServerip);
    }
    else goto __exit;
    if (tcp_client_send(socketfd, "connected tcp server")<0)
	{
        printf("tcp client send fail\r\n");
        goto __exit;
    }
    else
    {
        blog_info("tcp client send OK\r\n");
    }
    while (1) 
 	{
  		//t_tof_char=(char)getTofdata();
		//tcp_client_send(socketfd, &t_tof_char);
		ret = tcp_client_receive(socketfd, tcp_buff);
		if (ret>0)
		{
			memcpy(g_tcp_buff,tcp_buff,50);
			//blog_info("%s:tcp receive data:%d \r\n", __func__, tcp_buff);
			printf("%s",tcp_buff);
			//communication(tcp_buff);
			if (strstr(tcp_buff, "close"))
			{
				goto __exit;
			}
			
			if (strstr(tcp_buff, "disconn"))
			{
				goto __disconn;
			}

			memset(tcp_buff, 0, 50);
		}
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
	
__disconn:
		wifi_mgmr_sta_disconnect();

__exit:
	vPortFree(tcp_buff);
    tcp_client_deinit(socketfd);
    vTaskDelete(NULL);
}

/**
 * @brief network_event
 *      wifi connet ap event Callback function
 * @param event
 * @param private_data
 */
static void network_event(input_event_t* event, void* private_data)
{
    static char* ssid;
    static char* password;

    printf("start_________event->code %x\r\n", event->code);

    switch (event->code)
    {
        case CODE_WIFI_ON_INIT_DONE:
        {
            printf("[APP] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            wifi_mgmr_start_background(&conf);
        }
        break;
        case CODE_WIFI_ON_MGMR_DONE:
        {
            printf("[APP] [EVT] MGMR DONE %lld\r\n", aos_now_ms());
            //_connect_wifi();
            network_connect(routerssid, routerpwd);
        }
        break;
        case CODE_WIFI_ON_SCAN_DONE:
        {	
            printf("[APP] [EVT] SCAN Done %lld\r\n", aos_now_ms());
            // wifi_mgmr_cli_scanlist();
        }
        break;
        case CODE_WIFI_ON_DISCONNECT:
        {
            printf("[APP] [EVT] disconnect %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_CONNECTING:
        {
            printf("[APP] [EVT] Connecting %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_CMD_RECONNECT:
        {
            printf("[APP] [EVT] Reconnect %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_CONNECTED:
        {
            printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_PRE_GOT_IP:
        {
            printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_GOT_IP:
        {
            printf("[APP] [EVT] GOT IP %lld\r\n", aos_now_ms());
            printf("[SYS] Memory left is %d Bytes\r\n", xPortGetFreeHeapSize());
            //WiFi connection succeeded, create TCP client task
            xTaskCreate(tcp_client_task, (char*)"tcp_client_task", 1024*2, NULL, 16, NULL);
        }
        break;
        case CODE_WIFI_ON_PROV_SSID:
        {
            printf("[APP] [EVT] [PROV] [SSID] %lld: %s\r\n",
                   aos_now_ms(),
                   event->value ? (const char*)event->value : "UNKNOWN");
            if (ssid)
            {
                vPortFree(ssid);
                ssid = NULL;
            }
            ssid = (char*)event->value;
        }
        break;
        case CODE_WIFI_ON_PROV_BSSID:
        {
            printf("[APP] [EVT] [PROV] [BSSID] %lld: %s\r\n",
                   aos_now_ms(),
                   event->value ? (const char*)event->value : "UNKNOWN");
            if (event->value)
            {
                vPortFree((void*)event->value);
            }
        }
        break;
        case CODE_WIFI_ON_PROV_PASSWD:
        {
            printf("[APP] [EVT] [PROV] [PASSWD] %lld: %s\r\n", aos_now_ms(),
                   event->value ? (const char*)event->value : "UNKNOWN");
            if (password)
            {
                vPortFree(password);
                password = NULL;
            }
            password = (char*)event->value;
        }
        break;
        case CODE_WIFI_ON_PROV_CONNECT:
        {										
            printf("[APP] [EVT] [PROV] [CONNECT] %lld\r\n", aos_now_ms());	
            printf("connecting to %s:%s...\r\n", ssid, password);			
            network_connect(ssid, password);								
        }										
        break;									
        case CODE_WIFI_ON_PROV_DISCONNECT:
        {
            printf("[APP] [EVT] [PROV] [DISCONNECT] %lld\r\n", aos_now_ms());
        }
        break;
        default:
        {
            printf("[APP] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
            /*nothing*/
        }
    }
    printf("end->code %x\r\n", event->code);
}


void processIP(void)
{
	if(receiveFlag.ip==ON)
	{
		printf("IP address has open\r\n");
		printf("g_tcp_buff=%s\r\n",g_tcp_buff);
		printf("\r\ntcpServerip=%s\r\n",tcpServerip);
		receiveFlag.ip=OFF;
		wifi_mgmr_sta_disconnect();
	}
}


void networkproc(void* para)
{
	//gpioledl();

	aos_register_event_filter(EV_WIFI, network_event, NULL);//记录打印 wifi 模块实时所处的状态
	hal_wifi_start_firmware_task();
	aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);

    vTaskDelete(NULL);
}

