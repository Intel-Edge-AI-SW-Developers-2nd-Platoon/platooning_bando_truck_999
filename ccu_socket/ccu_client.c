#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define BUF_SIZE 1024
#define NAME_SIZE 20
#define ARR_CNT 16
#define DATA_SIZE 1024

void integral(int mode, char target, int pos);
void * send_msg(void * arg);
void * recv_msg(void * arg);
void * timer_msg(void * arg);
void error_handling(char * msg);

typedef struct {
        time_t  tv_sec;
        long    tv_nsec;
} timespec;

typedef struct {
	double dist[DATA_SIZE];
	double acc_x[DATA_SIZE];
	double acc_y[DATA_SIZE];
	double angle[DATA_SIZE];
	double gyro[DATA_SIZE];
	timespec curr_time[DATA_SIZE];
} PACU_BUF_T;

typedef struct {	
	double velo_x[DATA_SIZE];
	double velo_y[DATA_SIZE];
	double pos_x[DATA_SIZE];
	double pos_y[DATA_SIZE];
} CCU_BUF_T;

char name[NAME_SIZE]="[Default]";
char msg[BUF_SIZE];
PACU_BUF_T pacu_buf[2];
CCU_BUF_T ccu_buf[2];
int pacu_pos[2];

struct timespec prev_time, curr_time;

int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread, timer_thread;
	void * thread_return;

	if(argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n",argv[0]);
		exit(1);
	}
	sprintf(name, "%s",argv[3]);
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		error_handling("socket() error");
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		error_handling("connect() error");
	}

	sprintf(msg,"[%s:PASSWD]",name);
	write(sock, msg, strlen(msg));
	pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);
	pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
	pthread_create(&timer_thread, NULL, timer_msg, (void *)&sock);

	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);
	pthread_join(timer_thread, &thread_return);
	close(sock);
	return 0;
}

void integral(int mode, char target, int pos) {
	if (!pos) return;
	int interval = ((pacu_buf[mode].curr_time[pos].tv_sec - pacu_buf[mode].curr_time[pos-1].tv_sec) + (pacu_buf[mode].curr_time[pos].tv_nsec - pacu_buf[mode].curr_time[pos-1].tv_nsec))/1000000000;
	if (target == 'a') {
		ccu_buf[mode].velo_x[pos] = pacu_buf[mode].acc_x[pos] * (double)interval;
		ccu_buf[mode].velo_y[pos] = pacu_buf[mode].acc_y[pos] * (double)interval;
	}
	else if (target = 'v') {
		if (!ccu_buf[mode].velo_x[pos]) return;
		ccu_buf[mode].pos_x[pos] = ccu_buf[mode].velo_x[pos] * (double)interval;
		ccu_buf[mode].pos_y[pos] = ccu_buf[mode].velo_y[pos] * (double)interval;
	}	
}

void * send_msg(void * arg) {
	int *sock = (int *)arg;
	int str_len;
	int ret;
	fd_set initset, newset;
	struct timeval tv;
	char name_msg[NAME_SIZE + BUF_SIZE+2];

	FD_ZERO(&initset);
	FD_SET(STDIN_FILENO, &initset);

	fputs("Input a message! [ID]msg (Default ID:ALLMSG)\n",stdout);
	while(1) {
		memset(msg,0,sizeof(msg));
		name_msg[0] = '\0';
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(STDIN_FILENO + 1, &newset, NULL, NULL, &tv);
		if(FD_ISSET(STDIN_FILENO, &newset)) {
			fgets(msg, BUF_SIZE, stdin);
			if(!strncmp(msg,"quit\n",5)) {
				*sock = -1;
				return NULL;
			}
			else if(msg[0] != '[') {
				strcat(name_msg,"[ALLMSG]");
				strcat(name_msg,msg);
			}
			else strcpy(name_msg,msg);
			if(write(*sock, name_msg, strlen(name_msg))<=0) {
				*sock = -1;
				return NULL;
			}
		}
		if(ret == 0) {
			if(*sock == -1) return NULL;
		}
	}
}

