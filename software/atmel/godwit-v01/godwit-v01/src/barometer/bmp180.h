/*************************************************
 * FILE:    bmp180.h
 * AUTHOR:  ADG
 * DESC:    BMP180 Pressure Sensor
 *************************************************/

#ifndef _BMP180_H_
#define _BMP180_H_

// Calibration Data Used For Calculations
struct {
		uint16_t ac1;
		uint16_t ac2;
		uint16_t ac3;
		uint16_t ac4;
		uint16_t ac5;
		uint16_t ac6;
		
		uint16_t b1;
		uint16_t b2;
		
		uint16_t MB;
		uint16_t MC;
		uint16_t MD;
	} bmp180_cal_data;
	
uint16_t other_bmp180_cal_data[11];

#endif	// _BMP180_H_