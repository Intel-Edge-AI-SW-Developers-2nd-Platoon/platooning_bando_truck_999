/*
 * mpu.c
 *
 *  Created on: Jan 11, 2024
 *      Author: user
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "main.h"
#include "mpu.h"
//#include "uart.h"

void mpu_init() {
	pkt_flag = false;

	for(int i = 0; i < 3; i++){
		rxd_pkt[i].header_h = 0;
		rxd_pkt[i].header_l = 0;
		rxd_pkt[i].check_sum = 0;
	}

	HAL_UART_Receive_IT(&huart2, (PKT_T *)&rxd_pkt[0], 3 * PKT_SIZE);
}

void mpu_regcbf(MPU_CBF cbf) {
  mpu_cbf = cbf;
}

void mpu_uart_cbf() {
	for (int i = 0; i < 3; i++){
		PKT_T *pPkt = (PKT_T *)&rxd_pkt[i];

#if DEBUG_PKT
printf("%c %c \r\n", pPkt->header_h, pPkt->header_l);
#endif

		if (!pkt_flag && (pPkt->header_h == 0x55)) {

#if DEBUG_PKT
printf("%s Packet Data \r\n", pkt_name[i]);
#endif

			uint8_t check_sum_tmp = pPkt->header_h + pPkt->header_l;
			for (int i = 0; i < PKT_SIZE - 3; i++)
				check_sum_tmp += pPkt->data[i];

			for (int i = 0; i < PKT_SIZE - 3; i++) {

#if DEBUG_PKT
printf("%d:0x%02x ", i, pPkt->data[i]);
if ((i % (PKT_SIZE - 3)) == PKT_SIZE - 4) printf("\r\n");
#endif

			}

			if (check_sum_tmp == pPkt->check_sum) {

#if DEBUG_PKT
printf("Checksum : OK..!!\r\n");
#endif
				if (!i) {
					pQMpuBuf[qBufCnt].ax = ((double)(int16_t)((pPkt->data[1] << 8) | pPkt->data[0])) * 16 * 9.8 / 32768;
					pQMpuBuf[qBufCnt].ay = ((double)(int16_t)((pPkt->data[3] << 8) | pPkt->data[2])) * 16 * 9.8 / 32768;
					pQMpuBuf[qBufCnt].az = ((double)(int16_t)((pPkt->data[5] << 8) | pPkt->data[4])) * 16 * 9.8 / 32768;
				}
				else if (i == 1) {
					pQMpuBuf[qBufCnt].wx = ((double)(int16_t)((pPkt->data[1] << 8) | pPkt->data[0])) * 2000 / 32768;
					pQMpuBuf[qBufCnt].wx = ((double)(int16_t)((pPkt->data[3] << 8) | pPkt->data[2])) * 2000 / 32768;
					pQMpuBuf[qBufCnt].wx = ((double)(int16_t)((pPkt->data[5] << 8) | pPkt->data[4])) * 2000 / 32768;
				}
				else if (i == 2) {
					pQMpuBuf[qBufCnt].roll = ((double)(int16_t)((pPkt->data[1] << 8) | pPkt->data[0])) * 180 / 32768;
					pQMpuBuf[qBufCnt].pitch = ((double)(int16_t)((pPkt->data[3] << 8) | pPkt->data[2])) * 180 / 32768;
					pQMpuBuf[qBufCnt].yaw = ((double)(int16_t)((pPkt->data[5] << 8) | pPkt->data[4])) * 180 / 32768;
					pQMpuBuf[qBufCnt].temperature = ((double)(int16_t)((pPkt->data[7] << 8) | pPkt->data[6])) / 340 + 36.53;

					qBufCnt++;

					if (qBufCnt == Q_BUF_SIZE) {
						qBufCnt = 0;
						qBufFlag = true;
					}

					mpu_avg_calc();
				}
			}
			else {

#if DEBUG_PKT
printf("Checksum : Failed...\r\n");
#endif

			}
		}
	}

#if DEBUG_DATA
//if (pkt_flag){
printf("|----------------------------------------------------------------|\r\n");
printf("Accel X, Y, Z : %f, %f, %f m/s^2 \r\n", pQMpuBuf->ax, pQMpuBuf->ay, pQMpuBuf->az);
printf("Gyro X, Y, Z : %f, %f, %f degree/s \r\n", pQMpuBuf->wx, pQMpuBuf->wy, pQMpuBuf->wz);
printf("Roll, Pitch, Yaw : %f, %f, %f degree \r\n", pQMpuBuf->roll, pQMpuBuf->pitch, pQMpuBuf->yaw);
printf("Temperature : %f degree(C) \r\n", pQMpuBuf->temperature);
printf("|----------------------------------------------------------------|\r\n");
//}
#endif

	pkt_flag = true;
}

void mpu_avg_calc() {
	if (qBufFlag) {
		qAvgBuf.ax = 0; qAvgBuf.ay = 0; qAvgBuf.az = 0;
		qAvgBuf.wx = 0; qAvgBuf.wy = 0; qAvgBuf.wz = 0;
		qAvgBuf.roll = 0; qAvgBuf.pitch = 0; qAvgBuf.yaw = 0;
		qAvgBuf.temperature = 0;
		for (int i = 0; i < Q_BUF_SIZE; i++){
			qAvgBuf.ax += pQMpuBuf[i].ax; qAvgBuf.ay += pQMpuBuf[i].ay; qAvgBuf.az += pQMpuBuf[i].az;
			qAvgBuf.wx += pQMpuBuf[i].wx; qAvgBuf.wy += pQMpuBuf[i].wy; qAvgBuf.wz += pQMpuBuf[i].wz;
			qAvgBuf.roll += pQMpuBuf[i].roll; qAvgBuf.pitch += pQMpuBuf[i].pitch; qAvgBuf.yaw += pQMpuBuf[i].yaw;
			qAvgBuf.temperature += pQMpuBuf[i].temperature;
		}
		qAvgBuf.ax /= Q_BUF_SIZE; qAvgBuf.ay /= Q_BUF_SIZE; qAvgBuf.az /= Q_BUF_SIZE;
		qAvgBuf.wx /= Q_BUF_SIZE; qAvgBuf.wy /= Q_BUF_SIZE; qAvgBuf.wz /= Q_BUF_SIZE;
		qAvgBuf.roll /= Q_BUF_SIZE; qAvgBuf.pitch /= Q_BUF_SIZE; qAvgBuf.yaw /= Q_BUF_SIZE;
		qAvgBuf.temperature /= Q_BUF_SIZE;

#if DEBUG_AVG
printf("|----------------------------------------------------------------|\r\n");
printf("Accel X, Y, Z : %f, %f, %f m/s^2 \r\n", qAvgBuf.ax, qAvgBuf.ay, qAvgBuf.az);
printf("Gyro X, Y, Z : %f, %f, %f degree/s \r\n", qAvgBuf.wx, qAvgBuf.wy, qAvgBuf.wz);
printf("Roll, Pitch, Yaw : %f, %f, %f degree \r\n", qAvgBuf.roll, qAvgBuf.pitch, qAvgBuf.yaw);
printf("Temperature : %f degree(C) \r\n", qAvgBuf.temperature);
printf("|----------------------------------------------------------------|\r\n");
#endif
	}
}

void mpu_proc() {
	for(int i = 0; i < 3; i++){
		PKT_T *pPkt = (PKT_T *)&rxd_pkt[i];
		if (pkt_flag) {
			pPkt->header_h = 0;
			pPkt->header_l = 0;
			pPkt->check_sum = 0;
			//pkt_cnt = 0;
			pkt_flag ^= true;
		}
	}
	HAL_UART_Receive_IT(&huart2, (PKT_T *)&rxd_pkt[0], 3 * PKT_SIZE);
}
