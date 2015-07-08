#ifndef __GPS_H
#define __GPS_H
#include "sys.h"

#define BIT0 0X01
#define BIT1 0X01<<1
#define BIT2 0X01<<2
#define BIT3 0X01<<3
#define BIT4 0X01<<4
#define BIT5 0X01<<5
#define BIT6 0X01<<6
								  
void UBLOX_CFG(unsigned char * ID,short int  Length,unsigned char *Payload);
void Init_UBLOX_MSG(void);
void Init_UBLOX_TP(void);
void GPS_ready(void);
#endif
