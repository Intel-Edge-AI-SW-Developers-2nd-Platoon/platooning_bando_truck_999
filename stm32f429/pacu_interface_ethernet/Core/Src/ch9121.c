/*
 * ch9121.c
 *
 *  Created on: Jan 16, 2024
 *      Author: roby238
 */

#include "ch9121.h"

/******************************************************************************
function:	ch9121_set_conf
parameter:
Info:  Set ch9121 configuration
******************************************************************************/
void ch9121_set_conf(void) {
	HAL_GPIO_WritePin(RST1_GPIO_Port, RST1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CFG0_GPIO_Port, CFG0_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);

	ch9121_send_4_bytes(CH9121_MODE, MODE1); 						// Mode
	ch9121_send_7_bytes(CH9121_LOCAL_IP, LOCAL_IP); 				// LOCAL IP
	ch9121_send_7_bytes(CH9121_SUBNET_MASK, SUBNET_MASK); 			// SUBNET MASK
	ch9121_send_7_bytes(CH9121_GATEWAY, GATEWAY); 					// GATEWAY
	ch9121_send_7_bytes(CH9121_TARGET_IP, TARGET_IP1); 				// TARGET IP
	ch9121_send_4_bytes(CH9121_RANDOM_PORT, PORT_RANDOM_ENABLE1); 	// Random port on-off
	ch9121_send_5_bytes(CH9121_PORT1, LOCAL_PORT1);               	// Local port
	ch9121_send_5_bytes(CH9121_TARGET_PORT, TARGET_PORT1); 			// Target Port
	ch9121_send_baud(CH9121_BAUD_RATE, UART1_BAUD1); 				// Port 1 baud rate
	ch9121_update_conf();

	HAL_GPIO_WritePin(CFG0_GPIO_Port, CFG0_Pin, GPIO_PIN_SET);
}

/******************************************************************************
function:	ch9121_send_4_bytes
parameter:
    data: parameter
    command: command code
Info:  Set mode, enable port, clear serial port, switch DHCP, switch port 2
******************************************************************************/
void ch9121_send_4_bytes(uint8_t data, int command) {
    for (int i = 2; i < 4; i++) {
        if (i == 2)
        	ch9121_tx_buf[i] = command;
        else
        	ch9121_tx_buf[i] = data;
    }
    HAL_Delay(10);
    for (int o = 0; o < 4; o++)
        HAL_UART_Transmit(&huart4, &ch9121_tx_buf[o], 1, 1);
    HAL_Delay(10);
    for (int i = 2; i < 4; i++)
    	ch9121_tx_buf[i] = 0;
    HAL_Delay(200);
}

/******************************************************************************
function:	ch9121_send_5_bytes
parameter:
    data: parameter
    command: command code
Info:  Set the local port and target port
******************************************************************************/
void ch9121_send_5_bytes(uint16_t data, int command) {
    uint8_t Port[2];
    Port[0] = data & 0xff;
    Port[1] = data >> 8;
    for (int i = 2; i < 5; i++)
    {
        if (i == 2)
        	ch9121_tx_buf[i] = command;
        else
        	ch9121_tx_buf[i] = Port[i - 3];
    }
    HAL_Delay(10);
    for (int o = 0; o < 5; o++)
        HAL_UART_Transmit(&huart4, &ch9121_tx_buf[o], 1, 1);
    HAL_Delay(10);
    for (int i = 2; i < 5; i++) ch9121_tx_buf[i] = 0;
    HAL_Delay(100);
}

/******************************************************************************
function:	ch9121_send_7_bytes
parameter:
    data: parameter
    command: command code
Info:  Set the IP address, subnet mask, gateway,
******************************************************************************/
void ch9121_send_7_bytes(uint8_t data[], int command) {
    for (int i = 2; i < 7; i++) {
        if (i == 2)
        	ch9121_tx_buf[i] = command;
        else
        	ch9121_tx_buf[i] = data[i - 3];
    }
    HAL_Delay(10);
    for (int o = 0; o < 7; o++)
        HAL_UART_Transmit(&huart4, &ch9121_tx_buf[o], 1, 1);
    HAL_Delay(10);
    for (int i = 2; i < 7; i++) ch9121_tx_buf[i] = 0;
    HAL_Delay(100);
}

/******************************************************************************
function:	ch9121_send_baud
parameter:
    data: parameter
    command: command code
Info:  Set baud rate
******************************************************************************/
void ch9121_send_baud(uint32_t data, int command) {
    uint8_t Port[4];
    Port[0] = (data & 0xff);
    Port[1] = (data >> 8) & 0xff;
    Port[2] = (data >> 16) & 0xff;
    Port[3] = data >> 24;

    for (int i = 2; i < 7; i++) {
        if (i == 2)
        	ch9121_tx_buf[i] = command;
        else
        	ch9121_tx_buf[i] = Port[i - 3];
    }
    HAL_Delay(10);
    for (int o = 0; o < 7; o++)
        HAL_UART_Transmit(&huart4, &ch9121_tx_buf[o], 1, 1);
    HAL_Delay(10);
    for (int i = 2; i < 7; i++) ch9121_tx_buf[i] = 0;
    HAL_Delay(100);
}

/******************************************************************************
function:	ch9121_update_conf
parameter:
Info:  Updating configuration Parameters
******************************************************************************/
void ch9121_update_conf(void) {
	ch9121_tx_buf[2] = 0x0d;
    for (int o = 0; o < 3; o++)
        HAL_UART_Transmit(&huart4, &ch9121_tx_buf[o], 1, 1);
    HAL_Delay(200);
    ch9121_tx_buf[2] = 0x0e;
    for (int o = 0; o < 3; o++)
        HAL_UART_Transmit(&huart4, &ch9121_tx_buf[o], 1, 1);
    HAL_Delay(200);
    ch9121_tx_buf[2] = 0x5e;
    for (int o = 0; o < 3; o++)
        HAL_UART_Transmit(&huart4, &ch9121_tx_buf[o], 1, 1);
    HAL_Delay(100);
}
