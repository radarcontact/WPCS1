
/*
 
888888b.                     888     888           .d888         888      888               888 
888  "88b                    888     888          d88P"          888      888               888 
888  .88P                    888     888          888            888      888               888 
8888888K.   .d88b.  888  888 888     888 88888b.  888888 .d88b.  888  .d88888  .d88b.   .d88888 
888  "Y88b d88""88b `Y8bd8P' 888     888 888 "88b 888   d88""88b 888 d88" 888 d8P  Y8b d88" 888 
888    888 888  888   X88K   888     888 888  888 888   888  888 888 888  888 88888888 888  888 
888   d88P Y88..88P .d8""8b. Y88b. .d88P 888  888 888   Y88..88P 888 Y88b 888 Y8b.     Y88b 888 
8888888P"   "Y88P"  888  888  "Y88888P"  888  888 888    "Y88P"  888  "Y88888  "Y8888   "Y88888 
File:       application.c
 
Abstract:
 
 
 
 
 
Version:    <0.1>
 
Created: 8/15/13
 
Author: George W. Melcer
 
Copyright ( C ) 2015 BoxUnfolded LLC. All Rights Reserved.
 
 
 
*/
 
 
#include "application.h"
//#include "softdevice_handler.h"
 
#include "misc.h"
#include "ble_lbs.h"
#include <stdbool.h>
#include "nrf_gpio.h"
#include "nrf_adc.h"
#include "nrf_soc.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "main.h"
#include "pins.h"
 #include "nrf_log.h"
#include "nrf_log_ctrl.h"
extern ble_lbs_t                        b_ble;



 static uint16_t quarterCount = 0;
 static uint8_t pulseDurationms = 25;

uint8_t numberOfQuarterPulses = 0;
bool enableLED =false;

static bool isAutoSendEnabled = false;



void send_hardware_rev(void)
{
    create_and_send_packet(&b_ble, GET_HARDWARE_REV_CMD_ID, HARDWARE_REV);
 
}
 


void send_firmware_build(void)
{
    create_and_send_packet(&b_ble, GET_FIRMWARE_BUILD_PACKET_ID, FIRMWARE_RELEASE_BUILD_NUMBER);
 
}
 

 
void send_serial_number(void)
{
    create_and_send_packet(&b_ble, GET_SERIAL_NUMBER_PACKET_ID, SERIAL_NUMBER);
 
}
 

 void enableLEDWithNumberOfPulses(uint8_t pulseCount)
 {
          numberOfQuarterPulses = 5;
          enableLED = true;

 }
 
 
 
void send_device_id(void)
{
    create_and_send_packet(&b_ble, GET_DEVICE_ID_PACKET_ID, DEVICE_ID);
 
}
 
 

 
 
 

void send_quarter_count(void)
{
create_and_send_packet(&b_ble, SEND_QUARTER_COUNT_CMD_ID, quarterCount);

} 



void reset_quarter_count(void)
{
  quarterCount = 0;

}


 
void set_pulse_duration(uint8_t msDuration)
{
 
  pulseDurationms = msDuration;
}
 

 
void get_pulse_duration(void)
{
 
  create_and_send_packet(&b_ble, GET_PULSE_PERIOD_CMD_ID, pulseDurationms);
}
 

void send_activation_confirmation(void)
{
 
  create_and_send_packet(&b_ble, ACTIVATION_CONFIRMATION_CMD_ID, (uint32_t)quarterCount);
}
 

 void send_init_activation_cmd(void)
{
 
  create_and_send_packet(&b_ble, INIT_ACTIVIATION_CMD_ID, 0);
}
 



void increment_quarter_count_only(void)
{
  quarterCount++;
}




void increment_quarter_count(void)
{
 
    
    
    numberOfQuarterPulses = 0;
    pulseLEDs(5);  //pulses the leds
    nrf_gpio_pin_set(PIN_OUT);
    pulse_pin_out();
    
    if(isAutoSendEnabled)
    {
      send_quarter_count(); 
    }
  
}
 
 
 
 void enable_auto_send(void)
 {
    isAutoSendEnabled = true;

 }
 
 
  void disable_auto_send(void)
 {
    isAutoSendEnabled = true;

 }



 
packet_t decode_packet(uint8_t* data, uint32_t counter)
{   
 
    packet_t packet;
    uint8_t i;
 
    packet.command_code = data[0];
    for(i=1; i<4; i++)
        packet.data[i-1] = data[i];
 
 
 
    switch (packet.command_code)
    {
         
 
       case GET_DEVICE_ID_PACKET_ID:
       {
           send_device_id();
           break;
        }
        case GET_FIRMWARE_BUILD_PACKET_ID:
        {
          send_firmware_build();
          break;
        }
       case GET_SERIAL_NUMBER_PACKET_ID:
       {
           send_serial_number();
           break;
        }
      case AUTHORIZE_MACHINE_PACKET_ID:
      {

          NRF_LOG_INFO("Authorize Machine With %u Pulses!\r\n", data[1]);
           pulseLEDs(5);
           //numberOfQuarterPulses = data[1];
           numberOfQuarterPulses = 1;
           
          pulse_pin_out();
          send_activation_confirmation();

          

          break;
      }
      case SEND_QUARTER_COUNT_CMD_ID:
      {
        send_quarter_count();
        break;
      }
      case RESET_QUARTER_COUNT_CMD_ID:
      {
        reset_quarter_count();
        break;
      }
      case SET_PULSE_PERIOD_CMD_ID:
      {
        set_pulse_duration(data[1]);
        break;
      }
      case GET_PULSE_PERIOD_CMD_ID:
      {
        get_pulse_duration();
        break;
      }
      case ENABLE_AUTO_SEND_CMD_ID:
      {
        enable_auto_send();
        break;
      }
      case DISABLE_AUTO_SEND_CMD_ID:
      {
        disable_auto_send();
        break;
      }
      case GET_HARDWARE_REV_CMD_ID:
      {
        send_hardware_rev();
        break;
      }


         default:
            break;
 
    };
 
 
    return packet;
}