/*
 * ch9121.h
 *
 *  Created on: Jan 16, 2024
 *      Author: roby238
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "main.h"

#ifndef INC_CH9121_H_
#define INC_CH9121_H_

#define CH9121_TX_BUF_SIZE 	  	8

#define MODE1 					0x10    // Port 1: Setup Mode
										// 0x00:TCP Server
										// 0x01:TCP Client
										// 0x02:UDP Server
										// 0x03:UDP Client
#define LOCAL_IP 				0x11    // Local IP
#define SUBNET_MASK 			0x12    // Subnet Mask
#define GATEWAY 				0x13    // Gateway
#define LOCAL_PORT1 			0X14    // Port 1:Local Port
#define TARGET_IP1 				0x15    // Port 1:Target IP
#define TARGET_PORT1 			0x16    // Port 1:Target Port
#define PORT_RANDOM_ENABLE1 	0x17 	// Port 1:Port Random Enable
#define UART1_BAUD1 			0x21    // Port 1:Baud rate of serial port 1

#define ON_OFF_UART2 			0X39    // Port 2: Enable/disable port 2
#define MODE2					0x40    // Port 2: Setup Mode
										// 0x00:TCP Server
										// 0x01:TCP Client
										// 0x02:UDP Server
										// 0x03:UDP Client
#define LOCAL_PORT2 			0X41    // Port 2:Local Port
#define TARGET_IP2				0x42    // Port 2:Target IP
#define TARGET_PORT2 			0x43    // Port 2:Target Port
#define UART1_BAUD2 			0x44    // Port 2:Baud rate of serial port 2
#define PORT_RANDOM_ENABLE2 	0x17 	// Port 2:Port Random Enable

extern UART_HandleTypeDef huart4;

uint8_t CH9121_MODE 			= 1;						// 0:TCP_SERVER
															// 1:TCP_CLIENT
															// 2:UDP_SERVER
															// 3:UDP_CLIENT
uint8_t CH9121_LOCAL_IP[4] 		= { 10,  10, 141, 203};   	// LOCAL IP
uint8_t CH9121_GATEWAY[4] 		= { 10,  10, 141, 254};     // GATEWAY
uint8_t CH9121_SUBNET_MASK[4] 	= {255, 255, 255,   0}; 	// SUBNET MASK
uint8_t CH9121_TARGET_IP[4] 	= { 10,  10, 141,  26};   	// TARGET_IP
uint8_t CH9121_RANDOM_PORT 		= 0;                     	// Random port    1:on   0:off
uint16_t CH9121_PORT1 			= 3000;                     // LOCAL PORT1
uint16_t CH9121_TARGET_PORT 	= 5000;                 	// TARGET PORT
uint32_t CH9121_BAUD_RATE 		= 115200;                 	// BAUD RATE

static uint8_t ch9121_tx_buf[CH9121_TX_BUF_SIZE] = {0x57, 0xAB};

void ch9121_set_conf(void);
void ch9121_send_4_bytes(uint8_t data, int command);
void ch9121_send_5_bytes(uint16_t data, int command);
void ch9121_send_7_bytes(uint8_t data[], int command);
void ch9121_send_baud(uint32_t data, int command);
void ch9121_update_conf(void);

#endif /* INC_CH9121_H_ */
