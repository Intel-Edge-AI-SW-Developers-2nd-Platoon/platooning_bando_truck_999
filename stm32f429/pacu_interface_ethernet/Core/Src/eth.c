/*
 * eth.c
 *
 *  Created on: Jan 13, 2024
 *      Author: roby238
 */

#include "eth.h"

/******************************************************************************
function:	eth_init
parameter:
Info:  Initialize ethrnet settings and ch9121
******************************************************************************/
uint8_t eth_init(void) {

#if DEBUG
printf("Ethernet Initializing...\r\n");
#endif

	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;

	ch9121_set_conf();

	if (HAL_UART_Init(&huart4) != HAL_OK) {
		Error_Handler();
	}

	HAL_UARTEx_ReceiveToIdle_DMA(&huart4, eth_rx_buf, ETH_RX_BUF_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);

	sprintf((uint8_t *)eth_login_buf, "[%s:%s]", eth_local_id_buf, eth_local_pw_buf);
	eth_write(eth_login_buf, strlen((uint8_t *)eth_login_buf));

	return 0;
}

/******************************************************************************
function:	eth_regcbf
parameter:
	cbf: function pointer
Info:  register function pointer
******************************************************************************/
uint8_t eth_regcbf(ETH_CBF cbf) {
	eth_cbf = cbf;
	return 0;
}

/******************************************************************************
function:	eth_read
parameter:
	size: size of data received
Info:  Write data to ch9121(ethernet module) with uart
******************************************************************************/
void eth_read(uint16_t size) {
	old_pos = new_pos;
	if (old_pos + size > FULL_ETH_RX_BUF_SIZE) {
		uint16_t data_to_copy = FULL_ETH_RX_BUF_SIZE - old_pos;
		memcpy ((uint8_t *)full_eth_rx_buf + old_pos, eth_rx_buf, data_to_copy);
		old_pos = 0;
		memcpy ((uint8_t *)full_eth_rx_buf, (uint8_t *)eth_rx_buf + data_to_copy, (size - data_to_copy));
		new_pos = (size - data_to_copy);
	}
	else {
		memcpy ((uint8_t *)full_eth_rx_buf + old_pos, eth_rx_buf, size);
		new_pos = size + old_pos;
	}

	HAL_UARTEx_ReceiveToIdle_DMA(&huart4, (uint8_t *) eth_rx_buf, ETH_RX_BUF_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);

#if DEBUG
printf("%s: %s\r\n", __func__, eth_rx_buf);
#endif

}

/******************************************************************************
function:	eth_write
parameter:
	buf: string data
	len: length of string data
Info:  Write data to ch9121(ethernet module) with uart
******************************************************************************/
uint8_t eth_write(uint8_t *buf, uint16_t len) {
	if(HAL_UART_Transmit_IT(&huart4, (uint8_t *)buf, strlen((uint8_t *)buf))!= HAL_OK) {
		printf("FAIL TO TRANSMIT..\r\n");
		return 1;
	}
	return 0;
}

/******************************************************************************
function:	eth_send_data
parameter:
	buf: string data
Info:  Send data to destination
******************************************************************************/
uint8_t eth_send_data(uint8_t *buf) {
	if (!strlen(buf)) {
		printf("Send data is empty..\r\n");
		return 1;
	}
	sprintf((uint8_t *)eth_send_buf, "[%s]%s", eth_dest_id_buf, buf);
	if(eth_write(eth_send_buf, strlen((char *)eth_send_buf))) {
		printf("Fail to send data..\r\n");
		return 1;
	}
	return 0;
}
