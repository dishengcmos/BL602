#ifndef _NETWORKDRIVEN_H_
#define _NETWORKDRIVEN_H_

#include <stdint.h>
extern char tcpServerip[];
extern char routerssid[];
extern char routerpwd[];

extern char g_tcp_buff[50];
void networkproc(void* para);
void processIP(void);












#endif

