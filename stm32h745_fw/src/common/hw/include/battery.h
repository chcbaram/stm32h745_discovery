/*
 * battery.h
 *
 *  Created on: 2019. 3. 28.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_BATTERY_H_
#define SRC_COMMON_HW_INCLUDE_BATTERY_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_BATTERY

#define BATTERY_MAX_CH       HW_BATTERY_MAX_CH



typedef struct
{
  uint16_t voltage;               // 전압  0.01V
  int16_t  current;               // 전류  0.01A
  uint16_t battery_condition;     // 배터리 상태
  uint16_t time_to_full;          // 충전완료 시간  1min
  uint16_t time_to_empty;         // 방전완료 시간  1min
  uint8_t  state_of_charge;       // 잔량(%)
  uint8_t  state_of_health;       // 수명(%)
  uint16_t remaining_capacity;    // 잔류 용량(Ah)   0.01Ah
  uint16_t available_energy;      // 잔여 에너지(Wh) 0.1Wh
  int16_t  temperature;           // 온도 0.1도
} battery_info_t;



bool batteryInit(void);
bool batteryGetInfo(uint8_t channel, battery_info_t *p_info);

#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_BATTERY_H_ */
