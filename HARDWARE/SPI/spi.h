#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
 													  
void SPI1_init(void);			 //��ʼ��SPI2�� 
//u8 SPI2_ReadWriteByte(u8 TxData);//SPI2���߶�дһ���ֽ�
u16 SPI1_readwrite(u16 TxData);
void SPI1_data(void);
		 
#endif

