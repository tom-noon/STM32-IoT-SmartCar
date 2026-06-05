#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"


#define MOTOR_PORT        GPIOE
#define IN1_PIN           GPIO_PIN_0
#define IN2_PIN           GPIO_PIN_1
#define EN1_PIN           GPIO_PIN_2
#define EN2_PIN           GPIO_PIN_3
#define IN3_PIN           GPIO_PIN_4
#define IN4_PIN           GPIO_PIN_5

//PE0-PE5
#define IN1_on            HAL_GPIO_WritePin(MOTOR_PORT,IN1_PIN,GPIO_PIN_SET)//左往后
#define IN1_off           HAL_GPIO_WritePin(MOTOR_PORT,IN1_PIN,GPIO_PIN_RESET)
#define IN2_on            HAL_GPIO_WritePin(MOTOR_PORT,IN2_PIN,GPIO_PIN_SET)//左往前
#define IN2_off           HAL_GPIO_WritePin(MOTOR_PORT,IN2_PIN,GPIO_PIN_RESET)
#define EN1_on            HAL_GPIO_WritePin(MOTOR_PORT,EN1_PIN,GPIO_PIN_SET)//左使能
#define EN1_off           HAL_GPIO_WritePin(MOTOR_PORT,EN1_PIN,GPIO_PIN_RESET)
#define EN2_on            HAL_GPIO_WritePin(MOTOR_PORT,EN2_PIN,GPIO_PIN_SET)//右使能
#define EN2_off           HAL_GPIO_WritePin(MOTOR_PORT,EN2_PIN,GPIO_PIN_RESET)
#define IN3_on            HAL_GPIO_WritePin(MOTOR_PORT,IN3_PIN,GPIO_PIN_SET)//右往前
#define IN3_off           HAL_GPIO_WritePin(MOTOR_PORT,IN3_PIN,GPIO_PIN_RESET)
#define IN4_on            HAL_GPIO_WritePin(MOTOR_PORT,IN4_PIN,GPIO_PIN_SET)//右往后
#define IN4_off           HAL_GPIO_WritePin(MOTOR_PORT,IN4_PIN,GPIO_PIN_RESET)

//EN为1 IN值才有效
#define Left_motor_go       { IN1_on; IN2_off; }  //左电机向前走
#define Left_motor_goback   { IN1_off; IN2_on; }  //左电机向后走
#define Right_motor_go      { IN3_off; IN4_on; }  //右电机向前走
#define Right_motor_goback  { IN3_on; IN4_off; }  //右电机向后走

#define Left_motor_stop     { IN1_off; IN2_off; } //停止
#define Right_motor_stop    { IN3_off; IN4_off; } //停止

//控制motor的转速
void Motor_pwm(void);

//引脚初始化
void MOTOR_Init(void);
//设置速度
void Set_Speed(uint8_t speed);
//设置左轮速度
void Set_LeftSpeed(uint8_t speed);
//设置右轮速度
void Set_RightSpeed(uint8_t speed);
//小车向前
void Motor_forward(void);
//小车向后
void Motor_backward(void);
//小车向左转向 此函数仅完成左转动作 后接向前、向后或停止
void Motor_leftward(void);
//小车向右转向 此函数仅完成右转动作 后接向前、向后或停止
void Motor_rightward(void);
//小车停止
void Motor_stop(void);

#endif

