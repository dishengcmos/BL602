#ifndef __COMMUCATION_H__
#define __COMMUCATION_H__

#include <stdint.h>



typedef enum{
	VI5300,
	MPU42607,
}iicDevice_e;
extern iicDevice_e iicDevice;

typedef enum{
	OFF,
	ON,
}enumflagswitch;

typedef struct{
	enumflagswitch ip;
	enumflagswitch accou_passw;
}receiveFlag_s;

extern receiveFlag_s receiveFlag;


void communicat(char* buff);
void communprocess(void* para);

void accountpassbuffprocess(char* accpasbuff);


#endif /*__COMMUCATION_H__*/
