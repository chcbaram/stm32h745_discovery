/*
 * bme280.h
 *
 *  Created on: 2019. 3. 21.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_BME280_H_
#define SRC_COMMON_HW_INCLUDE_BME280_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_BME280

#define BME280_MAX_CH       HW_BME280_MAX_CH


#define BME280_ADDRESS                (0x77)



bool bme280Init(void);
bool bme280Begin(uint8_t channel);
bool bme280ReadValues(uint8_t channel, int32_t *T, uint32_t *P, uint32_t *H);


#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_BME280_H_ */
