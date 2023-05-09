#ifndef __VI5300_H__
#define __VI5300_H__

#include <stdint.h>

#define VI5300_DEVICE_ADDR 0x6C
#define VI5300_REG_MCU_CFG 0x00
#define VI5300_RET_INT_STATUS 0x03
#define VI5300_REG_SYS_CFG 0x01
#define VI5300_REG_PW_CTRL 0x07
#define VI5300_REG_CMD 0x0a
#define VI5300_REG_SIZE 0x0b
#define VI5300_REG_SCRATCH_PAD_BASE 0x0c

#define VI5300_WRITEFW_CMD 0x03
#define VI5300_USER_CFG_CMD 0x09
#define VI5300_START_RANG_CMD 0x0E

#define VI5300_CFG_SUBCMD 0x01
#define VI5300_OTPW_SUBCMD 0x02
#define VI5300_OTPR_SUBCMD 0x03

#define PILEUP_A (9231000)
#define PILEUP_B (4896)
#define PILEUP_C (1922)
#define PILEUP_D (10)

#define XSHUT_Pin1 0      // GPIO_PIN_0
#define XSHUT_Pin2 0      // GPIO_PIN_1
#define XSHUT_Pin3 0      // GPIO_PIN_4
#define XSHUT_GPIO_Port 0 // GPIOA






uint8_t vi5300Init(void);
void VI5300_DownloadFirmware(uint8_t *buf, uint16_t size);


float getTofdata(void);


void i2c_tof(void);
void bl602_i2c_init(void);





#endif /*__VI5300_H__*/

