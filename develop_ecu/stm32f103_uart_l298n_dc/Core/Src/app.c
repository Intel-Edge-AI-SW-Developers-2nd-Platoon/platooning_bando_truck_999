/*
 * app.c
 *
 *  Created on: Aug 10, 2023
 *      Author: iot00
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "app.h"
#include "uart.h"
#include "l298n_dc_motor.h"

void app(void);
void app_init(void);
bool app_parser(uint8_t* buf);

void app(void)
{
	app_init();

	//set init_led
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

	while (1)
	{
		uart_proc();
		HAL_Delay(100);
	}
}

void app_init(void)
{
	//set motor init
	l298n_dc_motor_init();

	//uart receive init
	uart_init();
	//link uart input to app_parser
	uart_regcbf(app_parser);
}

#define D_DELIMITER	"@ ,\r\n"
bool app_parser(uint8_t* buf)
{
	int argc = 0;
	uint8_t motor_direction = 0, motor_pwm_1 = 0, motor_pwm_2 = 0;

	char *ptr;
	char *argv[10];

	ptr = buf;
	if(ptr!=NULL)
	{
		ptr = strtok(ptr, D_DELIMITER);

		while(ptr != NULL)
		{
			argv[argc] = ptr;
			argc++;
			ptr = strtok(NULL, D_DELIMITER);
		}

		if(strcmp(argv[0], "CAR_B") == 0)
		{
			motor_direction = atoi(argv[1]);
			motor_pwm_1 = atoi(argv[2]);
			motor_pwm_2 = atoi(argv[3]);
			l298n_dc_motor_movement(motor_direction, motor_pwm_1, motor_pwm_2);
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
		}
	}
}
