#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "main.h"
#include "uart.h"

extern UART_HandleTypeDef huart1;

#define D_BUF_MAX	100
typedef struct {
    uint8_t buf[D_BUF_MAX + 1];
    uint8_t idx;
    uint8_t flag;
} BUF_T;

BUF_T gBufObj[2];

static UART_CBF uart_cbf;
volatile uint8_t rxd;

void uart_init(void)
{
    gBufObj[0].idx = 0;
    gBufObj[0].flag = false;

    HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxd, 1);

    printf("UART Initialized...\r\n");
}

void uart_regcbf(UART_CBF cbf)
{
    uart_cbf = cbf;
}

// non-blocking procedure routine
void uart_proc(void)
{
    BUF_T* p = (BUF_T*)&gBufObj[0];

    if (p->flag == true) {
        //printf("%s:%s", __func__, p->buf);
        // uart parsing
        if (uart_cbf != NULL) uart_cbf(p->buf);

        p->idx = 0;
        p->flag = false;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart == &huart1) {
        BUF_T* p = (BUF_T*)&gBufObj[0];

        if (p->flag == false) {
            p->buf[p->idx] = rxd;
            //p->idx++;
            //p->idx %= D_BUF_MAX;
            if (p->idx < D_BUF_MAX) {
                p->idx++;
            }

            if (rxd == '\r' || rxd == '\n') {
                p->buf[p->idx] = '\0';
                p->flag = true;
            }
        }

        HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxd, 1);
    }
}


int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xffff);
    return ch;
}
