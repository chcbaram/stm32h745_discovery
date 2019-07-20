/*
 * usb_hid.c
 *
 *  Created on: 2019. 7. 19.
 *      Author: Baram
 */



#include "usbh_core.h"
#include "usbh_hid.h"
#include "usbh_hid_parser.h"

#include "usb_hid.h"
#include "usb_hid_joy.h"
#include "uart.h"


typedef enum {
  USB_HID_DEV_IDLE = 0,
  USB_HID_DEV_DISCONNECT,
  USB_HID_DEV_START,
  USB_HID_DEV_READY,
  USB_HID_DEV_RUNNING,
} usb_hid_dev_t;

typedef enum {
  USB_HID_APP_IDLE = 0,
  USB_HID_APP_WAIT,
  USB_HID_APP_START,
  USB_HID_APP_MOUSE,
  USB_HID_APP_KEYBOARD,
  USB_HID_APP_JOYSTICK,
  USB_HID_APP_REENUMERATE,
} usb_hid_app_t;



USBH_HandleTypeDef hUSB_Host;



static usb_hid_dev_t usb_hid_dev_state = USB_HID_DEV_IDLE;
static usb_hid_app_t usb_hid_app_state = USB_HID_APP_IDLE;
static osMessageQId  app_event;


extern HCD_HandleTypeDef hhcd;


static void usbHidProcess(USBH_HandleTypeDef * phost, uint8_t id);
static void usbHidThread(void const *argument);


bool usbHidInit(void)
{
  /* Enable the USB voltage level detector */
  HAL_PWREx_EnableUSBVoltageDetector();

  return true;
}

bool usbHidStart(void)
{

  /* Init Host Library */
  USBH_Init(&hUSB_Host, usbHidProcess, 0);

  /* Add Supported Class */
  USBH_RegisterClass(&hUSB_Host, USBH_HID_CLASS);

  /* Start Host Process */
  USBH_Start(&hUSB_Host);


  osSemaphoreDef(osSem);
  app_event = osSemaphoreCreate(osSemaphore(osSem), 1);
  osSemaphoreRelease(app_event);

  /* Start task */
  osThreadDef(usbHidThread, usbHidThread, osPriorityNormal, 0, 8 * configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(usbHidThread), NULL);


  return true;
}

static void usbHidThread(void const *argument)
{
  for (;;)
  {
    if (osSemaphoreWait(app_event, osWaitForever) == osOK)
    {
      switch (usb_hid_app_state)
      {
        case USB_HID_APP_IDLE:
          usb_hid_app_state = USB_HID_APP_WAIT;
          osSemaphoreRelease(app_event);
          break;

        case USB_HID_APP_WAIT:
          usb_hid_app_state = USB_HID_APP_START;
          osSemaphoreRelease(app_event);
          break;

        case USB_HID_APP_START:
          if (usb_hid_dev_state == USB_HID_DEV_READY)
          {
            if (USBH_HID_GetDeviceType(&hUSB_Host) == HID_JOYSTICK)
            {
              usb_hid_app_state = USB_HID_APP_JOYSTICK;
              usbHidJoyInit(&hUSB_Host);
            }
          }
          else
          {
            usb_hid_app_state = USB_HID_APP_WAIT;
          }
          osSemaphoreRelease(app_event);
          break;

        case USB_HID_APP_REENUMERATE:
          USBH_ReEnumerate(&hUSB_Host);
          usb_hid_app_state = USB_HID_APP_WAIT;
          osSemaphoreRelease(app_event);
          break;

        case USB_HID_APP_JOYSTICK:
          if (usb_hid_dev_state == USB_HID_DEV_READY)
          {
            usbHidJoyProcess(&hUSB_Host);
          }
          break;

        default:
          break;
      }

      if (usb_hid_dev_state == USB_HID_DEV_DISCONNECT)
      {
        usb_hid_dev_state = USB_HID_DEV_IDLE;
        usb_hid_app_state = USB_HID_APP_IDLE;
        usbHidJoyDeInit();
      }
    }
  }
}

static void usbHidProcess(USBH_HandleTypeDef * phost, uint8_t id)
{
  switch (id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
    break;

  case HOST_USER_DISCONNECTION:
    usb_hid_dev_state = USB_HID_DEV_DISCONNECT;
    osSemaphoreRelease(app_event);
    break;

  case HOST_USER_CLASS_ACTIVE:
    usb_hid_dev_state = USB_HID_DEV_READY;
    break;

  case HOST_USER_CONNECTION:
    usb_hid_dev_state = USB_HID_DEV_START;
    break;

  default:
    break;
  }
}


void USBH_HID_EventCallback(USBH_HandleTypeDef * phost)
{
  osSemaphoreRelease(app_event);
}

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  HAL_HCD_IRQHandler(&hhcd);
}
