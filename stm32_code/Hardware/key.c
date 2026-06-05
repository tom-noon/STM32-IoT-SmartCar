#include "key.h"


void KEY_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();//使能GPIOE的时钟信号
	
	GPIO_InitStruct.Pin=GPIO_PIN_3|GPIO_PIN_4;// PE3对应key1  PE4对应key0  对应的按键按下，该端口就被拉低
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT; GPIO_InitStruct.Pull=GPIO_PULLUP;//上拉输入
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE,&GPIO_InitStruct);
}

//KEY关键函数        KEY0--PE4       KEY1--PE3
uint8_t key_scan(void)
{
	static uint8_t key,state;
	switch(state)
	{
		case 0:
			if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3) == GPIO_PIN_RESET)
			{
				state=1;//有按键按下
			}
			key=0;
			break;
		case 1://
			if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4) == GPIO_PIN_SET && HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3) == GPIO_PIN_SET)
			{
				state=0;//停留时间过短--误触类似
			}
			else
			{
				if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4) == GPIO_PIN_RESET)
					key=11;
				else if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3) == GPIO_PIN_RESET)
					key=12;
				state=2;//正常按下
			}
			break;
		case 2:
			if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4) == GPIO_PIN_SET && HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3) == GPIO_PIN_SET)
			{
				key=key-10;
				
				state=0;//松开后--完成一次按键事件
			}
			break;
		default:
			break;
	}
	return key;
}
