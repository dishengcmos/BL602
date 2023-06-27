
#include "mpu42607.h"

#include <hosal_i2c.h>

#include <blog.h>
#include "led.h"
#include <stdint.h>
#include "usart_communication.h"

typedef struct axis_s
{
    float x;
    float y;
    float z;
}axis_t;

/***********************计算方差*******************************************/
// 方差采样计数
#define BIAS_SAMPLES 128

typedef struct _bias
{
	axis_t bias;
	uint8_t isBiasValueFound;
	uint8_t isBufferFilled;
	axis_t *bufHead;
	axis_t buffer[BIAS_SAMPLES];
} bias_t;


bias_t gyroBiasRunning; 
static axis_t gyroBias; 

/*
 * 往方差缓冲区（循环缓冲区）添加一个新值，缓冲区满后，替换旧的的值
 */
static void sensorsAddBiasValue(bias_t *bias, float x, float y, float z)
{
	bias->bufHead->x = x;
	bias->bufHead->y = y;
	bias->bufHead->z = z;
	bias->bufHead++;	 
	
	if (bias->bufHead >= &bias->buffer[BIAS_SAMPLES])
	{
		bias->bufHead = bias->buffer;
		bias->isBufferFilled = 1;
	}
}

/* 计算方差和平均值 */					
static void sensorsCalculateVarianceAndMean(bias_t *bias, axis_t *varOut, axis_t *meanOut)
{
	uint16_t i;					
	float sum[3] = {0};			
	float sumsq[3] = {0};		
/**
	for (i = 0; i < BIAS_SAMPLES; i++)
	{
		sum[0] += bias->buffer[i].x;
		sum[1] += bias->buffer[i].y;
		sum[2] += bias->buffer[i].z;
		sumsq[0] += //powf(bias->buffer[i].x, 2);//bias->buffer[i].x的2次幂
		sumsq[1] += //powf(bias->buffer[i].y, 2);
		sumsq[2] += powf(bias->buffer[i].z, 2);
	}

	varOut->x = (sumsq[0] - powf(sum[0], 2) / BIAS_SAMPLES);
	varOut->y = (sumsq[1] - powf(sum[1], 2) / BIAS_SAMPLES);
	varOut->z = (sumsq[2] - powf(sum[2], 2) / BIAS_SAMPLES);
**/
	varOut->x = 0;
	varOut->y = 0;
	varOut->z = 0;

	meanOut->x = (float)sum[0] / BIAS_SAMPLES;
	meanOut->y = (float)sum[1] / BIAS_SAMPLES;
	meanOut->z = (float)sum[2] / BIAS_SAMPLES;
}

/*gyro传感器查找偏置值
参数bias: 输出,平均值
*/ 
static uint8_t sensorsGyroFindBiasValue(bias_t *bias)
{	
	uint8_t foundbias = 0;
	
	if (bias->isBufferFilled)
	{
		axis_t mean;
		axis_t variance;
		sensorsCalculateVarianceAndMean(bias, &variance, &mean);
		
		if (variance.x < 4000 && variance.y < 4000 && variance.z < 4000)
		{
			bias->bias.x = mean.x;
			bias->bias.y = mean.y;
			bias->bias.z = mean.z;
			foundbias = 1;
			bias->isBiasValueFound = 1;
		}
		else
			bias->isBufferFilled = 0;
	}
	return foundbias;
}

/**
 *	计算陀螺方差 
 参数gyroBiasOut: 输出,平均值
 */	
static uint8_t processGyroBias(int16_t gx, int16_t gy, int16_t gz, axis_t *gyroBiasOut)
{	
	sensorsAddBiasValue(&gyroBiasRunning, gx, gy, gz);
	
	if (!gyroBiasRunning.isBiasValueFound)
	{
		sensorsGyroFindBiasValue(&gyroBiasRunning);
	}
	
	gyroBiasOut->x = gyroBiasRunning.bias.x;
	gyroBiasOut->y = gyroBiasRunning.bias.y;
	gyroBiasOut->z = gyroBiasRunning.bias.z;
	
	return gyroBiasRunning.isBiasValueFound;
}	


