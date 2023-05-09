
#include "pwmdriven.h"
#include <bl_pwm.h>

#include <bl602_gpio.h>

#include <bl602_glb.h>

#define PWM_Get_Channel_Reg(ch)  (PWM_BASE+PWM_CHANNEL_OFFSET+(ch)*0x20)
#define PWM_STOP_TIMEOUT_COUNT          (160*1000)


#define GPIO_BEEP_PIN 5
/// @brief Copy from PWM_Smart_Configure. Use Bus Clock instead of External Crystal Clock for PWM Timer
/// @param ch PWM Channel
/// @param clkDiv PWM clock divider
/// @param period PWM period
/// @param threshold2 （threshold1等于0）
/// @return
BL_Err_Type PWM_Smart_Configure2(PWM_CH_ID_Type ch, uint16_t clkDiv, uint16_t period,uint16_t threshold2)
{
    uint32_t tmpVal;
    uint32_t timeoutCnt = PWM_STOP_TIMEOUT_COUNT;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    // if(BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL) != PWM_CLK_XCLK){
    if(BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL) != PWM_CLK_BCLK){
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmpVal, PWM_STOP_EN));
        while(!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)){
            timeoutCnt--;
            if(timeoutCnt == 0){
                return TIMEOUT;
            }
        }
        // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL, PWM_CLK_XCLK);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL, PWM_CLK_BCLK);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_OUT_INV, PWM_POL_NORMAL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_STOP_MODE, PWM_STOP_GRACEFUL);
    BL_WR_REG(PWMx, PWM_CONFIG, tmpVal);

    /* Config pwm division */
    BL_WR_REG(PWMx, PWM_CLKDIV, clkDiv);

    /* Config pwm period and duty */
    BL_WR_REG(PWMx, PWM_PERIOD, period);
    BL_WR_REG(PWMx, PWM_THRE1, 0);
    BL_WR_REG(PWMx, PWM_THRE2, threshold2);

    return SUCCESS;
}


void pwminit(void)
{
    GLB_GPIO_Cfg_Type cfg =
	{
		.drive = 0,
		.smtCtrl = 1,
		.gpioMode = GPIO_MODE_OUTPUT,
		.pullType = GPIO_PULL_DOWN,
		.gpioPin = 5,  /// pwm BEEP GPIO
		.gpioFun = 8,
	};

    GLB_GPIO_Init(&cfg);
    PWM_CH_ID_Type ch=0;
    PWM_Channel_Disable(ch);

    /// set pwm channel 0 for  % duty, 1MHz
    /// param1 频道
    /// param2 分频系数
    /// param3 周期，即每个pwm周期共经历的计数个数
    /// 实际的pwm每个计数周期等于 80mhz/分频系数/周期
    /// param4 这个是threshold2，表示在哪个计数个数下电平变高。 （threshold1等于0）
    PWM_Smart_Configure2(0, 8, 10, 0);
    PWM_Channel_Enable(0);
}




