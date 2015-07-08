#include"timer.h"
void TIM3_init(u16 psc)
{  
 	RCC->APB1ENR|=1<<1;	//TIM3时钟使能 
	TIM3->ARR=65535;  	//设定计数器自动重装值,装满   
	TIM3->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  	  
	//TIM3->CR1|=0x01;    //使能定时器3							 
}











