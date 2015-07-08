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
	uart1_init(72,256000);//时钟72MHZ,波特率4500000
	uart3_init(36,9600);  //时钟36MHZ,波特率9600
	SPI1_init();
	TIM3_init(7199);  //预分频7200，得到10KHZ的计数时钟
	/*LED初始化*/
	GPIOE->CRL&=0XFF0FFFFF;
	GPIOE->CRL|=0X00300000;
	LED0=0;	
	exit_PPSA_init();
	exit_IMU_init();	   
	Init_UBLOX_MSG();
	Init_UBLOX_TP();
	GPS_ready();
	EXTI->IMR|=1<<11 ;	//开外部PPSA中断
	TIM3->CR1|=0x01;    //使能定时器3		
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
				//USART3->CR1|=1<<2; //开串口接收
				//USART3->CR1|=1<<8;//开串口中断
			//	while(end==0||BUF[count-1]!=0X0A||BUF[count-2]!=0x0D);	//等待数据全部传输完毕 
				while(end==0||ok==0);
				USART3->CR1&=0<<8;    //关接收中断
				RCC->APB1ENR&=0<<18;  //关GPS串口3
				for(i=0;i<count;i++)		
				{
					USART1->DR=BUF[i];
					while((USART1->SR&0X40)==0);//等待发送结束
				}
				i=0;
				while(remark[i]!='$')
				{
					USART1->DR=remark[i];
					while((USART1->SR&0X40)==0);//等待发送结束
					i++;
				}
						 //首秒传输绝对时间和输出格式给电脑
				printf("\r\n");
				i=0;
				EXTI->IMR|=1<<8;//开IMU中断
				SPI2->CR1|=1<<6; 		//SPI设备使能

			}
			 
			 n++;
			 LED0=!LED0;
			 time=TIM3->CNT;
			 first_flag=1;
		}
		if(IMU_flag==1)
		{
			IMU_flag=0;
			/*读取IMU数据*/	
			 SPI1_data();
			/*处理时间*/
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
				displaytime2=displaytime1+time3;	//处理出两IMU脉冲间的时间差，精度取小数点后四位
			}
		/*时间进制转换*/
		transform_4(displaytime2,displaytime);
		/*数据进制转换*/
		for(i=0;i<6;i++)
		{
		transform_5(IMU_data[i],IMU_data1[i]) ;
		}		  
		/*for(i=0;i<12;i++)
		{
		transform_5(IMU_data[i],IMU_data1[i]) ;
		}		  */
		/*传输处理结果给电脑*/
		USART1->DR=n-1;
		while((USART1->SR&0X40)==0);//等待发送结束
		USART1->DR='.' ;
		for(i=0;i<4;i++)		
		{
			USART1->DR=displaytime[i];
			while((USART1->SR&0X40)==0);//等待发送结束
		}
		i=0;
		USART1->DR=',';
		while((USART1->SR&0X40)==0);//等待发送结束	
		for(i=0;i<6;i++)
		{
			for(j=0;j<6;j++)
			{
				USART1->DR=IMU_data1[i][j];
				while((USART1->SR&0X40)==0);//等待发送结束		
			}
			USART1->DR=',';
			while((USART1->SR&0X40)==0);//等待发送结束	
		}	
		/*for(i=0;i<12;i++)
		{
			for(j=0;j<5;j++)
			{
				USART1->DR=IMU_data1[i][j];
				while((USART1->SR&0X40)==0);//等待发送结束		
			}
			USART1->DR=',';
			while((USART1->SR&0X40)==0);//等待发送结束	
		}*/		
		i=0;
		printf("\r\n");
		

		}
	}

}

void transform_5(int p,u8 *d)
{
	  
	 /*加上转换为ASCII码*/
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
	 /*加上转换为ASCII码*/
	 *d=p/1000+0x30;
	 *(d+1)=(p%1000)/100+0x30;
	 *(d+2)=(p%100)/10+0x30;
	 *(d+3)=p%10+0x30;	
	/**d=p/1000;
	 *(d+1)=(p%1000)/100;
	 *(d+2)=(p%100)/10;
	 *(d+3)=p%10;	*/
}

