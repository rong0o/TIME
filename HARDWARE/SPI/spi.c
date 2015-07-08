#include "spi.h"

//u16 address[13]={0x1000,0x1200,0x1400,0x1600,0x1800,0x1a00,0x1c00,0x1e00,0x2000,0x2200,0x2400,0x2600,0X2600}; //X_GYRO_LOW,X_GYRO_OUT, Y_GYRO_LOW,Y_GYRO_OUT,Z_GYRO_LOW,Z_GYRO_OUT,
	                                                                           //X_ACCL_LOW,X_ACCL_OUT,Y_ACCL_LOW,Y_ACCL_OUT,Z_ACCL_LOW,Z_ACCL_OUT,
u16 address[13]={0x1200,0x1600,0x1a00,0x1e00,0x2200,0x2600,0x2600}; //X_GYRO_OUT, Y_GYRO_OUT,Z_GYRO_OUT,
	                                                          //X_ACCL_OUT,Y_ACCL_OUT,Z_ACCL_OUT,

extern int IMU_data[12];
extern u8 n;
void SPI1_init(void)
{
	RCC->APB2RSTR|=1<<12;   //��λSPI1
	RCC->APB2RSTR&=~(1<<12);//ֹͣ��λ	
	RCC->APB2ENR|=1<<12;//ʹ��SPI1ʱ��
	GPIOA->CRL&=0X000FFFFF;
	GPIOA->CRL|=0XB8B00000;   //����IO�ڸ��ã�MISOΪ���������룬����Ϊ���츴�����
	GPIOA->ODR|=0XF<<13;   	//PA4/5/6/7ȫ���øߣ�PA4ΪƬѡ
	//SPI1->CR1|=0<<10;		//ȫ˫��ģʽ	
	SPI1->CR1|=1<<9; 		//���nss����
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; 		//SPI����
	SPI1->CR1|=1<<11;		//16bit���ݸ�ʽ	
	SPI1->CR1|=1<<1; 		//����ģʽ��SCKΪ1 CPOL=1
	SPI1->CR1|=1<<0; 		//���ݲ����ӵڶ���ʱ����ؿ�ʼ,CPHA=1  
	//��SPI1����APB2������.ʱ��Ƶ�����Ϊ72M.��ʼ��APB2Ϊ72M
	SPI1->CR1|=2<<3; 		//010�˷�Ƶ����SPI���ٶ�Ϊ9MHZ
	SPI1->CR1|=0<<7; 		//MSBfirst   
//	SPI1->CR1|=1<<6; 		//SPI�豸ʹ��	
}

u16 SPI1_readwrite(u16 TxData)
{
	u16 retry=0;				 
	while((SPI1->SR&1<<1)==0)		//�ȴ���������	
	{
		retry++;
		if(retry>=0XFFFE)return 0; 	//��ʱ�˳�
	}			  
	SPI1->DR=TxData;	 	  		//����һ��byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) 		//�ȴ�������һ��byte  
	{
		retry++;
		if(retry>=0XFFFE)return 0;	//��ʱ�˳�
	}	  						    
	return SPI1->DR;          		//�����յ�������			
}

															
void SPI1_data(void)
{
	u8 i;
	if(n==2)GPIOA->ODR&=0<<4;// ����Ƭѡ
	SPI1_readwrite(0x8000);//�����0ҳ
	SPI1_readwrite(address[0]);
	for(i=1;i<6;i++)
	{
		IMU_data[i-1]=SPI1_readwrite(address[i]);	  // ��ȡ����		
	}
	//GPIOA->ODR|=1<<4;// ����Ƭѡ		   //ֻȡ��16λ
/*	for(i=1;i<12;i++)
	{
		IMU_data[i-1]=SPI1_readwrite(address[i]);	  // ��ȡ����		
	}
	//GPIOA->ODR|=1<<4;// ����Ƭѡ		*/
}



