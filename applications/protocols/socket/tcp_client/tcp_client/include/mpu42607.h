#ifndef __MPU42607__
#define __MPU42607__

#include <stdint.h>


void sensorUpdate(void);


void i2c_42607mpu(void);

uint8_t icm42607Init(void);










#endif /*__MPU42607__*/
