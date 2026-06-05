#include "motor.h"

extern uint8_t isrun;

//俩车轮引脚初始化
void MOTOR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = IN1_PIN|IN2_PIN|EN1_PIN|EN2_PIN|IN3_PIN|IN4_PIN;	 
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; //速度50MHz
 	HAL_GPIO_Init(MOTOR_PORT, &GPIO_InitStructure);	  //初始化GPIOE 012345
}

/*
	控制motor的转速-pwm调波
	PWM_SIZE 一周期的长度
	PWM_VALUE1 一周期内的有效时间
	size<--->value期间off  value<--->0期间on   value和size越接近劲越大
*/
u8 PWM_COUNT1=0,PWM_VALUE1=0,PWM_COUNT2=0,PWM_VALUE2=0;
#define PWM_SIZE 10  //5个挡位 0 2 4 6 8  易区分
//u8 pwm_i=0;
void Motor_pwm(void)
{
	
	if( (PWM_COUNT1 == PWM_VALUE1) && (PWM_VALUE1!=0) )    //do
	{
		EN1_on;
	}
	else if(PWM_COUNT1 == 0)             //restart
	{
		EN1_off;
		PWM_COUNT1 = PWM_SIZE;              //重新计时
	}
	
	if( (PWM_COUNT2 == PWM_VALUE2) && (PWM_VALUE2!=0) )    //do
	{
		EN2_on;
	}
	else if(PWM_COUNT2 == 0)             //restart
	{
		EN2_off;
		PWM_COUNT2 = PWM_SIZE;              //重新计时
	}
	
//呼吸效果
//		if(pwm_i%2==0) PWM_VALUE3++;
//		if(pwm_i%2==1) PWM_VALUE3--;
//		
//		if((PWM_VALUE3==PWM_SIZE3-1)||(PWM_VALUE3==1)) pwm_i++;
	
	PWM_COUNT1--;PWM_COUNT2--;
	
}
//设置速度
void Set_Speed(uint8_t speed)
{
	PWM_VALUE1=speed*2;
	PWM_VALUE2=speed*2;
}
//设置左轮速度
void Set_LeftSpeed(uint8_t speed)
{
	PWM_VALUE1=speed;
}
//设置右轮速度
void Set_RightSpeed(uint8_t speed)
{
	PWM_VALUE2=speed;
}

//小车向前
void Motor_forward()
{
	Left_motor_go;   //左电机前转
	Right_motor_go;  //右电机前转
	isrun=1;
}

//小车向后
void Motor_backward()
{
	Left_motor_goback;   //左电机后转
	Right_motor_goback;  //右电机后转
	isrun=1;
}

//小车向左转向 此函数仅完成左转动作 后接向前、向后或停止  Motor_leftward();turn_time=1000;Motor_forward();
void Motor_leftward(void)
{
	Left_motor_stop;   //左电机停
	Right_motor_go;    //右电机前转
	isrun=1;
}

//小车向右转向 此函数仅完成右转动作 后接向前、向后或停止
void Motor_rightward(void)
{
	Left_motor_go;     //左电机前转
	Right_motor_stop;  //右电机停
	isrun=1;
}

//小车停止
void Motor_stop(void)
{
	Left_motor_stop;   //左电机停
	Right_motor_stop;  //右电机停
}

