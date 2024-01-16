/*
 * cli.h
 *
 *  Created on: Jul 28, 2023
 *      Author: roby238
 */

#ifndef INC_CLI_H_
#define INC_CLI_H_

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart4;

void cli_init(void);
int cli_help(int argc, char *argv[]);
int cli_echo(int argc, char *argv[]);
int cli_eth(int argc, char *argv[]);
bool cli_parser(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* INC_CLI_H_ */
