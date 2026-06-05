#include "led.h"

void externalLED_Config(void)//led配置函数,外接led灯的配置程序
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();//使能GPIOE的时钟信号
	
	GPIO_InitStruct.Pin=GPIO_PIN_9;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//推挽输出
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE,&GPIO_InitStruct);
}//本程序将S端接在了PE9，如果改变接口，需要改变函数内容

void internalLED_Config(void)//板载led灯配置程序
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();//使能GPIOE的时钟信号
	__HAL_RCC_GPIOB_CLK_ENABLE();//使能GPIOB的时钟信号
	
	GPIO_InitStruct.Pin=GPIO_PIN_5;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//推挽输出
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE,&GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);//上电关闭板载LED D1
	
	GPIO_InitStruct.Pin=GPIO_PIN_5;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//推挽输出
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);//上电关闭板载LED D0
}
