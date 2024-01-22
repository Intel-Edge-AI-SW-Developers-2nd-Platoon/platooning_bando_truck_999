#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>

#define BUF_SIZE 1024
#define NAME_SIZE 20
#define ARR_CNT 16
#define DATA_SIZE 1024
#define PACU_NUMBER 6
#define CAR_NUMBER 3

void integral(int mode, char target, int pos);
void * ccu_send_msg(void * arg);
void * ccu_recv_msg(void * arg);
void * platoon_send_msg(void * arg);
void * platoon_recv_msg(void * arg);
void * timer_msg(void * arg);
void error_handling(char * msg);

//autodirver funtion ,flags and data structure
void platoon_autodriver_order(void);
int autodirver_counter = 0;
int car_set_interval_distance = 20;
bool car_set_flag[CAR_NUMBER] = {};
bool car_move_able_flag[CAR_NUMBER] = {};
uint8_t curr_direction[CAR_NUMBER] = {};
uint8_t curr_pwm[CAR_NUMBER] = {};

typedef struct {
	uint8_t order_direction[DATA_SIZE];
	uint8_t order_pwm[DATA_SIZE];
	double angle[DATA_SIZE];
	double velo_x[DATA_SIZE];
	double velo_y[DATA_SIZE];
	double pos_x[DATA_SIZE];
	double pos_y[DATA_SIZE];
	double dist_front[DATA_SIZE];
	double dist_back[DATA_SIZE];
} autodriver_vals;
autodriver_vals autodirvers_vals[CAR_NUMBER];

typedef struct {
        time_t	tv_sec;
        time_t	tv_usec;
} timeval;

typedef struct {
	double dist[DATA_SIZE];
	double acc_x[DATA_SIZE];
	double acc_y[DATA_SIZE];
	double angle[DATA_SIZE];
	double gyro[DATA_SIZE];
	timeval curr_time[DATA_SIZE];
} PACU_BUF_T;

typedef struct {
	double velo_x[DATA_SIZE];
	double velo_y[DATA_SIZE];
	double pos_x[DATA_SIZE];
	double pos_y[DATA_SIZE];
} CCU_BUF_T;

pthread_mutex_t g_mutex[8];
char name[2][NAME_SIZE]={"[Default]", "[Default]"};
char msg[2][BUF_SIZE];
PACU_BUF_T pacu_buf[PACU_NUMBER];
CCU_BUF_T ccu_buf[PACU_NUMBER];
int pacu_pos[PACU_NUMBER];
//int gettimeofday(struct timeval *tv, struct timezone *tz);

struct timeval prev_time, curr_time;

int sock[2];

int main(int argc, char *argv[]) {
	struct sockaddr_in ccu_serv_addr;
	struct sockaddr_in platoon_serv_addr;
	pthread_t ccu_snd_thread, ccu_rcv_thread, platoon_snd_thread, platoon_rcv_thread, timer_thread;
	void * thread_return;

	if(argc != 7) {
		printf("Usage : %s <CCU IP> <port> <name> <PLATOON IP> <port> <name>\n",argv[0]);
		exit(1);
	}
	sock[0] = socket(PF_INET, SOCK_STREAM, 0);
	if(sock[0] == -1) {
		error_handling("socket() error");
	}
	sock[1] = socket(PF_INET, SOCK_STREAM, 0);
	if(sock[1] == -1) {
		error_handling("socket() error");
	}
	memset(&ccu_serv_addr, 0, sizeof(ccu_serv_addr));
	memset(&platoon_serv_addr, 0, sizeof(platoon_serv_addr));
	
	ccu_serv_addr.sin_family=AF_INET;
	ccu_serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	ccu_serv_addr.sin_port = htons(atoi(argv[2]));

	platoon_serv_addr.sin_family=AF_INET;
	platoon_serv_addr.sin_addr.s_addr = inet_addr(argv[4]);
	platoon_serv_addr.sin_port = htons(atoi(argv[5]));
	
	if(connect(sock[0], (struct sockaddr *)&ccu_serv_addr, sizeof(ccu_serv_addr)) == -1) {
		error_handling("connect() error");
	}
	if(connect(sock[1], (struct sockaddr *)&platoon_serv_addr, sizeof(platoon_serv_addr)) == -1) {
		error_handling("connect() error");
	}

	sprintf(&name[0][0], "%s",argv[3]);
	sprintf(&msg[0][0],"[%s:PASSWD]",&name[0][0]);
	write(sock[0], &msg[0][0], strlen(&msg[0][0]));
	
	sprintf(&name[1][0], "%s",argv[6]);
	sprintf(&msg[1][0],"[%s:PASSWD]",&name[1][0]);
	write(sock[1], &msg[1][0], strlen(&msg[1][0]));

	pthread_create(&ccu_rcv_thread, NULL, ccu_recv_msg, (void *)&sock[0]);
	pthread_create(&ccu_snd_thread, NULL, ccu_send_msg, (void *)&sock[0]);
	pthread_create(&platoon_rcv_thread, NULL, platoon_recv_msg, (void *)&sock[0]);
	pthread_create(&platoon_snd_thread, NULL, platoon_send_msg, (void *)&sock[1]);
	pthread_create(&timer_thread, NULL, timer_msg, (void *)&sock[0]);

	pthread_join(ccu_snd_thread, &thread_return);
	pthread_join(platoon_snd_thread, &thread_return);
	//pthread_join(rcv_thread, &thread_return);
	//pthread_join(timer_thread, &thread_return);
	close(sock[0]);
	close(sock[1]);
	return 0;
}

