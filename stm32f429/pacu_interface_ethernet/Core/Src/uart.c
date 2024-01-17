/*
 * uart.c
 *
 *  Created on: Jul 28, 2023
 *      Author: roby238
 */

#include "uart.h"

uint8_t uart_init(void) {
	HAL_Delay(100);

#if DEBUG
printf("UART Initializing...\r\n");
#endif

	if (HAL_UART_Init(&huart3) != HAL_OK) {
		Error_Handler();
	}

	gBufObj[0].idx = 0;
	gBufObj[0].flag = false;

	if (HAL_UART_Receive_IT(&huart3, (uint8_t *)&rxd, 1) != HAL_OK) {
		return 1;
	}

	return 0;
}

uint8_t uart_regcbf(UART_CBF cbf) {
	uart_cbf = cbf;
	return 0;
}

// non-blocking procedure routine
uint8_t uart_proc(void) {
	BUF_T *p = (BUF_T *)&gBufObj[0];
	if (p->flag == true) {

#if DEBUG
printf("%s: %s", __func__, p->buf);
#endif

		// uart parsing
		if (uart_cbf != NULL) uart_cbf(p->buf);
		p->idx = 0;
		p->flag = false;
	}
	return 0;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart4) {
		;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart4) {
		;
	}
	else if (huart == &huart3) {
		BUF_T *p = (BUF_T *)&gBufObj[0];
		if (p->flag == false) {
			p->buf[p->idx] = rxd;
			if (p->idx < D_BUF_MAX) p->idx++;
			if (rxd == '\r' || rxd == '\n') {
				p->buf[p->idx] = '\0';
				p->flag = true;
			}
		}
		HAL_UART_Receive_IT(&huart3, (uint8_t *)&rxd, 1);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
	if (huart->Instance == UART4) {
		eth_read(size);
	}
}

int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xffff);
	return ch;
}
