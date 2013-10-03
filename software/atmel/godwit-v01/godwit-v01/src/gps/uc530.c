/*************************************************
 * FILE:    uc530.c
 * AUTHOR:  ADG
 * DESC:    UC530 NMEA PARSER
 *************************************************/
#include "uc530.h"
#include <math.h>

static void find_data(const char *c, int data_i[50]);

static int str2int(char* pStr, unsigned int len)
{
	int iRetVal = 0;
	int cnt = 0;
	
	if(pStr){
		while((cnt < len) && (*pStr <= '9') && (*pStr >= '0')){
			iRetVal = (iRetVal * 10) + (*pStr - '0');
			pStr++;
			cnt++;
		}
	}
	return iRetVal;
}

static float str2float(char* pStr){
	float iRetVal = 0;
	char *decP;
	
	if(pStr){
		while (*pStr && (*pStr <= '9') && (*pStr >= '0')){
			iRetVal = (iRetVal * 10) + (*pStr - '0');
			pStr++;
		}
	
		if(*pStr == '.'){
			decP = pStr++;
			
			while (*pStr && (*pStr <= '9') && (*pStr >= '0')){
				iRetVal = (iRetVal * 10) + (*pStr - '0');
				pStr++;
			}
			while(--pStr > decP){
				iRetVal = iRetVal/10;
			}
		}
	}
	
	return iRetVal;
}

int uc530_parse_default(char *str, unsigned int len, uc530_data_t *gps_data){
	char *nmea = str;
	unsigned int found_start = 0;
	
	int data_i[50];
	
	// Make sure we are in bounds
	while((nmea-str) < len){
		if(found_start){
			// Ignore standard 'GP'
			nmea += 2;
			
			// Find Data Indices
			find_data(nmea, data_i);
			gps_data->valid = 1;
			
			//GGA
			if((*nmea=='G')&&(*(nmea+1)=='G')&&(*(nmea+2)=='A')){
				// Fix Indicator
				if(data_i[5] != -1){
					gps_data->fix_status = str2int(nmea+data_i[5],10);
				}
				else
					gps_data->valid = 0;
				
				// Number of Satellites
				if(data_i[6] != -1){
					gps_data->num_sats = str2int(nmea+data_i[6],10);
				}
				else
					gps_data->valid = 0;
					
				// Altitude
				if(data_i[8] != -1){
					gps_data->alt = str2float(nmea+data_i[8]);
				}
				else
					gps_data->valid = 0;
			}
			//RMC
			else if((*nmea=='R')&&(*(nmea+1)=='M')&&(*(nmea+2)=='C')){
				// Time
				if(data_i[0] != -1){
					// Hours
					gps_data->hour = str2int(nmea+data_i[0], 2);
					
					// Minutes
					gps_data->minute = str2int(nmea+data_i[0]+2, 2);
					
					// Seconds
					gps_data->second = str2float(nmea+data_i[0]+4);
				}
				else
					gps_data->valid = 0;
					
				// Latitude
				if(data_i[2] != -1){
					// Degrees
					gps_data->lat.deg = str2int(nmea+data_i[2], 2);
					
					// Minutes
					gps_data->lat.min = str2float(nmea+data_i[2]+2);
				}
				else
					gps_data->valid = 0;
					
				// Latitude Hemi
				if(data_i[3] != -1){
					gps_data->lat.hemi = *(nmea+data_i[3]);
				}
				else
					gps_data->valid = 0;
					
				// Longitude
				if(data_i[4] != -1){
					// Degrees
					gps_data->longi.deg = str2int(nmea+data_i[4], 3);
					
					// Minutes
					gps_data->longi.min = str2float(nmea+data_i[4]+3);
				}
				else
					gps_data->valid = 0;
					
				// Longitude Hemi
				if(data_i[5] != -1){
					gps_data->longi.hemi = *(nmea+data_i[5]);
				}
				else
					gps_data->valid = 0;
					
				// Speed
				if(data_i[6] != -1){
					gps_data->spd = str2float(nmea+data_i[6]);
				}
				else
					gps_data->valid = 0;
					
				// Heading
				if(data_i[7] != -1){
					gps_data->hdg = str2float(nmea+data_i[7]);
				}
				else
					gps_data->valid = 0;
					
				// Date
				if(data_i[8] != -1){
					// Day
					gps_data->day = str2int(nmea+data_i[8], 2);
					
					// Month
					gps_data->month = str2int(nmea+data_i[8]+2, 2);
					
					// Year
					gps_data->year = str2int(nmea+data_i[8]+4, 10);
				}
				else
					gps_data->valid = 0;
			}
			//GSV
			else if((*nmea=='G')&&(*(nmea+1)=='S')&&(*(nmea+2)=='V')){
				
			}
			//GSA
			else if((*nmea=='G')&&(*(nmea+1)=='S')&&(*(nmea+2)=='A')){
				// PDOP
				if(data_i[14] != -1){
					gps_data->pdop = str2float(nmea+data_i[14]);
				}
				else
					gps_data->valid = 0;
					
				// HDOP
				if(data_i[15] != -1){
					gps_data->hdop = str2float(nmea+data_i[15]);
				}
				else
					gps_data->valid = 0;
					
				// VDOP
				if(data_i[16] != -1){
					gps_data->vdop = str2float(nmea+data_i[16]);
				}
				else
					gps_data->valid = 0;
			}
			
			found_start = 0;
		}
		else if(*nmea == '$')
			found_start = 1;
		
		// Increment to next character
		nmea++;
	}
	
	return UC530_SUCCESS;
}

// Finds indices of data fields between commas
// will list up to 50 indices or scan 200 characters
// If empty will have -1.
static void find_data(const char *str, int data_i[50]){
	unsigned int str_cnt = 0;
	unsigned int data_cnt = 0;
	
	while((str_cnt < 200) && (data_cnt < 50)){
		if(str[str_cnt] == ','){
			str_cnt++;
			
			// No Data
			if(str[str_cnt] == ',')
				data_i[data_cnt] = -1;
			// Mark index
			else
				data_i[data_cnt] = str_cnt;
				
			data_cnt++;
		}
		
		// End of NMEA Sentence
		else if(str[str_cnt] == '\r')
			return;
		
		else
			str_cnt++;
	}
}