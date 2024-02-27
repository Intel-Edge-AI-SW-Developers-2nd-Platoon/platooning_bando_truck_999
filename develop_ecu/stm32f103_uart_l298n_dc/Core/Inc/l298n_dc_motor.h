/*
 * l298d_dc_motor.h
 *
 *  Created on: Jan 9, 2024
 *      Author: USER
 */

#ifndef INC_L298N_DC_MOTOR_H_
#define INC_L298N_DC_MOTOR_H_
#include <stdbool.h>
#include <stdint.h>

#ifdef __cpluspluse
extern "C" {
#endif

void l298n_dc_motor_init(void);
void l298n_dc_motor_movement(uint8_t, uint8_t, uint8_t);

#ifdef __cplusplus
}
#endif

#endif /* INC_L298N_DC_MOTOR_H_ */
