/*
 * bsp.h
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "def.h"
#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

void bspInit(void);

extern void delay(uint32_t ms);
uint32_t millis(void);


#ifdef __cplusplus
 }
#endif
#endif /* SRC_BSP_BSP_H_ */
