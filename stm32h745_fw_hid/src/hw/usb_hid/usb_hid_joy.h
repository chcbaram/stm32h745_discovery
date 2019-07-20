/*
 * usb_hid_joystick.h
 *
 *  Created on: 2019. 7. 20.
 *      Author: Baram
 */

#ifndef SRC_HW_USB_HID_USB_HID_JOY_H_
#define SRC_HW_USB_HID_USB_HID_JOY_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"
#include "usbh_hid.h"


typedef struct
{
  int8_t l_x;
  int8_t l_y;

  int8_t r_x;
  int8_t r_y;

  uint32_t buttons;

} usb_hid_joy_msg_t;


typedef enum {
  JOY_BTN_L_UP = 0,
  JOY_BTN_L_DOWN,
  JOY_BTN_L_LEFT,
  JOY_BTN_L_RIGHT,

  JOY_BTN_R_UP,
  JOY_BTN_R_DOWN,
  JOY_BTN_R_LEFT,
  JOY_BTN_R_RIGHT,

  JOY_BTN_BACK,
  JOY_BTN_START,
  JOY_BTN_MODE,
  JOY_BTN_VIBRATION,

  JOY_BTN_LT,
  JOY_BTN_LB,

  JOY_BTN_RT,
  JOY_BTN_RB,

} usb_hid_joy_button;



USBH_StatusTypeDef usbHidJoyInit(USBH_HandleTypeDef *phost);
bool usbHidJoyProcess(USBH_HandleTypeDef *phost);

uint32_t usbHidJoyAvailable(void);
void     usbHidJoyRead(usb_hid_joy_msg_t *p_msg);
void usbHidJoyCurrentRead(usb_hid_joy_msg_t *p_msg);

void usbHidJoyDeInit(void);
bool usbHidJoyIsConnected(void);



#ifdef __cplusplus
}
#endif



#endif /* SRC_HW_USB_HID_USB_HID_JOY_H_ */
