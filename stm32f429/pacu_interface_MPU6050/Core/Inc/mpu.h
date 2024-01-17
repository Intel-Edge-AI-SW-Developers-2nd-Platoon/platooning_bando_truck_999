/*
 * mpu.h
 *
 *  Created on: Jan 11, 2024
 *      Author: user
 */

#ifndef INC_MPU_H_
#define INC_MPU_H_

typedef bool (*MPU_CBF)(uint8_t *);

#ifdef __cplusplus
extern "C" {
#endif

void mpu_init(void);
void mpu_regcbf(MPU_CBF cbf);
void mpu_proc(void);
void mpu_avg_calc(void);
//void mpu_regcbf();

extern UART_HandleTypeDef huart2;

#define DEBUG_PKT 	0
#define DEBUG_DATA  0
#define DEBUG_AVG   1
#define PKT_SIZE	11
#define Q_BUF_SIZE	10

typedef struct {
	uint8_t header_h;
	uint8_t header_l;
	uint8_t data[PKT_SIZE - 3];
	uint8_t check_sum;
} PKT_T;

typedef struct {
	double ax; double ay; double az;
	double wx; double wy; double wz;
	double roll; double pitch; double yaw;
	double temperature;
} MPU_T;

static MPU_CBF mpu_cbf;
static PKT_T rxd_pkt[3];
static MPU_T pQMpuBuf[Q_BUF_SIZE];
static MPU_T qAvgBuf;
static uint8_t qBufCnt = 0;
static bool qBufFlag = false;

bool pkt_flag;

char *pkt_name[] = {
	"Accel", "Gyro", "Angle"
};

#ifdef __cplusplus
}
#endif

#endif /* INC_MPU_H_ */
