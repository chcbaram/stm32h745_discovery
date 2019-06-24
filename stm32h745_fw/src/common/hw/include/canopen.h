/*
 * canopen.h
 *
 *  Created on: 2019. 4. 24.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_CANOPEN_H_
#define SRC_COMMON_HW_INCLUDE_CANOPEN_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_CANOPEN



enum CobType
{
  NMT  = 0,
  SYNC,
  TIME,

  EMCY,
  PDO1_T,
  PDO1_R,
  PDO2_T,
  PDO2_R,
  PDO3_T,
  PDO3_R,
  PDO4_T,
  PDO4_R,
  SDO_T,
  SDO_R,
  NMT_E,
  COBTYPE_MAX
};



bool canOpenInit(void);
bool canOpenBegin(uint8_t channel, uint8_t baud);
void canOpenFlush(uint32_t timeout);

err_code_t canOpenReadSDO(uint16_t node_id, uint16_t index, uint8_t sub_index, uint8_t *p_data, uint8_t len, uint32_t timeout);
err_code_t canOpenWriteSDO(uint16_t node_id, uint16_t index, uint8_t sub_index, uint8_t *p_data, uint8_t len, uint32_t timeout);

err_code_t canOpenWritePDO(enum CobType cob_type_index, uint16_t node_id, uint8_t *p_data, uint8_t len, uint32_t timeout);
err_code_t canOpenMappingPDO(enum CobType cob_type_index, uint16_t node_id, uint16_t index, uint8_t sub_index, uint8_t len_bit, uint32_t timeout);

err_code_t canOpenSendNMT(uint16_t node_id, uint8_t command);
err_code_t canOpenSendSYNC(void);
err_code_t canOpenCommandGet(uint16_t node_id, const char *p_cmd_str, uint16_t index, data_t *p_data, uint32_t timeout);
err_code_t canOpenCommandSet(uint16_t node_id, const char *p_cmd_str, uint16_t index, data_t *p_data, uint32_t timeout);


#endif /* _USE_HW_CAN */

#ifdef __cplusplus
 }
#endif



#endif /* SRC_COMMON_HW_INCLUDE_CANOPEN_H_ */
