/*
 * usb_hid_joystick.c
 *
 *  Created on: 2019. 7. 20.
 *      Author: Baram
 */




#include "usb_hid_joy.h"
#include "usbh_hid_parser.h"
#include "ring.h"


static bool is_connected = false;
static ring_node_t q_rx;
static usb_hid_joy_msg_t q_rx_buffer[16];
static usb_hid_joy_msg_t joy_msg;

static uint32_t joy_report_data[2];



#define USE_LOG     0


static const HID_Report_ItemTypedef joy_data={
  (uint8_t *)(void *)joy_report_data+0, /*data*/
  8,     /*size*/
  0,     /*shift*/
  8,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};





USBH_StatusTypeDef usbHidJoyInit(USBH_HandleTypeDef *phost)
{
  uint32_t x;
  HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData;




  if(HID_Handle->length > (sizeof(joy_report_data)/sizeof(uint32_t)))
  {
    HID_Handle->length = (sizeof(joy_report_data)/sizeof(uint32_t));
  }
  HID_Handle->pData = (uint8_t*)(void *)joy_report_data;
  USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE * sizeof(joy_report_data));


  ringCreate(&q_rx, 16);
  is_connected = true;

  joy_msg.l_x = 0;
  joy_msg.l_y = 0;
  joy_msg.r_x = 0;
  joy_msg.r_y = 0;
  joy_msg.buttons = 0;

  return USBH_OK;
}

void usbHidJoyDeInit(void)
{
  joy_msg.l_x = 0;
  joy_msg.l_y = 0;
  joy_msg.r_x = 0;
  joy_msg.r_y = 0;
  joy_msg.buttons = 0;

  is_connected = false;
}

bool usbHidJoyIsConnected(void)
{
  return is_connected;
}

bool usbHidJoyProcess(USBH_HandleTypeDef *phost)
{
  HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData;
  uint16_t length;
  uint8_t data[8];
  uint8_t button;


  if(HID_Handle->length == 0U)
  {
    return false;
  }

  do
  {
    length = USBH_HID_FifoRead(&HID_Handle->fifo, &joy_report_data, HID_Handle->length);

    /*Fill report */
    if(length == HID_Handle->length)
    {
      for (int i=0; i<8; i++)
      {
        data[i] = (uint32_t)HID_ReadItem((HID_Report_ItemTypedef *) &joy_data, i);
  #if USE_LOG == 1
        printf("%02X ", data[i]);
  #endif
      }
  #if USE_LOG == 1
      printf("\r\n");
  #endif

      uint32_t index;
      index = ringGetWriteIndex(&q_rx);

      q_rx_buffer[index].l_x =  (int8_t)(data[1] - 0x80);
      q_rx_buffer[index].l_y = -(int8_t)(data[2] - 0x7F);

      q_rx_buffer[index].r_x =  (int8_t)(data[3] - 0x80);
      q_rx_buffer[index].r_y = -(int8_t)(data[4] - 0x7F);



      button = data[5] & 0x0F;
      q_rx_buffer[index].buttons = 0;

      if (button == 0 || button == 1 || button == 7) q_rx_buffer[index].buttons |= (1<<JOY_BTN_L_UP);
      if (button == 1 || button == 2 || button == 3) q_rx_buffer[index].buttons |= (1<<JOY_BTN_L_RIGHT);
      if (button == 3 || button == 4 || button == 5) q_rx_buffer[index].buttons |= (1<<JOY_BTN_L_DOWN);
      if (button == 5 || button == 6 || button == 7) q_rx_buffer[index].buttons |= (1<<JOY_BTN_L_LEFT);

      button = data[5];
      if (button & 0x80) q_rx_buffer[index].buttons |= (1<<JOY_BTN_R_UP);
      if (button & 0x40) q_rx_buffer[index].buttons |= (1<<JOY_BTN_R_RIGHT);
      if (button & 0x20) q_rx_buffer[index].buttons |= (1<<JOY_BTN_R_DOWN);
      if (button & 0x10) q_rx_buffer[index].buttons |= (1<<JOY_BTN_R_LEFT);

      button = data[6];
      if (button & 0x10) q_rx_buffer[index].buttons |= (1<<JOY_BTN_BACK);
      if (button & 0x20) q_rx_buffer[index].buttons |= (1<<JOY_BTN_START);
      if (button & 0x01) q_rx_buffer[index].buttons |= (1<<JOY_BTN_LB);
      if (button & 0x04) q_rx_buffer[index].buttons |= (1<<JOY_BTN_LT);
      if (button & 0x02) q_rx_buffer[index].buttons |= (1<<JOY_BTN_RB);
      if (button & 0x08) q_rx_buffer[index].buttons |= (1<<JOY_BTN_RT);

      button = data[7];
      if (button & 0x08) q_rx_buffer[index].buttons |= (1<<JOY_BTN_MODE);
      if (button & 0x20) q_rx_buffer[index].buttons |= (1<<JOY_BTN_VIBRATION);

      ringWriteUpdate(&q_rx);

      joy_msg.l_x = q_rx_buffer[index].l_x;
      joy_msg.l_y = q_rx_buffer[index].l_y;
      joy_msg.r_x = q_rx_buffer[index].r_x;
      joy_msg.r_y = q_rx_buffer[index].r_y;
      joy_msg.buttons = q_rx_buffer[index].buttons;
    }
  } while(length > 0);


  return true;
}

uint32_t usbHidJoyAvailable(void)
{
  uint32_t ret = 0;


  return ret;
}

void usbHidJoyRead(usb_hid_joy_msg_t *p_msg)
{

}

void usbHidJoyCurrentRead(usb_hid_joy_msg_t *p_msg)
{
  *p_msg = joy_msg;
}
