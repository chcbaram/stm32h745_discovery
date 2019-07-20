/*
 * usb_hid.h
 *
 *  Created on: 2019. 7. 19.
 *      Author: Baram
 */

#ifndef SRC_HW_USB_HID_USB_HID_H_
#define SRC_HW_USB_HID_USB_HID_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


bool usbHidInit(void);
bool usbHidStart(void);


#ifdef __cplusplus
 }
#endif



#endif /* SRC_HW_USB_HID_USB_HID_H_ */
