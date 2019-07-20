/*
 * elmo.h
 *
 *  Created on: 2019. 4. 24.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_DRIVER_ELMO_H_
#define SRC_HW_DRIVER_ELMO_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_ELMO


#define ELMO_MAX_CH   HW_ELMO_MAX_CH


#define ELMO_MODE_NONE                      0
#define ELMO_MODE_NOT_READY_TO_SWITCH_ON    1
#define ELMO_MODE_SWITCH_ON_DISABLED        2
#define ELMO_MODE_READY_TO_SWITCH_ON        3
#define ELMO_MODE_SWITCH_ON                 4
#define ELMO_MODE_OPERAION_ENABLED          5
#define ELMO_MODE_QUICK_STOP_ACTIVE         6
#define ELMO_MODE_FAULT_REACTION_ACTIVE     7
#define ELMO_MODE_FAULT                     8
#define ELMO_MODE_CAN_ERR                   0xFF



bool elmoInit(void);
bool elmoEnable(uint8_t node_id, uint32_t timeout);
bool elmoDisable(uint8_t node_id);
bool elmoInitParameter(uint8_t node_id);
bool elmoSetVelocity(uint8_t node_id, int32_t vel);
bool elmoGetInput(uint8_t node_id, uint32_t *p_data);
bool elmoGetVelocity(uint8_t node_id, int32_t *p_vel);
bool elmoGetPosition(uint8_t node_id, int32_t *p_pos);
bool elmoGetCurrent(uint8_t node_id, int32_t *p_cur);
bool elmoGetRateCurrent(uint8_t node_id, int32_t *p_cur);
bool elmoGetStatus(uint8_t node_id, uint32_t *p_status);
bool elmoGetErrorCode(uint8_t node_id, uint32_t *p_error);
bool elmoGetTemperature(uint8_t node_id, int8_t *p_temp);
bool elmoSetProfileAcc(uint8_t node_id, int32_t value);
bool elmoSetProfileDec(uint8_t node_id, int32_t value);
bool elmoSetProfileStop(uint8_t node_id, int32_t value);

uint32_t elmoGetError(void);
uint8_t elmoGetState(uint8_t node_id);


#endif /* _USE_HW_CAN */

#ifdef __cplusplus
 }
#endif



#endif /* SRC_HW_DRIVER_ELMO_H_ */
