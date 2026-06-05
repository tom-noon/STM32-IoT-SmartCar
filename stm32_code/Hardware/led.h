#ifndef led_h
#define led_h

#include "sys.h"

#define LED0_on HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_SET)
#define LED0_off HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_RESET)

#define D0LED_on HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)
#define D0LED_off HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)
#define D1LED_on HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET)
#define D1LED_off HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET)


void externalLED_Config(void);
void internalLED_Config(void);


#endif

