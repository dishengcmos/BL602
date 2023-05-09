#include <FreeRTOS.h>

#include <task.h>
#include "vi5300.h"
#include <hosal_i2c.h>

#include <blog.h>
#include "vi5300string.h"
#include "led.h"


static float t_tof;
uint8_t gSalve;
uint8_t chip_reg = 0xff;
typedef struct
{
    int16_t millimeter;
    uint32_t peak;
    uint16_t noise;
    uint32_t confidence;

} VI5300_Dist_TypeDef;

typedef struct
{
    int8_t xtalk_cal;
    uint16_t xtalk_peak;

} VI5300_XTALK_Calib_Data;

typedef struct OFFSET_CALIB
{
    int16_t offset_cal;

} VI5300_OFFSET_Calib_Data;

typedef enum
{
    VI5300_OK = 0x00,
    VI5300_RANGING = 0x01,
    VI5300_BUSY = 0x02,
    VI5300_BUS_BUSY = 0x03,
    VI5300_SLEEP = 0x04,
    VI5300_BOOTING = 0x05,
    VI5300_ERROR = 0x06
} VI5300_Status;

typedef enum
{
    VI5300_DATA_VALID = 0x00,
    VI5300_RAW_DATA = 0x01,
    VI5300_DATA_ERROR = 0x02
} VI5300_RangingDataStatus;
typedef enum NEXTACTION
{
    VI5300_GO_ON = 0x00,
    VI5300_STOP = 0x01
} VI5300_NextAction;

VI5300_XTALK_Calib_Data xtalk_cal;
VI5300_OFFSET_Calib_Data offset_cal;

// 判断芯片是否存在
static uint8_t existenceFlag = 0;
VI5300_Dist_TypeDef distance;

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
void bl602_i2c_init(void)
{
	hosal_i2c_init(&i2c0);
}
//static hosal_i2c_dev_t i2c_mpu = {
//    .config = {
//        .address_width = HOSAL_I2C_ADDRESS_WIDTH_7BIT,/**< @brief Addressing mode: 7 bit or 10 bit */
//        .freq = 25000, /**< @brief CLK freq */
//        .mode = HOSAL_I2C_MODE_MASTER,/**< @brief master or slave mode */
//        .scl = 14,/**< @brief i2c clk pin */
//        .sda = 17, /**< @brief i2c data pin */
//    },
//    .port = 0,
//};

void VI5300_Set_Digital_Clock_Dutycycle(void)
{
    uint8_t t_reg;
    t_reg = 0x0F;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_PW_CTRL,1, &t_reg, 1,100);
    t_reg = 0x0E;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_PW_CTRL,1, &t_reg, 1,100);
    if (chip_reg == 0x00)
    {
        vTaskDelay(1);
        t_reg = 0x24;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0xE9, 1, &t_reg, 1,100);
        t_reg = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0xEE, 1, &t_reg, 1,100);
        t_reg = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0xF5, 1, &t_reg, 1,100);
    }
    vTaskDelay(4); //默认4ms ，延时不准可以增加
}

void VI5300_WriteFirmwarePostConfig(void)
{
    uint8_t t_reg;
    t_reg = 0x0c;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_SYS_CFG, 1, &t_reg, 1,100);
    t_reg = 0x06;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_MCU_CFG, 1, &t_reg, 1,100);
    t_reg = 0xA0;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3B, 1, &t_reg, 1,100);
    t_reg = 0x80;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3B, 1, &t_reg, 1,100);

    if (chip_reg == 0x00)
    {
        t_reg = 0x24;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0xE9, 1, &t_reg, 1,100);
        t_reg = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0xEE, 1, &t_reg, 1,100);
        t_reg = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0xF5, 1, &t_reg, 1,100);
    }

    t_reg = 0x07;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_MCU_CFG, 1, &t_reg, 1,100);
    t_reg = 0x02;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_PW_CTRL, 1, &t_reg, 1,100);
    t_reg = 0x00;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_PW_CTRL, 1, &t_reg, 1,100);
}

void VI5300_WriteFirmware32Byte(uint8_t len, uint8_t *data)
{
    uint8_t cmd_size[2];

    if (len > 32)
        return;

    cmd_size[0] = VI5300_WRITEFW_CMD;
    cmd_size[1] = len;

    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_CMD,1, cmd_size, 2,100);
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_SCRATCH_PAD_BASE,1, data, len,100);
}

