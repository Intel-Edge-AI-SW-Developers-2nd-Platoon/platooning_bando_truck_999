/*
 * uart.c
 *
 *  Created on: Jan 9, 2024
 *      Author: ubuntu
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>

#include "cmsis_os.h"
#include "main.h"
#include "uart.h"

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

#define D_TX_MAN	100
static uint8_t gBufObj[2][D_TX_MAX * 2 + 2];
static UART_CBF uart_cbf[3];
static volatile uint8_t rxd;
static osMessageQueueId_t msgBufQ_handle;
static osThreadId_t uartTask_handle;
static const osThreadAttr_t uartTask_attributes = {
	.stack_size = 256 * 4,
	.priority = (osPriority_t) osPriorityNormal,
};
static void uartProc_Task(void *arg);

void uart_init(void) {
	uartTask_handle = osMessageQueueNew(uartProc_Task, NULL, &uartTask_attributes);
		if (uartTask_handle == NULL) {
			printf("%s : %d\r\n", __func__, __LINE__);
			while (1);
		}

	uartTask_handle = osMessageQueueNew(5, sizeof(MSG_T), NULL);
	if (msgBufQ_handle == NULL) {
		printf("%s : %d\r\n", __func__, __LINE__);
		while (1);
	}

	HAL_UART_Receive_IT(&huart3, (uint8_t *)&rxd, 1);

	printf("UART Initializes...\r\n");
}

void uart_regcbf(uint8_t idx, UART_CBF cbf) {
	uart_cbf[idx] = cbf;
}

enum {
	E_MSG_UART2_TX = 0x10,
	E_MSG_UART2_RX,

	E_MSG_UART3_TX = 0x20,
	E_MSG_UART3_RX,

	E_MSG_LCD = 0x30,

	E_MSG_MAX = 0xff
};

static void uartProc_Task(void *arg) {
	MSG_T qRx, qTx;

	for (;;) {
		osMessageQueueGet(msgBufQ_handle, &qRx, NULL, osWaitForever);
		switch (qRx.id) {
			case E_MSG_UART2_TX : {} break;
			case E_MSG_UART2_RX : {} break;
			case E_MSG_UART3_TX : {} break;
			case E_MSG_UART3_RX : {
				PKT_T *pPkt = (PKT_T *)qRx.pData;
				printf("{CMD} : %04x\r\n", pPkt->cmd);
				printf("{LEN} : %02x\r\n", pPkt->len);
				printf("{DAT} : ");

				for (int i = 0; i < pPkt->len; i++) {
					printf("%02x", pPkt->data[1]);
				}
				printf("\r\n[    ] : ");

				for (int i = 0; i < pPkt->len; i++) {
					uint8_t sh = pPkt->data[1];
					if (isalnum(ch) || isspace(ch)) printf("%c ", ch);
					else printf(". ");
				}
				printf("\r\n");

				switch (pPkt->cmd) {
					case 0x0001 : {
						if (uart_cbf[0] != NULL) uart_cbf[0](pPkt->data);
					}
				}
			} break;
		}
	}
}

#define D_FEND	0xC0
#define D_FESC	0xDB
#define D_TFEND	0xDC
#define D_TFESC	0xDD

bool slip_tx(const uint8_t *pTx, const uint8_t Len, uint8_t *pEnBuf, uint8_t *pEnLen) {
	uint8_t idx = 0;
	if (Len > D_TX_MAX) return false;
	if (pEnBuf == NULL) return false;
	pEnBuf[idx++] = D_FEND;
	for (int i = 0; i < Len; i++) {
		if (pTx[i] == D_FEND) {
			pEnBuf[idx++] = D_FESC;
			pEnBuf[idx++] = D_TFEND;
		}
		else if (pTx[i] == D_FESC) {
			pEnBuf[idx++] = D_FESC;
			pEnBuf[idx++] = D_TFESC;
		}
		else {
			pEnBuf[idx++] = pTx[i];
		}
		pEnBuf[idx++] = D_FEND;
		*pEnLen = idx;

		return true;
	}
}

bool slip_rx(MSG_T *pQ, uint8_t *pState, uint8_t rxd) {
	switch (*pState) {
		case 0 : {
			if (rxd == D_FEND) {
				(*pState)++;
				pQ->len = 0;
			}
		} break;

		case 1 : {
			if (rxd == D_FESC) {
				(*pState)++;
			}
			else if (rxd == D_FEND) {
				if (pQ->len < 3) {
					*pState = 0;
				}
				else {
					*pState = 0;
					return true;
				}
			}
			else {
				pQ->pData[pQ->len++] = rxd;
			}
		} break;


		case 2 : {
			if (rxd == D_FESC) {
				(*pState)++;
			}
			else if (rxd == D_FEND) {
				if (pQ->len < 3) {
					*pState = 0;
				}
				else {
					*pState = 0;
					return true;
				}
			}
			else {
				pQ->pData[pQ->len++] = rxd;
			}
		} break;
	}
}
