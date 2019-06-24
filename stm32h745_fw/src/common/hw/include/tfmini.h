/*
 * tfmini.h
 *
 *  Created on: 2019. 4. 5.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_TFMINI_H_
#define SRC_COMMON_HW_INCLUDE_TFMINI_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"



#ifdef _USE_HW_TFMINI

#define TFMINI_MAX_CH       HW_TFMINI_MAX_CH




typedef struct
{
  uint8_t trig_flag;
  int16_t strength;
  uint8_t inttime;

  int16_t  distance_cm;
} tfmini_msg_t;


bool tfminiInit(void);
bool tfminiReadMsg(uint8_t channel, tfmini_msg_t *p_msg, uint32_t timeout);
bool tfminiBegin(uint8_t channel);

bool tfminiReadReg(uint8_t channel, uint16_t addr, uint8_t *p_data, uint8_t len, uint32_t timeout);
bool tfminiWriteReg(uint8_t channel, uint16_t addr, uint8_t *p_data, uint8_t len, uint32_t timeout);

err_code_t tfminiGetErrCode(void);
err_code_t tfminiDaigonose(uint8_t level, uint8_t channel);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_TFMINI_H_ */
