#include"sys.h"
#include"usart.h"
#include"exti.h"
#include"spi.h"
#include"timer.h"
#include"gps.h"
#include<stdlib.h>

#define LED0 PEout(5)
u8 GPS_flag=0;
u8 IMU_flag=0;
u8 BUF[40];
u8 n=0;
//u8 plan=0;
extern u8 end;
extern u8 count;
extern u8 ok;
int IMU_data[6];
u8 IMU_data1[6][6];
//int IMU_data[12];
//u8 IMU_data1[12][5];
//u8 display[50];
u16 time,time1,time2,time3,displaytime1,displaytime2;
//u8 m;
u8 displaytime[4];
u8 remark[]={"Output Format:time,X_GYRO_OUT,Y_GYRO_OUT,Z_GYRO_OUT,X_ACCL_OUT,Y_ACCL_OUT,Z_ACCL_OUT.$"};

				

void transform_5(int p,u8 *d);
void transform_4(u16 p,u8 *d);


int main(void)
{
	u8 first_flag;
	u8 i,j=0;
	Stm32_Clock_Init(9);
	uart1_init(72,256000);//ʱ��72MHZ,������4500000
	uart3_init(36,9600);  //ʱ��36MHZ,������9600
	SPI1_init();
	TIM3_init(7199);  //Ԥ��Ƶ7200���õ�10KHZ�ļ���ʱ��
	/*LED��ʼ��*/
	GPIOE->CRL&=0XFF0FFFFF;
	GPIOE->CRL|=0X00300000;
	LED0=0;	
	exit_PPSA_init();
	exit_IMU_init();	   
	Init_UBLOX_MSG();
	Init_UBLOX_TP();
	GPS_ready();
	EXTI->IMR|=1<<11 ;	//���ⲿPPSA�ж�
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3		
	while(1)
	{
		if(GPS_flag==1)
		{
			GPS_flag=0;
			if(n==0)
			{ 	 	
			//	plan=1;
			//	end=0;
				//count=0;
				//ok=0;
				//USART3->CR1|=1<<2; //�����ڽ���
				//USART3->CR1|=1<<8;//�������ж�
			//	while(end==0||BUF[count-1]!=0X0A||BUF[count-2]!=0x0D);	//�ȴ�����ȫ��������� 
				while(end==0||ok==0);
				USART3->CR1&=0<<8;    //�ؽ����ж�
				RCC->APB1ENR&=0<<18;  //��GPS����3
				for(i=0;i<count;i++)		
				{
					USART1->DR=BUF[i];
					while((USART1->SR&0X40)==0);//�ȴ����ͽ���
				}
				i=0;
				while(remark[i]!='$')
				{
					USART1->DR=remark[i];
					while((USART1->SR&0X40)==0);//�ȴ����ͽ���
					i++;
				}
						 //���봫�����ʱ��������ʽ������
				printf("\r\n");
				i=0;
				EXTI->IMR|=1<<8;//��IMU�ж�
				SPI2->CR1|=1<<6; 		//SPI�豸ʹ��

			}
			 
			 n++;
			 LED0=!LED0;
			 time=TIM3->CNT;
			 first_flag=1;
		}
		if(IMU_flag==1)
		{
			IMU_flag=0;
			/*��ȡIMU����*/	
			 SPI1_data();
			/*����ʱ��*/
			if(first_flag==1)
			{
				first_flag=0;
				time1=time;
			}
			else time1=time2 ;
			time2=TIM3->CNT;
			if(time2>time1)time3=time2-time1;
			else time3=time1-time2;
			if(first_flag==1)
			{
				first_flag=0;
				displaytime2=time3 ;
			}
			else 
			{
				displaytime1=displaytime2;
				displaytime2=displaytime1+time3;	//�������IMU������ʱ������ȡС�������λ
			}
		/*ʱ�����ת��*/
		transform_4(displaytime2,displaytime);
		/*���ݽ���ת��*/
		for(i=0;i<6;i++)
		{
		transform_5(IMU_data[i],IMU_data1[i]) ;
		}		  
		/*for(i=0;i<12;i++)
		{
		transform_5(IMU_data[i],IMU_data1[i]) ;
		}		  */
		/*���䴦����������*/
		USART1->DR=n-1;
		while((USART1->SR&0X40)==0);//�ȴ����ͽ���
		USART1->DR='.' ;
		for(i=0;i<4;i++)		
		{
			USART1->DR=displaytime[i];
			while((USART1->SR&0X40)==0);//�ȴ����ͽ���
		}
		i=0;
		USART1->DR=',';
		while((USART1->SR&0X40)==0);//�ȴ����ͽ���	
		for(i=0;i<6;i++)
		{
			for(j=0;j<6;j++)
			{
				USART1->DR=IMU_data1[i][j];
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���		
			}
			USART1->DR=',';
			while((USART1->SR&0X40)==0);//�ȴ����ͽ���	
		}	
		/*for(i=0;i<12;i++)
		{
			for(j=0;j<5;j++)
			{
				USART1->DR=IMU_data1[i][j];
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���		
			}
			USART1->DR=',';
			while((USART1->SR&0X40)==0);//�ȴ����ͽ���	
		}*/		
		i=0;
		printf("\r\n");
		

		}
	}

}

void transform_5(int p,u8 *d)
{
	  
	 /*����ת��ΪASCII��*/
	  u16 temp;
	  temp=p&0x8000;
	  if(temp==0x8000)*d='-'	;
	  else *d='+' ;
	  p=abs(p);
	  *(d+1)=p/10000+0x30;
	  *(d+2)=(p%10000)/1000+0x30;
	  *(d+3)=(p%1000)/100+0x30;
	  *(d+4)=(p%100)/10+0x30;
	  *(d+5)=p%10+0x30;		 
	  /**d=p/10000;
	  *(d+1)=(p%10000)/1000;
	  *(d+2)=(p%1000)/100;
	  *(d+3)=(p%100)/10;
	  *(d+4)=p%10;		 */
}

void transform_4(u16 p,u8 *d)
{
	 /*����ת��ΪASCII��*/
	 *d=p/1000+0x30;
	 *(d+1)=(p%1000)/100+0x30;
	 *(d+2)=(p%100)/10+0x30;
	 *(d+3)=p%10+0x30;	
	/**d=p/1000;
	 *(d+1)=(p%1000)/100;
	 *(d+2)=(p%100)/10;
	 *(d+3)=p%10;	*/
}

