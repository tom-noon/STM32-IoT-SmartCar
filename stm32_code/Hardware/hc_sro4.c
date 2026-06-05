#include "hc_sro4.h"

Hcsro4InfoTypeDef Hcsro4Info;

/**
 * @description: 超声波模块的输入捕获定时器通道初始化
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsro4Init(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /*--------[ Configure The Hcsro4 IC Timer Channel ] */
  // MX_TIM2_Init();  // cubemx中配置
  Hcsro4Info.prescaler = htim->Init.Prescaler; //  72-1
  Hcsro4Info.period = htim->Init.Period;       //  65535

  Hcsro4Info.instance = htim->Instance;        //  TIM2
  Hcsro4Info.ic_tim_ch = Channel;
  if(Hcsro4Info.ic_tim_ch == TIM_CHANNEL_1)
  {
    Hcsro4Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_1;             //  TIM_CHANNEL_4
  }
  else if(Hcsro4Info.ic_tim_ch == TIM_CHANNEL_2)
  {
    Hcsro4Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_2;             //  TIM_CHANNEL_4
  }
  else if(Hcsro4Info.ic_tim_ch == TIM_CHANNEL_3)
  {
    Hcsro4Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_3;             //  TIM_CHANNEL_4
  }
  else if(Hcsro4Info.ic_tim_ch == TIM_CHANNEL_4)
  {
    Hcsro4Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_4;             //  TIM_CHANNEL_4
  }
  /*--------[ Start The ICU Channel ]-------*/
  HAL_TIM_Base_Start_IT(htim);
  HAL_TIM_IC_Start_IT(htim, Channel);//在中断模式下启动 TIM 输入捕获测量。
}

/**
 * @description: HC-SR04触发
 * @param {*}
 * @return {*}
 */
void Hcsro4Start(void)
{
  HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);
  delay_us(15);  //  10us以上
  HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
}

/**
 * @description: 定时器计数溢出中断处理函数
 * @param {*}    main.c中重定义void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
 * @return {*}
 */
void Hcsro4TimOverflowIsr(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == Hcsro4Info.instance) //  TIM2
  {
    Hcsro4Info.tim_overflow_counter++;
  }
}

/**
 * @description: 输入捕获计算高电平时间->距离
 * @param {*}    main.c中重定义void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
 * @return {*}
 */
void Hcsro4TimIcIsr(TIM_HandleTypeDef* htim)
{
  if((htim->Instance == Hcsro4Info.instance) && (htim->Channel == Hcsro4Info.active_channel))
  {
    if(Hcsro4Info.edge_state == 0)      //  捕获上升沿
    {
      // 得到上升沿开始时间T1，并更改输入捕获为下降沿
      Hcsro4Info.t1 = HAL_TIM_ReadCapturedValue(htim, Hcsro4Info.ic_tim_ch);
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, Hcsro4Info.ic_tim_ch, TIM_INPUTCHANNELPOLARITY_FALLING);
      Hcsro4Info.tim_overflow_counter = 0;  //  定时器溢出计数器清零
      Hcsro4Info.edge_state = 1;        //  上升沿、下降沿捕获标志位
    }
    else if(Hcsro4Info.edge_state == 1) //  捕获下降沿
    {
      // 捕获下降沿时间T2，并计算高电平时间
      Hcsro4Info.t2 = HAL_TIM_ReadCapturedValue(htim, Hcsro4Info.ic_tim_ch);
      Hcsro4Info.t2 += Hcsro4Info.tim_overflow_counter * Hcsro4Info.period; //  需要考虑定时器溢出中断
      Hcsro4Info.high_level_us = Hcsro4Info.t2 - Hcsro4Info.t1; //  高电平持续时间 = 下降沿时间点 - 上升沿时间点
      // 计算距离
      Hcsro4Info.distance = (Hcsro4Info.high_level_us / 1000000.0) * 340.0 / 2.0 * 100.0;
      // 重新开启上升沿捕获
      Hcsro4Info.edge_state = 0;  //  一次采集完毕，清零
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, Hcsro4Info.ic_tim_ch, TIM_INPUTCHANNELPOLARITY_RISING);
    }
  }
}

/**
 * @description: 读取距离 
 * @param {*}
 * @return {*}
 */
float Hcsro4Read(void)
{
  // 测距结果限幅
  if(Hcsro4Info.distance >= 450)
  {
    Hcsro4Info.distance = 450;
  }
  return Hcsro4Info.distance;
}

