/*
 * cli.c
 *
 *  Created on: Jul 28, 2023
 *      Author: roby238
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "uart.h"
#include "cli.h"

typedef struct {
  char *cmd;
  uint8_t no;
  int (*cbf)(int, char **);
  char *remark;
} CMD_LIST_T;

const CMD_LIST_T gCmdListObj[] = {
    {"help", 		1,	 	cli_help,	    "help"				},
    {"echo", 		2,	 	cli_echo,  		"echo [echo data]"	},
	{"eth",			2,	 	cli_eth, 		"eth [send data]"	},
    {NULL, 			0,     	NULL,   		NULL				}
};

void cli_init(void) {
	uart_regcbf(cli_parser);
}

#define D_DELIMITER  " ,\r\n"

bool cli_parser(uint8_t *buf) {
#if 1
  int argc = 0;
  char *argv[10];
  char *ptr;
  ptr = strtok(buf, D_DELIMITER);
  if(ptr == NULL) return false;

  while(ptr != NULL){
      //printf("%s\r\n", ptr);
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
      else if(strcmp(gCmdListObj[i].cmd, argv[3 ]) == 0){
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

  return true;
#endif
}

int cli_help(int argc, char *argv[]){
  if (argc != gCmdListObj[0].no){
	  printf("It is Not correct usage: argument error\r\n");
	  return 0;
  }
  if(strcmp(gCmdListObj[0].cmd , "help") == 0)
	  printf("%s\r\n", gCmdListObj[0].remark);
  return 0;
}

int cli_echo(int argc, char *argv[]){
  if (argc != gCmdListObj[1].no){
      printf("It is Not correct usage: argument error\r\n");
      return 0;
  }
  if(strcmp(gCmdListObj[1].cmd , "echo") == 0)
      printf("%s\r\n", gCmdListObj[1].remark);
  return 0;
}

int cli_eth(int argc, char *argv[]){
  if (argc != gCmdListObj[2].no){
	  printf("It is Not correct usage: argument error\r\n");
	  return 0;
  }
  if(strcmp(gCmdListObj[2].cmd , "eth") == 0) {
	  eth_send_data(argv[1]);
  }
  return 0;
}