void VI5300_WriteFirmwarePreConfig(void)
{
    uint8_t t_reg = 0x01;
    VI5300_Set_Digital_Clock_Dutycycle(); // set rco config add 20210129
    t_reg = 0x08;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_PW_CTRL, 1, &t_reg, 1,100);
    t_reg = 0x0a;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_PW_CTRL, 1, &t_reg, 1,100);
    t_reg = 0x06;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_MCU_CFG, 1, &t_reg, 1,100);
    hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_SYS_CFG, 1, &t_reg, 1,100);
    t_reg |= 0x01;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_SYS_CFG, 1, &t_reg, 1,100);

    if (chip_reg == 0x00)
    {
        t_reg = 0x30;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x38, 1, &t_reg, 1,100);
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3A, 1, &t_reg, 1,100);
    }

    t_reg = 0x01;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_CMD, 1, &t_reg, 1,100);
    t_reg = 0x02;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_SIZE, 1, &t_reg, 1,100);
    t_reg = 0x00;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_SCRATCH_PAD_BASE, 1, &t_reg, 1,100);
    t_reg = 0x00;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_SCRATCH_PAD_BASE + 0x01, 1, &t_reg, 1,100);
}

void VI5300_DownloadFirmware(uint8_t *buf, uint16_t size)
{

    uint8_t page = 0;

    VI5300_WriteFirmwarePreConfig();
    while (size >= 32)
    {
        VI5300_WriteFirmware32Byte(32, buf + page * 32);
        size -= 32;
        page++;
    }

    if (size > 0)
    {
        VI5300_WriteFirmware32Byte(size, buf + page * 32);
    }

    VI5300_WriteFirmwarePostConfig();
}

void VI5300_Interrupt_Enable(void)
{
    uint8_t loop = 0;
    uint8_t enable = 0;

    do
    {
        hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x04,1, &enable, 1,100);
        enable |= 0x01;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x04,1, &enable, 1,100);
        hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x04,1, &enable, 1,100);
        loop++;
    } while ((loop < 5) && (!(enable & 0x01)));

    if (loop >= 5)
    {
        // 中断出错
        existenceFlag = 0;
    }
}

void VI5300_Integral_Counts_Write(uint32_t inte_counts)
{
    uint8_t buf[4] = {0};
    //小端模式，从小到大保存
    buf[0] = (inte_counts)&0xFF;
    buf[1] = (inte_counts >> 8) & 0xFF;
    buf[2] = (inte_counts >> 16) & 0xFF;

    VI5300_Set_Digital_Clock_Dutycycle();
    uint8_t t_reg;
    t_reg = 0x01;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0C, 1, &t_reg, 1,100);
    t_reg = 0x03;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0D, 1, &t_reg, 1,100);
    t_reg = 0x01;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0E, 1, &t_reg, 1,100);
    t_reg = buf[0];
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0F, 1, &t_reg, 1,100);
    t_reg = buf[1];
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x10, 1, &t_reg, 1,100);
    t_reg = buf[2];
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x11, 1, &t_reg, 1,100);
    t_reg = 0x09;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0A, 1, &t_reg, 1,100);
}

void VI5300_Delay_Count_Write(uint16_t delay_count)
{
    uint8_t buf[2] = {0};

    //大端模式
    buf[0] = (delay_count >> 8) & 0xFF;
    buf[1] = (delay_count)&0xFF;

    VI5300_Set_Digital_Clock_Dutycycle();

    uint8_t t_reg;
    t_reg = 0x01;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0C, 1, &t_reg, 1,100);
    t_reg = 0x02;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0D, 1, &t_reg, 1,100);
    t_reg = 0x04;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0E, 1, &t_reg, 1,100);
    //大端模式
    t_reg = buf[0];
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0F, 1, &t_reg, 1,100);
    t_reg = buf[1];
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x10, 1, &t_reg, 1,100);
    t_reg = 0x09;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x0A, 1, &t_reg, 1,100);
}

void VI5300_Set_Integralcounts_Frame(uint8_t fps, uint32_t intecoutns)
{
    uint32_t inte_time;
    uint32_t fps_time;
    uint32_t delay_time;
    uint16_t delay_counts;

    inte_time = intecoutns * 1463 / 10;
    fps_time = 1000000000 / fps;
    delay_time = fps_time - inte_time - 1600000;
    delay_counts = (uint16_t)(delay_time / 40900);

    VI5300_Integral_Counts_Write(intecoutns);
    VI5300_Delay_Count_Write(delay_counts);
}

void VI5300_Wait_For_CPU_Ready(void)
{
    uint8_t stat;
    int retry = 0;

    do
    {
        vTaskDelay(1); // delay 1ms
        hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x02,1,&stat,1,100);
    } while ((retry++ < 20) && (stat & 0x01));
    if (retry >= 20)
    {
        existenceFlag = 0;
    }
}

