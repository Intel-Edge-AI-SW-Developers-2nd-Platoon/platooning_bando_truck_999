/*
 * app.c
 *
 *  Created on: Jul 28, 2023
 *      Author: roby238
 */

#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "cli.h"
#include "uart.h"
//#include "eth.h"

#define DEBUG	1

typedef struct {
  uint32_t counter;
  uint32_t period;
  void (*cbf)(void *);
  bool flag;
} PROC_T;

//static void proc_10ms(void *arg);
//static void proc_50ms(void *arg);
//static void proc_100ms(void *arg);
//static void proc_1s(void *arg);
/*
PROC_T gProcListObj[] = {
    {0,   10,  proc_10ms, false},
    {0,   50,  proc_50ms, false},
    {0,  100, proc_100ms, false},
    {0, 1000,    proc_1s, false},
    {0,    0,       NULL, false}
};
*/
void application_init(void)
{
	HAL_Delay(500);

#if DEBUG
printf("System start....\r\n");
#endif
}

void application(void)
{
	application_init();
	uart_init();
	cli_init();
	eth_init();

	while (1) {
		uart_proc();
		//eth_proc();
	}
}


