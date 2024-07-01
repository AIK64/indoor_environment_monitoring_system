#include "stm32f10x.h"                  // Device header


void LED_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体变量
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;				//GPIO引脚，赋值为第13号引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将赋值后的构体变量传递给GPIO_Init函数
	
	GPIO_SetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_2);
}

void LED1_on(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}

void LED1_off(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}
void LED1_turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1)==0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
	}
	else{
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	}
}


void LED2_on(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}

void LED2_off(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}

void LED2_turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_2)==0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
	}
	else{
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
}
