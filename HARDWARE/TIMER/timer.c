#include"timer.h"
void TIM3_init(u16 psc)
{  
 	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ�� 
	TIM3->ARR=65535;  	//�趨�������Զ���װֵ,װ��   
	TIM3->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  	  
	//TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3							 
}











