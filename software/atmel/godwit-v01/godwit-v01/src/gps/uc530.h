/*************************************************
 * FILE:    uc530.h
 * AUTHOR:  ADG
 * DESC:    UC530 NMEA PARSER
 *************************************************/

#ifndef _UC530_H_
#define _UC530_H_

typedef struct {
	unsigned char deg;
	float min;
	unsigned char hemi;
} lat_long_t;

typedef struct {
	// UTC Time
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	float second;
	
	// Location Data
	lat_long_t lat;
	lat_long_t longi;
	float alt;
	float spd;
	float hdg;
	
	// Fix Data
	unsigned char fix_status;
	unsigned char num_sats;
	float pdop;
	float hdop;
	float vdop;
	
	unsigned int valid;
} uc530_data_t;

#define UC530_SUCCESS	1
#define UC530_FAILURE	0

int uc530_parse_default(char *str, unsigned int len, uc530_data_t *gps_data);

#endif  // _UC530_H_