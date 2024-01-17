/*
 * uart.h
 *
 *  Created on: Jul 28, 2023
 *      Author: roby238
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "main.h"

#ifndef INC_UART_H_
#define INC_UART_H_

typedef bool (*UART_CBF)(uint8_t *);

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart3;

uint8_t uart_init(void);
uint8_t uart_regcbf(UART_CBF cbf);
uint8_t uart_proc(void);

#define DEBUG		1
#define D_BUF_MAX	100

typedef struct {
  uint8_t buf[D_BUF_MAX+1];
  uint8_t idx;
  uint8_t flag;
} BUF_T;

static BUF_T gBufObj[2];

static UART_CBF uart_cbf;
static volatile uint8_t rxd;


#ifdef __cplusplus
}
#endif

#endif /* INC_UART_H_ */