void set_integral(int mode, char target, int pos) {
	if (!pos) return;
	int interval = ((pacu_buf[mode].curr_time[pos].tv_usec - pacu_buf[mode].curr_time[pos-1].tv_usec))/1000000000;
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

void * ccu_send_msg(void * arg) {
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
		memset(&msg[0][0],0,sizeof(&msg[0][0]));
		name_msg[0] = '\0';
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(STDIN_FILENO + 1, &newset, NULL, NULL, &tv);
		if(FD_ISSET(STDIN_FILENO, &newset)) {
			pthread_mutex_lock(&g_mutex[0]);
			fgets(&msg[0][0], BUF_SIZE, stdin);
			if(!strncmp(&msg[0][0],"quit\n",5)) {
				*sock = -1;
				return NULL;
			}
			//else if(!strncmp(&msp[0][0],"[]")) {
			//}
			else if(msg[0][0] != '[') {
				strcat(name_msg,"[ALLMSG]");
				strcat(name_msg,msg[0]);
			}
			else strcpy(name_msg,&msg[0][0]);
			if(write(*sock, name_msg, strlen(name_msg))<=0) {
				*sock = -1;
				return NULL;
			}
			pthread_mutex_unlock(&g_mutex[0]);
		}
		if(ret == 0) {
			if(*sock == -1) return NULL;
		}
	}
}

void * ccu_recv_msg(void * arg) {
	int * sock = (int *)arg;	
	int i;
	char *pToken;
	char *pArray[ARR_CNT]={0};
	char name_msg[NAME_SIZE + BUF_SIZE +1];
	int str_len;

	while(1) {
		pthread_mutex_lock(&g_mutex[0]);
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
			gettimeofday(&(pacu_buf[0].curr_time[pacu_pos[0]]), NULL);
			set_integral(0, 'a', pacu_pos[0]);
			set_integral(0, 'd', pacu_pos[0]);
			pacu_pos[0]++;
			if (pacu_pos[0] >= DATA_SIZE) pacu_pos[0] = 0;
		}
		else if (!strcmp(pArray[1], "PACU2")) {
			sscanf(pArray[2], "%lf", &(pacu_buf[1].dist[pacu_pos[1]]));	
			sscanf(pArray[3], "%lf", &(pacu_buf[1].acc_x[pacu_pos[1]]));	
			sscanf(pArray[4], "%lf", &(pacu_buf[1].acc_y[pacu_pos[1]]));	
			sscanf(pArray[5], "%lf", &(pacu_buf[1].gyro[pacu_pos[1]]));	
			sscanf(pArray[6], "%lf", &(pacu_buf[1].angle[pacu_pos[1]]));	
			gettimeofday(&(pacu_buf[0].curr_time[pacu_pos[0]]), NULL);
			set_integral(1, 'a', pacu_pos[1]);
			set_integral(1, 'd', pacu_pos[1]);
			pacu_pos[1]++;
			if (pacu_pos[1] >= DATA_SIZE) pacu_pos[1] = 0;
		}
		else if (!strcmp(pArray[1], "CAR_A")) {
				sprintf(&msg[0][0], "[CONTROL]CAR_A@%s@%s@%s\n", pArray[2], pArray[3], pArray[4]);
				write(*sock, &msg[0][0], strlen(&msg[0][0]));
		}
		pthread_mutex_unlock(&g_mutex[0]);

	}
}

