/*

888888b.                     888     888           .d888         888      888               888 
888  "88b                    888     888          d88P"          888      888               888 
888  .88P                    888     888          888            888      888               888 
8888888K.   .d88b.  888  888 888     888 88888b.  888888 .d88b.  888  .d88888  .d88b.   .d88888 
888  "Y88b d88""88b `Y8bd8P' 888     888 888 "88b 888   d88""88b 888 d88" 888 d8P  Y8b d88" 888 
888    888 888  888   X88K   888     888 888  888 888   888  888 888 888  888 88888888 888  888 
888   d88P Y88..88P .d8""8b. Y88b. .d88P 888  888 888   Y88..88P 888 Y88b 888 Y8b.     Y88b 888 
8888888P"   "Y88P"  888  888  "Y88888P"  888  888 888    "Y88P"  888  "Y88888  "Y8888   "Y88888 
File:       application.h

Abstract:





Version:    <0.1>

Created: 8/15/13

Author: George W. Melcer

Copyright ( C ) 2015 BoxUnfolded LLC. All Rights Reserved.



*/


#ifndef LAUNDRY_PAY_
#define LAUNDRY_PAY_


#include "stdint.h"
#include <stdbool.h>
#include "nrf_drv_gpiote.h"
#include "deviceinfo.h"







#define GET_HARDWARE_REV_CMD_ID								0xB2
#define GET_FIRMWARE_BUILD_PACKET_ID						0xB1
#define GET_SERIAL_NUMBER_PACKET_ID							0x0E
#define GET_DEVICE_ID_PACKET_ID								0x0D
#define AUTHORIZE_MACHINE_PACKET_ID							0xA6
#define SEND_QUARTER_COUNT_CMD_ID        					0xA7
#define RESET_QUARTER_COUNT_CMD_ID        					0xA8
#define SET_PULSE_PERIOD_CMD_ID        						0xA3
#define GET_PULSE_PERIOD_CMD_ID        						0xA2
#define ENABLE_AUTO_SEND_CMD_ID								0xA1
#define DISABLE_AUTO_SEND_CMD_ID							0xA0
#define ACTIVATION_CONFIRMATION_CMD_ID						0xA9
#define INIT_ACTIVIATION_CMD_ID								0xAA

 typedef struct packet_data
{         
	uint8_t 				 command_code;
	uint8_t					 data[4];
} packet_t;




packet_t decode_packet(uint8_t* data, uint32_t counter);
void send_init_activation_cmd(void);
void increment_quarter_count_only(void);



void increment_quarter_count(void);
void releaseMutex(void);
void holdMutex(void);
#endif


