#ifndef __HCSRO4_H_
#define __HCSRO4_H_

#include "main.h"
#include "delay.h"

typedef struct
{
	uint8_t  edge_state;
	uint16_t tim_overflow_counter;
	uint32_t prescaler;
	uint32_t period;
	uint32_t t1;	//	上升沿时间
	uint32_t t2;	//	下降沿时间
	uint32_t high_level_us;	//	高电平持续时间
	float    distance;
	TIM_TypeDef* instance;
  uint32_t ic_tim_ch;
	HAL_TIM_ActiveChannel active_channel;
}Hcsro4InfoTypeDef;

extern Hcsro4InfoTypeDef Hcsro4Info;

/**
 * @description: 超声波模块的输入捕获定时器通道初始化
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsro4Init(TIM_HandleTypeDef *htim, uint32_t Channel);

/**
 * @description: HC-SRO4触发
 * @param {*}
 * @return {*}
 */
void Hcsro4Start(void);

/**
 * @description: 定时器计数溢出中断处理函数
 * @param {*}    main.c中重定义void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
 * @return {*}
 */
void Hcsro4TimOverflowIsr(TIM_HandleTypeDef *htim);

/**
 * @description: 输入捕获计算高电平时间->距离
 * @param {*}    main.c中重定义void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
 * @return {*}
 */
void Hcsro4TimIcIsr(TIM_HandleTypeDef* htim);

/**
 * @description: 读取距离 
 * @param {*}
 * @return {*}
 */
float Hcsro4Read(void);

#endif /* Hcsro4_H_ */

