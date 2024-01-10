/*
 * uart.h
 *
 *  Created on: Jan 9, 2024
 *      Author: ubuntu
 */

#ifndef INC_UART_H_
#define INC_UART_H_

typedef bool (*UART_CBF) (uint8_t *);
typedef struct __PACKED {
	uint8_t id;
	uint8_t len;
	uint8_t *pData;
} MSG_T;
typedef struct __PACKED {
	uint16_t cmd;
	uint8_t len;
	uint8_t data[1];
} PKT_T;

void uart_init(void);
void uart_regcbf(UART_CBF cbf);
bool slip_tx(const uint8_t *pTx, const uint8_t Len, uint8_t *pEnBuf, uint8_t *pEnLen);

#endif /* INC_UART_H_ */
