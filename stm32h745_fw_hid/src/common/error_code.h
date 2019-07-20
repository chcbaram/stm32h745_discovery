/*
 * error_code.h
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_ERROR_CODE_H_
#define SRC_COMMON_ERROR_CODE_H_



#define OK                            0
#define ERR_MEMORY                    1
#define ERR_FULL                      2
#define ERR_EMPTY                     3
#define ERR_NULL                      4
#define ERR_INVAILD_INDEX             5

#define ERR_I2C_READ                  10
#define ERR_I2C_WRITE                 11


#define ERR_LCD                       50
#define ERR_LCD_TIMEOUT               51
#define ERR_LCD_INVAILD_LAYER         52

#define ERR_TS                        53
#define ERR_TS_TIMEOUT                54
#define ERR_TS_DEV_NOT_FOUND          55

#define ERR_AUDIO                     56
#define ERR_AUDIO_TIMEOUT             57


#define ERR_INVALID_CMD               100
#define ERR_INVALID_LENGTH            239
#define ERR_FLASH_INVALID_CHECK_SUM   240
#define ERR_FLASH_INVALID_TAG         241
#define ERR_INVALID_FW                242
#define ERR_FLASH_ADDR_ALIGN          243
#define ERR_FLASH_INVALID_ADDR        244
#define ERR_FLASH_ERROR               245
#define ERR_FLASH_BUSY                246
#define ERR_FLASH_ERR_TIMEOUT         247
#define ERR_FLASH_NOT_EMPTY           248
#define ERR_FLASH_WRITE               249
#define ERR_FLASH_READ                250
#define ERR_FLASH_ERASE               251
#define ERR_FLASH_PACKET_SIZE         252
#define ERR_FLASH_SIZE                253
#define ERR_FLASH_CRC                 254


#define ERR_MEMORY_ERASE_LENGTH       300
#define ERR_MEMORY_ERASE_ALIGN        301
#define ERR_MEMORY_ERASE_PARAM        302
#define ERR_MEMORY_ERASE_RANGE        304
#define ERR_MEMORY_READ_LENGTH        305
#define ERR_MEMORY_READ_PARAM         306
#define ERR_MEMORY_READ_RANGE         307
#define ERR_MEMORY_WRITE_LENGTH       308
#define ERR_MEMORY_WRITE_PARAM        309
#define ERR_MEMORY_WRITE_ALIGN        310
#define ERR_MEMORY_WRITE_RANGE        311


#define ERR_DXL_ERROR                 400

#define ERR_RPI_ERROR                 500

#define ERR_DXL_ERROR_RESP            700


#define ERR_DXL_NOT_OPEN              1000
#define ERR_DXL_WRITE_BUFFER          1001
#define ERR_DXL_NOT_FOUND             1002
#define ERR_DXL_WRTIE_USB             1003
#define ERR_DXL_WRTIE_UART            1004
#define ERR_DXL_WRTIE_BLE             1005
#define ERR_DXL_WRTIE_MOTOR           1006


#define ERR_MOTION_OVER_STEP          1100
#define ERR_MOTION_OVER_PAGE          1101
#define ERR_MOTION_WRITE_BUFFER       1102
#define ERR_MOTION_FAIL_START         1103
#define ERR_MOTION_OVER_GOAL          1104
#define ERR_MOTION_WAIT_TIMEOUT       1105
#define ERR_MOTION_PAGE_VALID         1106

#define ERR_TASK_RANGE                1200
#define ERR_TASK_INVAILD_PARAM        1201
#define ERR_TASK_UNKNOWN_TYPE         1202
#define ERR_TASK_CONST_TYPE           1203
#define ERR_TASK_VAR_LENGTH           1204
#define ERR_TASK_VAR_TYPE             1205
#define ERR_TASK_INVAILD_TYPE         1206
#define ERR_TASK_PUT_INVAILD_TYPE     1207
#define ERR_TASK_INVAILD_OPERATOR     1208
#define ERR_TASK_ACC_LENGTH           1209
#define ERR_TASK_STACK_OVER           1210
#define ERR_TASK_STACK_UNDER          1211
#define ERR_TASK_LENGTH_OVER          1212
#define ERR_TASK_CONST_ID_CONST_ADDR_TYPE   1213
#define ERR_TASK_INVAILD_SENTENCE     1214
#define ERR_TASK_INVAILD_COMPUTE      1215


#define ERR_ETHERNET                  4000
#define ERR_ETHERNET_JOIN_GROUP       4040
#define ERR_ETHERNET_LEAVE_GROUP      4050


#define ERR_SONIC                     5000
#define ERR_SONIC_THREAD_FAIL         5001
#define ERR_SONIC_THREAD_START_FAIL   5002
#define ERR_MOTOR_THREAD_FAIL         5003
#define ERR_MOTOR_THREAD_START_FAIL   5004
#define ERR_WATCHDOG_THREAD_FAIL      5005
#define ERR_WATCHDOG_THREAD_START_FAIL 5006
#define ERR_ENV_THREAD_FAIL           5007
#define ERR_ENV_THREAD_START_FAIL     5008
#define ERR_BAT_THREAD_FAIL           5009
#define ERR_BAT_THREAD_START_FAIL     5010
#define ERR_CLIFF_THREAD_FAIL         5011
#define ERR_CLIRR_THREAD_START_FAIL   5012
#define ERR_LCD_THREAD_FAIL           5013
#define ERR_LCD_THREAD_START_FAIL     5014
#define ERR_NEO_THREAD_FAIL           5015
#define ERR_NEO_THREAD_START_FAIL     5016

#define ERR_MOTOR                     6000
#define ERR_MOTOR_REBOOT_FAIL         6001
#define ERR_MOTOR_ENABLE_FAIL         6002





#endif /* SRC_COMMON_ERROR_CODE_H_ */
