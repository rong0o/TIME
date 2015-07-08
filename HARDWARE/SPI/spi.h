#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
 													  
void SPI1_init(void);			 //初始化SPI2口 
//u8 SPI2_ReadWriteByte(u8 TxData);//SPI2总线读写一个字节
u16 SPI1_readwrite(u16 TxData);
void SPI1_data(void);
		 
#endif