void * platoon_send_msg(void * arg) {
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
		memset(&msg[1][0],0,sizeof(&msg[1][0]));
		name_msg[0] = '\0';
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(STDIN_FILENO + 1, &newset, NULL, NULL, &tv);
		if(FD_ISSET(STDIN_FILENO, &newset)) {
			//pthread_mutex_lock(&g_mutex[1]);
			fgets(&msg[1][0], BUF_SIZE, stdin);
			if(!strncmp(&msg[1][0],"quit\n",5)) {
				*sock = -1;
				return NULL;
			}
			else if(msg[1][0] != '[') {
				strcat(name_msg,"[ALLMSG]");
				strcat(name_msg,&msg[1][0]);
			}
			else strcpy(name_msg,&msg[1][0]);
			if(write(*sock, name_msg, strlen(name_msg))<=0) {
				*sock = -1;
				return NULL;
			}
			//pthread_mutex_unlock(&g_mutex[1]);
		}
		if(ret == 0) {
			if(*sock == -1) return NULL;
		}
	}
}

void * platoon_recv_msg(void * arg) {
	int * sock = (int *)arg;
	int i;
	char *pToken;
	char *pArray[ARR_CNT]={0};
	char name_msg[NAME_SIZE + BUF_SIZE +1];
	int str_len;

	while(1) {
		//pthread_mutex_lock(&g_mutex[1]);
		memset(name_msg,0x0,sizeof(name_msg));
		str_len = read(*(sock + 1), name_msg, NAME_SIZE + BUF_SIZE );
		if(str_len <= 0) {
			*(sock + 1) = -1;
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
		if (!strcmp(pArray[1],"PLATOON")) {
		}
		else if (!strcmp(pArray[1], "CAR_A")) {
				sprintf(&msg[0][0], "[CONTROL]CAR_A@%s@%s@%s\n", pArray[2], pArray[3], pArray[4]);
				write(*sock, &msg[0][0], strlen(&msg[0][0]));
				curr_direction[0] = (uint8_t)atoi(pArray[2]);
				curr_pwm[0] = (uint8_t)atoi(pArray[4]);
		}
		else if (!strcmp(pArray[1], "CAR_B")) {
		}
		else if (!strcmp(pArray[1], "CAR_C")) {
		}
		else if (!strcmp(pArray[1], "GUI")) {
		}
		//pthread_mutex_unlock(&g_mutex[1]);

	}
}

void * timer_msg(void * arg) {
	int *sock = (int *)arg;
	char name_msg[NAME_SIZE + BUF_SIZE +1];
	gettimeofday(&prev_time, NULL);
	double global_interval;
	while(1) {
		gettimeofday(&curr_time, NULL);
		global_interval = ((double)(curr_time.tv_sec - prev_time.tv_sec) * 1000 + (double)(curr_time.tv_usec - prev_time.tv_usec)/1000);
		if (global_interval >= 100) {
			pthread_mutex_unlock(&g_mutex[1]);
			
        		//for (int i = 0; i < 2; i++) {
				memset(&msg[1][0],0,sizeof(&msg[1][0]));
			//}
			name_msg[0] = '\0';
			
			sprintf(&msg[1][0], "[GUI]%s@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf\n", 
					&name[1][0], pacu_buf[0].dist[pacu_pos[0]], pacu_buf[1].dist[pacu_pos[1]], 
					(pacu_buf[0].acc_x[pacu_pos[0]] + pacu_buf[1].acc_x[pacu_pos[1]]) / 2, 
					(pacu_buf[0].acc_y[pacu_pos[0]] + pacu_buf[1].acc_y[pacu_pos[1]]) / 2, 
					(ccu_buf[0].velo_x[pacu_pos[0]] + ccu_buf[1].velo_x[pacu_pos[1]]) / 2, 
					(ccu_buf[0].velo_y[pacu_pos[0]] + ccu_buf[1].velo_y[pacu_pos[1]]) / 2, 
					(ccu_buf[0].pos_x[pacu_pos[0]] + ccu_buf[1].pos_y[pacu_pos[1]]) / 2, 
					(ccu_buf[0].pos_y[pacu_pos[0]] + ccu_buf[1].pos_y[pacu_pos[1]]) / 2, 
					(pacu_buf[0].angle[pacu_pos[0]] + pacu_buf[1].angle[pacu_pos[1]]) / 2, 
					(pacu_buf[0].gyro[pacu_pos[0]] + pacu_buf[1].gyro[pacu_pos[1]]) / 2
				);	
			//strcmp(name_msg, msg);	 
			write(*(sock + 1), &msg[1][0], strlen(&msg[1][0]));

			memset(&msg[1][0],0,sizeof(&msg[1][0]));
			name_msg[0] = '\0';

			sprintf(&msg[1][0], "[CAR_B]%s@DATA@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lf@%lld\n", &name[1][0],
					pacu_buf[0].dist[pacu_pos[0]], pacu_buf[1].dist[pacu_pos[1]],
					(pacu_buf[0].acc_x[pacu_pos[0]] + pacu_buf[1].acc_x[pacu_pos[1]]) / 2,
					(pacu_buf[0].acc_y[pacu_pos[0]] + pacu_buf[1].acc_y[pacu_pos[1]]) / 2,
					(ccu_buf[0].velo_x[pacu_pos[0]] + ccu_buf[1].velo_x[pacu_pos[1]]) / 2,
					(ccu_buf[0].velo_y[pacu_pos[0]] + ccu_buf[1].velo_y[pacu_pos[1]]) / 2,
					(ccu_buf[0].pos_x[pacu_pos[0]] + ccu_buf[1].pos_y[pacu_pos[1]]) / 2,
					(ccu_buf[0].pos_y[pacu_pos[0]] + ccu_buf[1].pos_y[pacu_pos[1]]) / 2,
					(pacu_buf[0].angle[pacu_pos[0]] + pacu_buf[1].angle[pacu_pos[1]]) / 2,
					(pacu_buf[0].gyro[pacu_pos[0]] + pacu_buf[1].gyro[pacu_pos[1]]) / 2,
					(long long)(pacu_buf[0].curr_time[pacu_pos[0]].tv_sec + pacu_buf[1].curr_time[pacu_pos[1]].tv_sec) * 500 + (long long)(pacu_buf[0].curr_time[pacu_pos[0]].tv_usec + pacu_buf[1].curr_time[pacu_pos[1]].tv_usec) * 500);
			//strcmp(name_msg, msg);
			//write(*sock, &msg[1][0], strlen(&msg[1][0]));
			
			pthread_mutex_lock(&g_mutex[1]);
			prev_time.tv_sec = curr_time.tv_sec;
			prev_time.tv_usec = curr_time.tv_usec;
			//printf("%ld\n", curr_time.tv_usec);	
			//printf("%lf\n", (double)curr_time.tv_sec + (double)curr_time.tv_usec / 1000000);	
		}
	}
}

void error_handling(char * msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void platoon_autodriver_order(void)
{
	//set values for autodrive
	//CAR_A, CAR_B, CAR_C's x_vel, y_vel, x_point, y_point, angle
	char temp[100] = "";
	
	for(int i=0; i<CAR_NUMBER; i++)
	{
		autodirvers_vals[i].order_direction[autodirver_counter] = curr_direction[0];
		autodirvers_vals[i].order_pwm[autodirver_counter] = curr_pwm[0];
		autodirvers_vals[i].angle[autodirver_counter] = (double)(pacu_buf[i*2].angle[pacu_pos[i*2]]-pacu_buf[i*2+1].angle[pacu_pos[i*2+1]])/2;
		autodirvers_vals[i].velo_x[autodirver_counter] = (double)(ccu_buf[i*2].velo_x[pacu_pos[i*2]]-ccu_buf[i*2].velo_x[pacu_pos[i*2+1]])/2;
		autodirvers_vals[i].velo_y[autodirver_counter] = (double)(ccu_buf[i*2].velo_y[pacu_pos[i*2]]-ccu_buf[i*2].velo_y[pacu_pos[i*2+1]])/2;
		autodirvers_vals[i].pos_x[autodirver_counter] = (double)(ccu_buf[i*2].pos_x[pacu_pos[i*2]]-ccu_buf[i*2+1].pos_x[pacu_pos[i*2+1]])/2;
		autodirvers_vals[i].pos_y[autodirver_counter] = (double)(ccu_buf[i*2].pos_y[pacu_pos[i*2]]-ccu_buf[i*2+1].pos_y[pacu_pos[i*2+1]])/2;
		autodirvers_vals[i].dist_front[autodirver_counter] = pacu_buf[i*2].dist[pacu_pos[i*2]];
		autodirvers_vals[i].dist_back[autodirver_counter] = pacu_buf[i*2+1].dist[pacu_pos[i*2+1]];
	}
	sprintf(&temp[0], "%d %d %4lf %4lf %4lf %4lf %4lf %4lf %4lf",
	autodirvers_vals[0].order_direction[autodirver_counter],
	autodirvers_vals[0].order_pwm[autodirver_counter],
	autodirvers_vals[0].angle[autodirver_counter],
	autodirvers_vals[0].velo_x[autodirver_counter],
	autodirvers_vals[0].velo_y[autodirver_counter],
	autodirvers_vals[0].pos_x[autodirver_counter],
	autodirvers_vals[0].pos_y[autodirver_counter],
	autodirvers_vals[0].dist_front[autodirver_counter],
	autodirvers_vals[0].dist_back[autodirver_counter]
	);

	autodirver_counter++;
	if(autodirver_counter >= DATA_SIZE)autodirver_counter = 0;
}