void VI5300_Start_Continuous_Measure(void)
{
    uint8_t stat = 0;

    hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, VI5300_RET_INT_STATUS,1, &stat, 1,100);

    VI5300_Wait_For_CPU_Ready();

    VI5300_Set_Digital_Clock_Dutycycle();

    uint8_t t_reg = 0x0F;
    hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_CMD, 1, &t_reg, 1,100);
}

uint8_t vi5300Init(void)
{
    uint8_t stat = 0;
    uint8_t retry = 0;
    uint8_t t_reg = 0xFF, t_reg1 = 0xF0, t_reg2;

    hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x06, 1, &t_reg2, 1,100);
	printf("ID data %d\r\n",t_reg2);
    if (t_reg2 == 0xD8)
    {
        for (retry = 0; retry < 5; retry++)
        {
            hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x38, 1, &t_reg, 1,100);
            if (t_reg == 0x30 || t_reg == 0x00)
            {
                hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x3A, 1, &t_reg1, 1,100);
                if (t_reg1 == t_reg)
                {
                    chip_reg = t_reg;
                    break;
                }
            }
        }
        vTaskDelay(500);
        t_reg2 = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x00, 1, &t_reg2, 1,100);
        t_reg2 = 0x0c;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x01, 1, &t_reg2, 1,100);
        t_reg2 = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x07, 1, &t_reg2, 1,100);
        t_reg2 = 0x01;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x07, 1, &t_reg2, 1,100);
        t_reg2 = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x07, 1, &t_reg2, 1,100);
        t_reg2 = 0x21;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x04, 1, &t_reg2, 1,100);
        t_reg2 = 0x0E;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x05, 1, &t_reg2, 1,100);
        t_reg2 = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x08, 1, &t_reg2, 1,100);
        t_reg2 = 0x80;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x37, 1, &t_reg2, 1,100);
        t_reg2 = t_reg;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x38, 1, &t_reg2, 1,100);
        t_reg2 = t_reg1;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3A, 1, &t_reg2, 1,100);
        t_reg2 = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x39, 1, &t_reg2, 1,100);
        t_reg2 = 0x80;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3B, 1, &t_reg2, 1,100);
        t_reg2 = 0x80;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3C, 1, &t_reg2, 1,100);
        t_reg2 = 0x80;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3D, 1, &t_reg2, 1,100);
        t_reg2 = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3E, 1, &t_reg2, 1,100);
        t_reg2 = 0x00;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x3F, 1, &t_reg2, 1,100);
        t_reg2 = 0x0E;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x07, 1, &t_reg2, 1,100);
        t_reg2 = 0x0F;
        hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x07, 1, &t_reg2, 1,100);

        retry = 0;
        do
        {
            vTaskDelay(1);                              // delay 1ms
            hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x02,1, &stat, 1,100); // stat = 0x10
        } while ((retry++ < 20) && (stat & 0x01));

        hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x08,1, &stat, 1,100);
        vTaskDelay(40);
        do
        {
            t_reg2 = 0x00;
            hosal_i2c_mem_write(&i2c0,VI5300_DEVICE_ADDR, 0x08, 1, &t_reg2, 1,100);
            vTaskDelay(1);
            hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x08,1, &stat, 1,100);
        } while (stat != 0 && retry++ < 5);
        // 芯片固件开始启动，大约持续 40 ms，
        // 可以通过确认 0x08 寄存器是否为 0x66 来判断是否启动成功。
        // 固件启动后，芯片会自动进入软件休眠模式。
        retry = 0;
        do
        {
            VI5300_DownloadFirmware(g_pVi5300Firmware, getFirmwareLen());
            vTaskDelay(10); // delay 10ms
            hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, 0x08,1, &stat, 1,100);
        } while ((stat != 0x55) && (stat != 0x66) && (retry++ < 5));

        if (retry < 5)
        {
            existenceFlag = 1;

            VI5300_Interrupt_Enable();
            VI5300_Set_Integralcounts_Frame(30, 131000);
            // 开始测量
            VI5300_Start_Continuous_Measure();
        }
    }

    return existenceFlag;
}