/**********************file mpu***************************************/
static hosal_i2c_dev_t i2c0 = {
    .config = {
        .address_width = HOSAL_I2C_ADDRESS_WIDTH_7BIT,/**< @brief Addressing mode: 7 bit or 10 bit */
        .freq = 25000, /**< @brief CLK freq */
        .mode = HOSAL_I2C_MODE_MASTER,/**< @brief master or slave mode */
        .scl = 14,/**< @brief i2c clk pin */
        .sda = 17, /**< @brief i2c data pin */
    },
    .port = 0,
};

#define ICM42607_DEVICE_ADDR 0x68
#define WHO_AM_I 0x75

#define KSCALE_ACC_UNIT_G             0.00048828125f // 16 / (65536/2), 这里是以G作为单位，方便后续的运算
#define MAX_ACCSHAKE_Z 112
#define MIN_ACCSHAKE_Z 96
static float accScale = 1.0f;

uint8_t gshake_42607=0;
enum
{
    X,
    Y,
    Z
};
axis_t accShake;
static axis_t accBias;

static uint8_t mpu42607existenceFlag = 0;
uint8_t icm42607Init(void)
{
	uint8_t id = 0;
	hosal_i2c_mem_read(&i2c0,ICM42607_DEVICE_ADDR,WHO_AM_I,1, &id, 1,100);
	//printf("42607id %d\r\n",id);
	if (id == 0x61)
	{
		mpu42607existenceFlag = 1;
		uint8_t regData = 0x05;
		hosal_i2c_mem_write(&i2c0,ICM42607_DEVICE_ADDR, 0x20,1, &regData,1,100);//配置GURO输出频率和量程
		hosal_i2c_mem_write(&i2c0,ICM42607_DEVICE_ADDR, 0x21,1, &regData,1,100);//配置ACC输出频率和量程
		regData = 0x0f;
		hosal_i2c_mem_write(&i2c0,ICM42607_DEVICE_ADDR, 0x1f,1, &regData,1,100);//主要是开启陀螺仪，并配置功耗
	}
	return mpu42607existenceFlag;
}

