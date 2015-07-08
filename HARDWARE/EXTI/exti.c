 #include"exti.h"
 /*void exti_init(void)
 {
 	//RCC->APB2ENR|=1<<2;	//ʹ��PA��ʱ��
	RCC->APB2ENR|=1; //ʹ�ܸ���ʱ��	  
	GPIOA->CRH&=0XFFFF0FFF;
	GPIOA->CRH|=0X00008008;//PA8,PA11����Ϊ����		  
	APIO->EXTICR[2]&=0XFFFF0FF0;
	EXTI->RTSR|=1<<11;	//PA0�����ش���
	EXTI->RTSR|=1<<8; //PA8�����ش���
	MY_NVIC_Init(2,2,EXTI8_IRQn,2);
	MY_NVIC_Init(1,2,EXTI11_IRQn,2);
	EXTI->IMR|=1<<11;
	EXTI->IMR|=1<<8;
 }		*/

extern u8 GPS_flag;
extern u8 IMU_flag;
extern u8 n;

void exit_PPSA_init(void)
{
	RCC->APB2ENR|=1;//IO�ڸ������ܿ�
	GPIOA->CRH&=0XFFFF0FFF;
	GPIOA->CRH|=0X00008000;
	EXTI->RTSR|=1<<11;	//PA0�����ش���	
	MY_NVIC_Init(1,2,EXTI15_10_IRQn,2);
	//EXTI->IMR|=1<<11 ;
}

void exit_IMU_init(void)
{
	GPIOA->CRH&=0XFFFFFFF0;
	GPIOA->CRH|=0X00000008;
	EXTI->RTSR|=1<<8; //PA8�����ش���
	MY_NVIC_Init(2,2,EXTI9_5_IRQn,2);
	//EXTI->IMR|=1<<8;
}
void EXTI15_10_IRQHandler(void)
{
	 EXTI->PR|=1<<11;
	 GPS_flag=1;
}		


void EXTI9_5_IRQHandler(void)
{
	 EXTI->PR|=1<<8;
	 if(n>1)IMU_flag=1;
}


