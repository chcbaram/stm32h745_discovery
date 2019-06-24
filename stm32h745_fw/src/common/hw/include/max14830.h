/*
 * max14830.h
 *
 *  Created on: 2019. 4. 2.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_CORE_MAX14830_H_
#define SRC_HW_CORE_MAX14830_H_


#include "hw_def.h"


#define MAX14830_MAX_CH        4

void max14830Init(void);
bool max14830IsConnected(void);
uint16_t max14830ReadReg(uint8_t channel, uint8_t addr);
uint8_t max14830ReadIrq(uint8_t channel, uint8_t addr);
void max14830WriteReg(uint8_t channel, uint8_t addr, uint8_t data);

bool     max14830Open(uint8_t channel, uint32_t baud);
uint32_t max14830Available(uint8_t channel);
void     max14830Flush(uint8_t channel);
int32_t  max14830Write(uint8_t channel, uint8_t *p_data, uint32_t length);
uint8_t  max14830Read(uint8_t channel);


#endif /* SRC_HW_CORE_MAX14830_H_ */
