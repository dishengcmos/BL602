#ifndef __MAIN_BOARD_H__
#define __MAIN_BOARD_H__

#include <stdint.h>

void ledStatusContar(void);
uint32_t bl60x_board_get_factory_addr(void);
int bl60x_board_cfg(uint8_t board_code);

uint8_t VI5300_Get_Measure_Data(float *dist);




#endif /*__MAIN_BOARD_H__*/