void VI5300_GetRawRangingData(VI5300_Dist_TypeDef *result)
{
    uint8_t buf[32];
    int16_t milimeter;
    uint16_t noise;
    uint32_t peak1;
    uint32_t peak2;
    uint32_t integral_times;
    uint32_t peak_tmp;
    uint16_t noise_tmp;
    uint32_t lower;
    uint32_t upper;
    int32_t bias;
    uint32_t confidence;

    hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, VI5300_REG_SCRATCH_PAD_BASE,1, buf, 32,100);

    /***********add the function of pileup and confidence 20210105  小端模式*********/
    milimeter = (short)((((short)buf[13]) << 8) | (((short)buf[12])));
    peak1 = (uint32_t)((((uint32_t)buf[31]) << 24) | ((uint32_t)buf[30] << 16) | ((uint32_t)buf[29] << 8) | ((uint32_t)buf[28]));
    noise = (uint16_t)((((uint16_t)buf[27]) << 8) | (((uint16_t)buf[26])));
    peak2 = (uint32_t)((((uint32_t)buf[11]) << 24) | (((uint32_t)buf[10]) << 16) | (((uint32_t)buf[9]) << 8) | ((uint32_t)buf[8]));

    integral_times = (uint32_t)((((uint32_t)buf[25]) << 24) | (((uint32_t)buf[24]) << 16) | (((uint32_t)buf[23]) << 8) | ((uint32_t)buf[22]));
    integral_times = integral_times & 0x00ffffff;
    if (peak2 > 65536)
    {
        peak_tmp = (peak2 * 256) / integral_times * 256;
    }
    else
    {
        peak_tmp = peak2 * 65536 / integral_times;
    }

    peak_tmp = peak_tmp >> 12;
    bias = (int32_t)(PILEUP_A / (PILEUP_B - peak_tmp * PILEUP_D) - PILEUP_C) / PILEUP_D;
    if (bias < 0)
    {
        bias = 0;
    }
    milimeter = milimeter + (int16_t)bias;

#ifdef VI5300_OFFSET_CALIBRATION
    milimeter = milimeter - offset_cal.offset_cal; //减去offset
#endif

    noise_tmp = noise / 8;
    if (noise_tmp < 45)
    {
        lower = 228 * noise / 8 + 1260;
        upper = 269 * noise / 8 + 2284;
    }
    else if (noise_tmp < 94)
    {
        lower = 252 * noise / 8 + 172;
        upper = 300 * noise / 8 + 901;
    }
    else if (noise_tmp < 154)
    {
        lower = 190 * noise / 8 + 6066;
        upper = 227 * noise / 8 + 7703;
    }
    else if (noise_tmp < 246)
    {
        lower = 209 * noise / 8 + 3043;
        upper = 237 * noise / 8 + 6121;
    }
    else if (noise_tmp < 721)
    {
        lower = 197 * noise / 8 + 6006;
        upper = 216 * noise / 8 + 11300;
    }
    else if (noise_tmp < 1320)
    {
        lower = 201 * noise / 8 + 3177;
        upper = 226 * noise / 8 + 4547;
    }
    else if (noise_tmp < 2797)
    {
        lower = 191 * noise / 8 + 16874;
        upper = 217 * noise / 8 + 16340;
    }
    else if (noise_tmp < 3581)
    {
        lower = 188 * noise / 8 + 23925;
        upper = 218 * noise / 8 + 14012;
    }
    else
    {
        lower = 180 * noise / 8 + 53524;
        upper = 204 * noise / 8 + 62305;
    }
    if (peak1 < lower)
    {
        confidence = 0;
    }
    else if (peak1 > upper)
    {
        confidence = 100;
    }
    else
    {
        confidence = 100 * (peak1 - lower) / (upper - lower);
    }

    result->millimeter = milimeter;
    result->peak = peak1;
    result->noise = noise;
    result->confidence = confidence;
}

uint8_t VI5300_Get_Measure_Data(float *dist)
{
    uint8_t int_status = 0;
    hosal_i2c_mem_read(&i2c0,VI5300_DEVICE_ADDR, VI5300_RET_INT_STATUS,1, &int_status, 1,100);
    if ((int_status & 0x01) == 0x00)
    {
        return 0;
    }
    VI5300_GetRawRangingData(&distance);
 	if(distance.confidence>0)
		*dist = distance.millimeter * 0.1f;	    // dist单位为cm
 	else
 		return 0;
    return 1;
}




void i2c_tof()//void *param
{
	//vi5300Init();
	//puts("start measure\r\n");
	if(VI5300_Get_Measure_Data(&t_tof)==1)
	{
		if((t_tof>20.0f)&&(ledstate.ledcolour!=green))
		{
			printf("tof if \r\n");
			ledstate.ledcolour=green;
			ledstate.ledchange=1;
			printf("colour= %d \r\n",ledstate.ledcolour);
			printf("change= %d \r\n",ledstate.ledchange);
		}
		else if((t_tof<20.0f)&&(ledstate.ledcolour!=blue))
		{
			printf("tof else if\r\n");
			ledstate.ledcolour=red;
			ledstate.ledchange=1;
			printf("colour= %d \r\n",ledstate.ledcolour);
			printf("change= %d \r\n",ledstate.ledchange);
		}
	}
	ledContarl();
}

float getTofdata(void)
{
	return t_tof;
}
