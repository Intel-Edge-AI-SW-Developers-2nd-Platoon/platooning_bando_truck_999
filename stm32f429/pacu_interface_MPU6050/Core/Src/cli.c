/*
 * cli.c
 *
 *  Created on: Jul 28, 2023
 *      Author: iot00
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "uart.h"
#include "cli.h"
//#include "led.h"
//#include "pwm.h"
#define DEBUG 1

extern UART_HandleTypeDef huart2;
//extern UART_HandleTypeDef huart3;

typedef struct {
  char *cmd;
  uint8_t no;
  int (*cbf)(int, char **);
  char *remark;
} CMD_LIST_T;

static int cli_help(int argc, char *argv[]);
static int cli_echo(int argc, char *argv[]);
//static int cli_mpu(int argc, char *argv[]);

const CMD_LIST_T gCmdListObj[] = {
    {"help", 1, cli_help,    "help"},
    {"echo", 2, cli_echo,    "echo [echo data]"},
	//{"mpu",	 2, cli_mpu, 	 "mpu6050 [start char]"},
    {  NULL, 0,     NULL,   NULL}
};

static int cli_help(int argc, char *argv[]){
  if (argc != gCmdListObj[0].no){
    printf("It is Not correct usage: argument error\r\n");
    return 0;
  }
  if(strcmp(gCmdListObj[0].cmd , "help") == 0)
          printf("%s\r\n", gCmdListObj[0].remark);
  return 0;
}

static int cli_echo(int argc, char *argv[]){
  if (argc != gCmdListObj[1].no){
      printf("It is Not correct usage: argument error\r\n");
      return 0;
  }
  if(strcmp(gCmdListObj[1].cmd , "echo") == 0)
          printf("%s\r\n", gCmdListObj[1].remark);
  return 0;
}

bool cli_parser(uint8_t *buf);

void cli_init(void){
	uart_regcbf(cli_parser);
}

#define D_DELIMITER  " ,\r\n"

bool cli_parser(uint8_t *buf){
#if DEBUG
	int argc = 0;
	char *argv[10];
	char *ptr;
	ptr = strtok((char *)buf, D_DELIMITER);
	if(ptr == NULL) return false;

	while(ptr != NULL){
	  printf("%s\r\n", ptr);
	  argv[argc] = ptr;
	  argc++;
	  ptr = strtok(NULL, D_DELIMITER);
	}

	for(int i = 0; gCmdListObj[i].cmd != NULL;i++){
	  if(strcmp(gCmdListObj[i].cmd, argv[0]) == 0){
	  gCmdListObj[i].cbf(argc, argv);
	  return true;
	  }
	  else if(strcmp(gCmdListObj[i].cmd, argv[1]) == 0){
	  gCmdListObj[i].cbf(argc, argv);
	  return true;
	  }
	  else if(strcmp(gCmdListObj[i].cmd, argv[2]) == 0){
	  gCmdListObj[i].cbf(argc, argv);
	  return true;
	  }
	}

	printf("Unsupported command. \r\n");

#else
	char *ptr = strtok(buf, " ");    //첫번째 strtok 사용.
	while (ptr != NULL)              //ptr이 NULL일때까지 (= strtok 함수가 NULL을 반환할때까지)
	{
	  printf("%s\n", ptr);         //자른 문자 출력
	  ptr = strtok(NULL, " ");     //자른 문자 다음부터 구분자 또 찾기
	}
#endif
	return true;
}
