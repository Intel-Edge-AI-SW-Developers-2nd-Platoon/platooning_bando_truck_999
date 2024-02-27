/*
 * l298d_dc_motor.c
 *
 *  Created on: Jan 9, 2024
 *      Author: USER
 */

#include "main.h"
#include "l298n_dc_motor.h"

extern TIM_HandleTypeDef htim2;

#define IN_GPIO_PORT GPIOA
#define IN1 GPIO_PIN_3
#define IN2 GPIO_PIN_4
#define IN3 GPIO_PIN_5
#define IN4 GPIO_PIN_6
#define MOTOR_PWM_CH1 TIM_CHANNEL_3
#define MOTOR_PWM_CH2 TIM_CHANNEL_4

void l298n_dc_motor_init()
{
	HAL_GPIO_WritePin(IN_GPIO_PORT,IN1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IN_GPIO_PORT,IN2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IN_GPIO_PORT,IN3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IN_GPIO_PORT,IN4, GPIO_PIN_RESET);
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim2, MOTOR_PWM_CH1);
	HAL_TIM_PWM_Start(&htim2, MOTOR_PWM_CH2);
}

void l298n_dc_motor_direction_set(uint8_t motor_direction)
{
	//move straight front
	if(motor_direction == 1)
	{
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN4, GPIO_PIN_RESET);
	}

	//move straight back
	else if(motor_direction == 2)
	{
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN4, GPIO_PIN_RESET);
	}

	//move right front
	else if(motor_direction == 3)
	{
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN4, GPIO_PIN_RESET);
	}

	//move left front
	else if(motor_direction == 4)
	{
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN4, GPIO_PIN_SET);
	}

	//move right back
	else if(motor_direction == 5)
	{
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN4, GPIO_PIN_RESET);
	}

	//move left back
	else if(motor_direction == 6)
	{
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN4, GPIO_PIN_SET);
	}

	//stop
	else
	{
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN_GPIO_PORT,IN4, GPIO_PIN_RESET);
	}
}

void l298n_dc_motor_pwm_set(uint8_t motor_pwm_1, uint8_t motor_pwm_2)
{
	 __HAL_TIM_SET_COMPARE(&htim2, MOTOR_PWM_CH1, motor_pwm_1);
	 __HAL_TIM_SET_COMPARE(&htim2, MOTOR_PWM_CH2, motor_pwm_2);
}

void l298n_dc_motor_movement(uint8_t movement, uint8_t motor_pwm_1, uint8_t motor_pwm_2)
{
	l298n_dc_motor_direction_set(movement);
	l298n_dc_motor_pwm_set(motor_pwm_1, motor_pwm_2);
}
