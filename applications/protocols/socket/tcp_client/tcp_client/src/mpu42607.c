

 #include "mpu42607.h"

#include <hosal_i2c.h>

#include <blog.h>
#include "led.h"

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
static float accScale = 1.0f;

uint8_t gshake_42607;

typedef struct axis_s
{
    float x;
    float y;
    float z;
}axis_t;
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
	printf("42607id %d\r\n",id);
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



void sensorUpdate(void)
{
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
	accShake.z=acc.z*100;

	printf("accZ4=%f\r\n",accShake.z);
	if(accShake.z>150)
	{
		gshake_42607=1;
	}
}

void i2c_42607mpu()
{
	//icm42607Init();
	sensorUpdate();
	printf("gshake=%d\r\n",gshake_42607);
	if(gshake_42607==1)
	{
		gshake_42607=0;
		ledstate.ledchange=1;
		ledstate.ledcolour=red;
	}
	ledContarl();
}
