/*
 * eth.h
 *
 *  Created on: Jan 13, 2024
 *      Author: roby238
 */

#ifndef INC_ETH_H_
#define INC_ETH_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"

typedef bool (*ETH_CBF)(uint8_t *);

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_uart4_rx;

#define DEBUG 					1

#define ETH_RX_BUF_SIZE 		128
#define FULL_ETH_RX_BUF_SIZE 	256
#define ETH_LOGIN_BUF_SIZE		16
#define ETH_SEND_BUF_SIZE		256

static uint8_t eth_local_id_buf[] 	= "18";
static uint8_t eth_local_pw_buf[] 	= "PASSWD";
static uint8_t eth_login_buf[ETH_LOGIN_BUF_SIZE];
static uint8_t eth_dest_id_buf[] 	= "24";
static uint8_t eth_send_buf[ETH_SEND_BUF_SIZE+1];


static uint16_t old_pos = 0;
static uint16_t new_pos = 0;
static uint8_t eth_rx_buf[ETH_RX_BUF_SIZE];
static uint8_t full_eth_rx_buf[FULL_ETH_RX_BUF_SIZE];

static ETH_CBF eth_cbf;

uint8_t eth_init(void);
void eth_uart_cbf(uint16_t Size);
void eth_uart_irq_handler(void);
void eth_read(uint16_t Size);
uint8_t eth_write(uint8_t *buf, uint16_t len);
uint8_t eth_login(uint8_t *id, uint8_t *pw);
uint8_t eth_send_data(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* INC_ETH_H_ */