// 先用int16来计算校准值，再用float来算出最后结果
// 防止用float来校准丢失精度
void getIcm42607Data(int16_t *acc, int16_t *gyro)
{
	if (mpu42607existenceFlag)
	{
		uint8_t data[12];
		hosal_i2c_mem_read(&i2c0,ICM42607_DEVICE_ADDR,0x0b,1, data, 12,100);
		int16_t t_sensorAcc[3];
		int16_t t_sensorGyro[3];
		t_sensorAcc[X] = (int16_t)(data[1] | (data[0] << 8));
		t_sensorAcc[Y] = (int16_t)(data[3] | (data[2] << 8));
		t_sensorAcc[Z] = (int16_t)(data[5] | (data[4] << 8));
		t_sensorGyro[X] = (int16_t)(data[7] | (data[6] << 8));
		t_sensorGyro[Y] = (int16_t)(data[9] | (data[8] << 8));
		t_sensorGyro[Z] = (int16_t)(data[11] | (data[10] << 8));

		acc[Y] = t_sensorAcc[X];
		acc[X] = t_sensorAcc[Y];
		acc[Z] = t_sensorAcc[Z];
		gyro[Y] = -t_sensorGyro[X];
		gyro[X] = -t_sensorGyro[Y];
		gyro[Z] = t_sensorGyro[Z];
	}
}
/*
static void sensorsAddBiasValue(bias_t *bias, float x, float y, float z)
{
	bias->bufHead->x = x;
	bias->bufHead->y = y;
	bias->bufHead->z = z;
	bias->bufHead++;

	if (bias->bufHead >= &bias->buffer[BIAS_SAMPLES])
	{
		bias->bufHead = bias->buffer;
		bias->isBufferFilled = 1;
	}
}

static uint8_t processGyroBias(int16_t gx, int16_t gy, int16_t gz, axis_t *gyroBiasOut)
{
	sensorsAddBiasValue(&gyroBiasRunning, gx, gy, gz);

	if (!gyroBiasRunning.isBiasValueFound)
	{
		sensorsGyroFindBiasValue(&gyroBiasRunning);
	}

	gyroBiasOut->x = gyroBiasRunning.bias.x;
	gyroBiasOut->y = gyroBiasRunning.bias.y;
	gyroBiasOut->z = gyroBiasRunning.bias.z;

	return gyroBiasRunning.isBiasValueFound;
}
*/
void sensorUpdate(void)
{
	static uint8_t step=0;
	// 传感器校准状态刷新
 	// sensorBiasCalibrated();
	// 获取陀螺仪基本数据
	int16_t t_acc[3], t_gyro[3];

	getIcm42607Data(t_acc, t_gyro);

	// 校准陀螺仪和加速度
	axis_t acc;
	acc.x = t_acc[X] - accBias.x; //acc的平均值
	acc.y = t_acc[Y] - accBias.y; //
	// z轴加速度不需要校准
	acc.z = t_acc[Z];
	// 校准,加速度z不用
	acc.x = (t_acc[X] - accBias.x) * KSCALE_ACC_UNIT_G / accScale;
	acc.y = (t_acc[Y] - accBias.y) * KSCALE_ACC_UNIT_G / accScale;
	acc.z = (t_acc[Z]) * KSCALE_ACC_UNIT_G / accScale;

	accShake.x=acc.x*100;
	accShake.y=acc.y*100;
	//accShake.z=acc.z*100;

	accShake.z=(-acc.z)*100;

//	printf("accZ4=%f\r\n",accShake.z);

	/****
	if(accShake.z>200)
	{
		gshake_42607=~gshake_42607;
	}
	if (gyroBiasRunning.isBiasValueFound == 0)
	{
		processGyroBias(t_gyro[X], t_gyro[Y], t_gyro[Z], &gyroBias);
	}
	****/

	/**/
	switch(step)
	{
		static uint16_t delaynum=0;
		case 0:
			if((accShake.z>MAX_ACCSHAKE_Z)||(accShake.z<MIN_ACCSHAKE_Z))
			{
				step=1;
			}
			else
			{
				step=0;
			}
		break;
		case 1:
			if(delaynum++>30)
			{
				step=2;
				delaynum=0;
			}
		break;
		case 2:
			if((accShake.z>106)||(accShake.z<102))
			{
				step=0;
			}
			else
			{
				gshake_42607=~gshake_42607;
				step=0;
			}
		break;
		default:
		break;
	}

	uint8_t sendData[23] = {0xAA, 0xAA, 2, 18};

	uint8_t usart_senddata[23];
	sendData[4] = 0;
	sendData[5] = (uint8_t)accShake.z;
	sendData[6] = 0;
	sendData[7] = 0;
	sendData[8] = 0;
	sendData[9] = 0;
	sendData[10] = 0;
	sendData[11] = 0;
	uint8_t sum = 0;
	for(uint8_t i = 0; i < 22; i++)
	{
		sum += sendData[i];
	}
	sendData[22] = sum;
	memcpy(usart_senddata, sendData, 23);

	usart_echo_communicat_send(usart_senddata, 23);
	/**/
}

void i2c_42607mpu() 
{
	static uint8_t gshake_42607_tmp=0;
	icm42607Init();
	sensorUpdate();
	//printf("gshake=%d\r\n",gshake_42607);

	if(gshake_42607_tmp!=gshake_42607)
	{
		gshake_42607_tmp=gshake_42607;
		ledstate.ledchange=1;
		
		if(ledstate.ledcolour==red)
		{
			ledstate.ledcolour=green;
		}
		else
		{
			ledstate.ledcolour=red;
		}
		 //后面控制LED灯有个延时正式版要记得删除
	}
	//ledContarl();
}