void * recv_msg(void * arg) {
	int * sock = (int *)arg;	
	int i;
	char *pToken;
	char *pArray[ARR_CNT]={0};
	char name_msg[NAME_SIZE + BUF_SIZE +1];
	int str_len;

	while(1) {
		memset(name_msg,0x0,sizeof(name_msg));
		str_len = read(*sock, name_msg, NAME_SIZE + BUF_SIZE );
		if(str_len <= 0) {
			*sock = -1;
			return NULL;
		}
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
		pToken = strtok(name_msg, "[:@]");
		i = 0;
		while (pToken != NULL) {
			pArray[i] = pToken;
			if ( ++i >= ARR_CNT) break;
			pToken = strtok(NULL, "[:@]");
		}
		if (!strcmp(pArray[1],"PACU1")) {
			sscanf(pArray[2], "%lf", &(pacu_buf[0].dist[pacu_pos[0]]));	
			sscanf(pArray[3], "%lf", &(pacu_buf[0].acc_x[pacu_pos[0]]));	
			sscanf(pArray[4], "%lf", &(pacu_buf[0].acc_y[pacu_pos[0]]));	
			sscanf(pArray[5], "%lf", &(pacu_buf[0].gyro[pacu_pos[0]]));	
			sscanf(pArray[6], "%lf", &(pacu_buf[0].angle[pacu_pos[0]]));
			clock_gettime(CLOCK_MONOTONIC, &(pacu_buf[0].curr_time[pacu_pos[0]]));
			integral(0, 'a', pacu_pos[0]);
			integral(0, 'd', pacu_pos[0]);
			pacu_pos[0]++;
			if (pacu_pos[0] >= DATA_SIZE) pacu_pos[0] = 0;
		}
		else if (!strcmp(pArray[1], "PACU2")) {
			sscanf(pArray[2], "%lf", &(pacu_buf[1].dist[pacu_pos[1]]));	
			sscanf(pArray[3], "%lf", &(pacu_buf[1].acc_x[pacu_pos[1]]));	
			sscanf(pArray[4], "%lf", &(pacu_buf[1].acc_y[pacu_pos[1]]));	
			sscanf(pArray[5], "%lf", &(pacu_buf[1].gyro[pacu_pos[1]]));	
			sscanf(pArray[6], "%lf", &(pacu_buf[1].angle[pacu_pos[1]]));	
			clock_gettime(CLOCK_MONOTONIC, &(pacu_buf[1].curr_time[pacu_pos[1]]));
			integral(1, 'a', pacu_pos[1]);
			integral(1, 'd', pacu_pos[1]);
			pacu_pos[1]++;
			if (pacu_pos[1] >= DATA_SIZE) pacu_pos[1] = 0;
		}

	}
}

void * timer_msg(void * arg) {
	int *sock = (int *)arg;
	clock_gettime(CLOCK_MONOTONIC, &(prev_time));
	int global_interval;
	while(1) {
		clock_gettime(CLOCK_MONOTONIC, &(curr_time));
		global_interval = ((curr_time.tv_nsec - prev_time.tv_nsec) + (curr_time.tv_sec - prev_time.tv_sec));
		if (global_interval >= 1000000000) {
			sprintf(msg, "[GUI]DATA@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%ld@%ld\n", 
					pacu_buf[0].dist[pacu_pos[0]], pacu_buf[1].dist[pacu_pos[1]], 
					(pacu_buf[0].acc_x[pacu_pos[0]] + pacu_buf[1].acc_x[pacu_pos[1]]) / 2, 
					(pacu_buf[0].acc_y[pacu_pos[0]] + pacu_buf[1].acc_y[pacu_pos[1]]) / 2, 
					(ccu_buf[0].velo_x[pacu_pos[0]] + ccu_buf[1].velo_x[pacu_pos[1]]) / 2, 
					(ccu_buf[0].velo_y[pacu_pos[0]] + ccu_buf[1].velo_y[pacu_pos[1]]) / 2, 
					(ccu_buf[0].pos_x[pacu_pos[0]] + ccu_buf[1].pos_y[pacu_pos[1]]) / 2, 
					(ccu_buf[0].pos_y[pacu_pos[0]] + ccu_buf[1].pos_y[pacu_pos[1]]) / 2, 
					(pacu_buf[0].angle[pacu_pos[0]] + pacu_buf[1].angle[pacu_pos[1]]) / 2, 
					(pacu_buf[0].gyro[pacu_pos[0]] + pacu_buf[1].gyro[pacu_pos[1]]) / 2,
					(pacu_buf[0].curr_time[pacu_pos[0]].tv_sec + pacu_buf[1].curr_time[pacu_pos[1]].tv_sec) / 2,
					(pacu_buf[0].curr_time[pacu_pos[0]].tv_nsec + pacu_buf[1].curr_time[pacu_pos[1]].tv_nsec) / 2);
					 
			if (write(*sock, msg, strlen(msg)) <= 0) {
				*sock = -1;
				return NULL;
			}
			prev_time.tv_sec = curr_time.tv_sec;
			prev_time.tv_nsec = curr_time.tv_nsec;
		}	
	}
}

void error_handling(char * msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

