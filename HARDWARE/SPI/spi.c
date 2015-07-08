#include "spi.h"

//u16 address[13]={0x1000,0x1200,0x1400,0x1600,0x1800,0x1a00,0x1c00,0x1e00,0x2000,0x2200,0x2400,0x2600,0X2600}; //X_GYRO_LOW,X_GYRO_OUT, Y_GYRO_LOW,Y_GYRO_OUT,Z_GYRO_LOW,Z_GYRO_OUT,
	                                                                           //X_ACCL_LOW,X_ACCL_OUT,Y_ACCL_LOW,Y_ACCL_OUT,Z_ACCL_LOW,Z_ACCL_OUT,
u16 address[13]={0x1200,0x1600,0x1a00,0x1e00,0x2200,0x2600,0x2600}; //X_GYRO_OUT, Y_GYRO_OUT,Z_GYRO_OUT,
	                                                          //X_ACCL_OUT,Y_ACCL_OUT,Z_ACCL_OUT,

extern int IMU_data[12];
extern u8 n;
void SPI1_init(void)
{
	RCC->APB2RSTR|=1<<12;   //复位SPI1
	RCC->APB2RSTR&=~(1<<12);//停止复位	
	RCC->APB2ENR|=1<<12;//使能SPI1时钟
	GPIOA->CRL&=0X000FFFFF;
	GPIOA->CRL|=0XB8B00000;   //设置IO口复用，MISO为带上拉输入，其他为推挽复用输出
	GPIOA->ODR|=0XF<<13;   	//PA4/5/6/7全部置高，PA4为片选
	//SPI1->CR1|=0<<10;		//全双工模式	
	SPI1->CR1|=1<<9; 		//软件nss管理
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; 		//SPI主机
	SPI1->CR1|=1<<11;		//16bit数据格式	
	SPI1->CR1|=1<<1; 		//空闲模式下SCK为1 CPOL=1
	SPI1->CR1|=1<<0; 		//数据采样从第二个时间边沿开始,CPHA=1  
	//对SPI1属于APB2的外设.时钟频率最大为72M.初始化APB2为72M
	SPI1->CR1|=2<<3; 		//010八分频，得SPI的速度为9MHZ
	SPI1->CR1|=0<<7; 		//MSBfirst   
//	SPI1->CR1|=1<<6; 		//SPI设备使能	
}

u16 SPI1_readwrite(u16 TxData)
{
	u16 retry=0;				 
	while((SPI1->SR&1<<1)==0)		//等待发送区空	
	{
		retry++;
		if(retry>=0XFFFE)return 0; 	//超时退出
	}			  
	SPI1->DR=TxData;	 	  		//发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) 		//等待接收完一个byte  
	{
		retry++;
		if(retry>=0XFFFE)return 0;	//超时退出
	}	  						    
	return SPI1->DR;          		//返回收到的数据			
}

															
void SPI1_data(void)
{
	u8 i;
	if(n==2)GPIOA->ODR&=0<<4;// 拉低片选
	SPI1_readwrite(0x8000);//激活第0页
	SPI1_readwrite(address[0]);
	for(i=1;i<6;i++)
	{
		IMU_data[i-1]=SPI1_readwrite(address[i]);	  // 读取数据		
	}
	//GPIOA->ODR|=1<<4;// 拉高片选		   //只取高16位
/*	for(i=1;i<12;i++)
	{
		IMU_data[i-1]=SPI1_readwrite(address[i]);	  // 读取数据		
	}
	//GPIOA->ODR|=1<<4;// 拉高片选		*/
}



