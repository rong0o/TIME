 #include"gps.h"

extern u8 BUF[40];
//	 u8 start=0;
	 u8 end=0;
	 u8 count=0;
//	 u8 ing=0;
//	 u8 readend=0;
//extern u8 plan;
u8 temp=0;
u8 ok=0;
void Init_UBLOX_MSG(void)
{
	unsigned char MSG_ID[2]={0x06,0x01};
	unsigned char GGA_Payload[3]={0xf0,0x00,0x00}; //第三个0x00是数据率 HZ
	unsigned char GLL_Payload[3]={0xf0,0x01,0x00};
	unsigned char GSA_Payload[3]={0xf0,0x02,0x00};
	unsigned char GSV_Payload[3]={0xf0,0x03,0x00};
	unsigned char RMC_Payload[3]={0xf0,0x04,0x00};
	unsigned char VTG_Payload[3]={0xf0,0x05,0x00};
	unsigned char ZDA_Payload[3]={0xf0,0x08,0x01};


	UBLOX_CFG(MSG_ID,3,GGA_Payload);	//发送GPGGA配置指令
	UBLOX_CFG(MSG_ID,3,GGA_Payload);	//更改波特率后，第一条指令发送错误？？？？
	UBLOX_CFG(MSG_ID,3,GLL_Payload);	//发送GPGLL配置指令
	UBLOX_CFG(MSG_ID,3,GSA_Payload);	//发送GPGSA配置指令
	UBLOX_CFG(MSG_ID,3,GSV_Payload);	//发送GPGSV配置指令
	UBLOX_CFG(MSG_ID,3,RMC_Payload);
	UBLOX_CFG(MSG_ID,3,VTG_Payload);
	UBLOX_CFG(MSG_ID,3,ZDA_Payload);
}

void Init_UBLOX_TP(void)
{
	unsigned char TP_ID[2]={0x06,0x31};
	unsigned char TP_Payload[32]={
	0x01,0x00,0x00,0x00,//0-3,tpIdx=0,timepulse,reserved0,reserved1   
	50&0xff,(50>>8)&0xff,0&0xff,(0>>8)&0xff,//4-7,antCableDelay=50ns,rfGroupDelay=0 
	1000000&0xff,(1000000>>8)&0xff,(1000000>>16)&0xff,(1000000>>24)&0xff,//8-11，PPS freqPeriod=1000000us=1s,IsFreq=0;
	1000000&0xff,(1000000>>8)&0xff,(1000000>>16)&0xff,(1000000>>24)&0xff,//12-15，freqPeriodLoc=1000000us=1S
	100000&0xff,(100000>>8)&0xff,(100000>>16)&0xff,(100000>>24)&0xff,//16-19,	pulseLenRatio=100000us=100ms
	100000&0xff,(100000>>8)&0xff,(100000>>16)&0xff,(100000>>24)&0xff,//20-23,	pulseLenRatioLock=100ms,
	10&0xff,(10>>8)&0xff,(10>>16)&0xff,(10>>24)&0xff,//24-27,	userConfigDelay=10ns
	BIT0|BIT1|BIT2|BIT4|BIT5|BIT6,0x00,0x00,0x00//Active=1,LockGpsFreq=1,lockedOtherSet=1,isFreq=0,isLength=1,alignToTow=1,polarity=1,gridUtcGps=0,  UTC对齐
	};
	UBLOX_CFG(TP_ID,32,TP_Payload);
}



void UBLOX_CFG(unsigned char * ID,short int  Length,unsigned char *Payload)
{
	int i=0;
	unsigned char CK_A=0;
	unsigned char CK_B=0;
/*
	CK_A=0xff;
	CK_B=0x02;
	CK_A=CK_A+CK_B;
*/
//	while(512-GetUartTXFIFOusedw(UBLOX_UART_STATUS_ADDR)<Length+7);//等待发送缓存容得下一个指令
	//写数据到缓存
	USART3->DR=0xB5;
	while((USART3->SR&0X40)==0);//等待发送结束
	USART3->DR=0x62;
	while((USART3->SR&0X40)==0);//等待发送结束
	USART3->DR=* ID;
	while((USART3->SR&0X40)==0);//等待发送结束
	USART3->DR=* (ID+1);
	while((USART3->SR&0X40)==0);//等待发送结束
	USART3->DR=(Length&0xff);
	while((USART3->SR&0X40)==0);//等待发送结束
	USART3->DR=((Length>>8)&0xff);
	while((USART3->SR&0X40)==0);//等待发送结束
	for(i=0;i<Length;i++)
	{	USART3->DR=* (Payload+i);
		while((USART3->SR&0X40)==0);//等待发送结束
	 }
	//计算校验值
	for(i=0;i<2;i++)
	{
	CK_A=CK_A+(* (ID+i));
	CK_B=CK_B+CK_A;
	}
	
	CK_A=CK_A+(Length&0xff);
	CK_B=CK_B+CK_A;

	CK_A=CK_A+((Length>>8)&0xff);
	CK_B=CK_B+CK_A;

	for (i=0;i<Length;i++)
	{
	CK_A=CK_A+(* (Payload+i));
	CK_B=CK_B+CK_A;
	}

	//写校验值到缓存
	USART3->DR=CK_A;
	while((USART3->SR&0X40)==0);//等待发送结束
	USART3->DR=CK_B;
	while((USART3->SR&0X40)==0);//等待发送结束
}

void GPS_ready(void)
{
	 //u8 n;
	 u8 gpsready=0;
	 u8 i=0,j=0;
	 u8 second[20];
	 USART3->CR1|=1<<8;    //PE中断使能	,,开接收中断
	 USART3->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	 MY_NVIC_Init(3,3,USART3_IRQn,2);//组2，最低优先级 
	 while(gpsready==0)
	 {

		if(end==1&&ok==1)
		{
	
			second[i]=BUF[12];				
			if(i>=1)
			{
				if((second[i]-second[i-1])==1)j++; 
			}				 
		if(j==3)	
		{gpsready=1;			
	    }
		i++;
		count=0;
		end=0;
		ok=0;
	
		}
	 }
	 //USART3->CR1&=0<<8;    //关接收中断
	 //USART3->CR1&=0<<2;	 //关闭接收
	// for(n=0;n<=40;n++)
	//	BUF[n]=0;	//清空缓存
}	  

void USART3_IRQHandler(void)
{
	 


	 	/*	temp=USART3->DR; 
			if(temp=='$')
			{
				start=1;
				readend=0;
			}
			if(start==1)
			{
				ing=1;
				BUF[count]=temp;
				count++;
			}
			if(temp=='*'&&ing==1)
			{
				end=1;
				start=0;
				ing=0;
			}		 */
		/*	temp=USART3->DR; 
			if(temp=='$')
			{
				start=1;
			}
			if(start==1)
			{
				BUF[count]=temp;
					count++;
			}
			if(BUF[count-1]==0x0d&&BUF[count]==0x0a)end=1;	   */
		
		 temp=USART3->DR; 
		 if(temp=='$')
			{
				ok=1;
			}
		 BUF[count]=temp;
		if(BUF[count-1]==0x0d&&BUF[count]==0x0a)end=1;
		count++;


}














