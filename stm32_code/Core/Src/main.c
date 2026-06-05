/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "sys.h"

#include "dht11.h"
#include "key.h"
#include "led.h"
#include "oled.h"
#include "motor.h"
#include "hc_sro4.h"

#include "usart.h"
#include "aliyun_connect.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define do_key_cycle 20
#define do_oled_cycle 1000//
#define do_led_cycle 100
#define do_data_cycle 1000//

#define usart_later 50
#define usart_size 50

#define mqttheart_interval 30//60s   60次data_event后发心脏包

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//MODE
uint8_t mode=0;//0手动 1自动

//TIM
uint16_t t;//计时

//LED
uint8_t led_flag,led0=0;

//KEY
uint8_t key_flag,key_value,key_temp;

//OLED
uint8_t oled_flag;

//DATA
uint8_t data_flag;


//USART1     dubug串口
uint8_t Tx_buff[usart_size],Rx_buff[usart_size],rx_flag1=0,rx1_ok=0,rx_value,rx_amount;

uint8_t rx_flag2=0,rx2_ok=0;

//USART2     esp8266通信串口  在esp8266at.c里


//DHT11
uint16_t temperature,temp_parameter;
uint16_t humidity,humi_parameter;

//HC_SRO4
float hc_distance=450;//4.5m为最大可测量距离

//JOB
uint8_t job_id=0,dir=0,runspeed=0,isrun=0;//小车开跑才runtime--
uint16_t runtime=0;//ms级

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

//改BT控制行走和WIFI控制行走
void BT_CONTROL(void);//蓝牙主动控制
void PATH_FINDING(void);//算法自动控制

void RUN_EVENT(void);//按路线行事
void KEY_EVENT(void);//按键 事件处理逻辑
void OLED_EVENT(void);//OLED屏 事件处理逻辑
void LED_EVENT(void);//LED灯 事件处理逻辑
void DATA_EVENT(void);//关键数据 事件处理逻辑
void UART_EVENT(void);//关键UART处理逻辑

void Enter_ErrorMode(uint8_t mode);
void ES8266_MQTT_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_TIM_Base_Start_IT(&htim3);//在中断模式下启动 TIM 基生成。
	
	HAL_UART_Receive_IT(&huart1,&rx_value,1);
	
	HAL_UART_Receive_IT(&huart2,usart2_rxone,1);
	
	//network config
	ES8266_MQTT_Init();//net
	
	MOTOR_Init();//电机初始化
	Hcsro4Init(&htim2, TIM_CHANNEL_4);//超声波模块初始化
	
	externalLED_Config();//外接LED灯配置初始化
	internalLED_Config();//板载LED灯配置初始化
	
	KEY_Config();//KEY初始化
	
	while(DHT11_Init());//DHT11初始化

	OLED_Init();//OLED显示屏初始化
	OLED_ColorTurn(0);//0正常显示，1 反色显示
	OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示	
	
	
	Hcsro4Start();//开始测距
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		RUN_EVENT();
		DATA_EVENT();
		UART_EVENT();
		OLED_EVENT();
		KEY_EVENT();
		LED_EVENT();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : TRIG_Pin */
  GPIO_InitStruct.Pin = TRIG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TRIG_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void BT_CONTROL(void)
{
	if(runtime!=0)
	{
		switch(dir){
			case 1:{//直行  dir==1 speed time
				Set_Speed(runspeed);Motor_forward();
				
			}
				break;
			case 2:{//倒车  dir==2 speed time
				Set_Speed(runspeed);Motor_backward();
				
			}
				break;
			case 3:{//左转  dir==3 speed固定 time固定
				Set_Speed(runspeed);Motor_leftward();
				
			}
				break;
			case 4:{//右转  dir==4 speed固定 time固定
				Set_Speed(runspeed);Motor_rightward();
				
			}
				break;
			default:dir=0;break;
		}
	}
	else { isrun=0; runtime=0; Motor_stop(); runspeed=0; Set_Speed(runspeed); }
}

void PATH_FINDING(void)
{//job_id,runtime,dir
	
}

void RUN_EVENT(void)
{
	if(mode) PATH_FINDING();//自动控制 算法
	else BT_CONTROL();//主动控制
}


//USART1   USART2回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)//USART1
	{
		Rx_buff[rx_amount++]=rx_value;
		if(rx_amount >= usart_size)
			rx_amount=0;
		rx_flag1=usart_later;//flag1
		HAL_UART_Receive_IT(huart,&rx_value,1);
	}
	
	else if(huart->Instance == USART2)//USART2
	{
		//将接收到的数据放入接收usart2接收数组		
		usart2_rxbuf[usart2_rxcounter] = usart2_rxone[0];
		usart2_rxcounter++;	//接收数量＋1
		
		rx_flag2=usart_later;//flag2    later 过了一段时间后无数据进入判断接收完毕 开始处理接收的数据
		//重新使能串口2接收中断
		HAL_UART_Receive_IT(&huart2,usart2_rxone,1);
		
	}
}


//关键UART处理逻辑
void UART_EVENT(void)
{
	
	if(rx2_ok==1)//USART2--WiFi
	{
		deal_MQTT_message(usart2_rxbuf,usart2_rxcounter);//解析来自MQTT服务器的数据信息
		
		//已在deal_MQTT_message函数内处理完数据后清空缓存了
		rx2_ok=0;
	}
		
	
	if(rx1_ok==1)//USART1--Debug
	{
		UART_Printf(&huart1,"\nUSART1_Recv:");
		HAL_UART_Transmit(&huart1,Rx_buff,strlen((char*)Rx_buff),50);//把串口1收到的 在串口1打印出来
		
		
		switch(Rx_buff[0])
		{
			case '0':{
					mode=0;}break;//WiFi控制行驶   方向速度时间
			case 49:{
					mode=1;}break;//蓝牙控制行驶   方向速度
			case 50:{
					led0=0;}break;
			case 51:{
					led0=1;}break;
			case 52:{
					dir=1;}break;//前
			case 53:{
					dir=3;}break;//左转
			case 54:{
					if(runspeed<4) runspeed++;}break;
			case 55:{
					if(runspeed>0) runspeed--;}break;
			case 56:break;
			case '9':break;
			default:break;
		}
		
		
		memset(Rx_buff,'\0',sizeof(Rx_buff));//清空
		
		rx_amount=0;
		rx1_ok=0;
	}
	
}


//关键DATA处理逻辑
void DATA_EVENT(void)
{
	
	if(data_flag==1)
	{
		data_flag=0;
		
		
		
		//本机数据上报
		STM32_Report();//net
		
//		hc_distance=Hcsro4Read();//获取数据
		
		//每1s进这里  start到测完有充足的缓冲时间（模块说明书要求每次测量周期间隔时间至少60ms）1000>>60
		Hcsro4Start();//开始测距
		
	}
	
}


//KEY处理逻辑
void KEY_EVENT(void)
{
	if(key_flag == 1)
	{
		key_temp=key_scan();
		if(key_temp != 0) key_value=key_temp;
		key_flag=0;
	}

	switch(key_value)
	{
		case 1://
		{
			
			
			
			key_value=0;
		}break;
		
		case 2://
		{
			
			
			
			key_value=0;
		}break;
		
		
		default:key_value=0;break;
	}
	
}


//LED处理逻辑
void LED_EVENT(void)
{
	if(led_flag==1)
	{
		led_flag=0;
		
		
	}
}


//OLED处理逻辑
void OLED_EVENT(void)
{
	if(oled_flag==1)
	{
		oled_flag=0;
		
		OLED_Clear();//先清屏
		
		//写入缓存              每个字符占空8个单位，每行最多显示16个字符
		OLED_ShowString(0,0,(unsigned char *)"STMCAR_ID:",16,1);
		OLED_ShowNum(8*10,0,hw_id,4,16,1);
		
		OLED_ShowString(0,16,(unsigned char *)"T:",16,1);
		OLED_ShowNum(8*2,16,temperature,2,16,1);
		
		OLED_ShowString(8*8,16,(unsigned char *)"H:",16,1);
		OLED_ShowNum(8*10,16,humidity,2,16,1);
		
		OLED_ShowString(0,16*2,(unsigned char *)"Speed:",16,1);
		OLED_ShowNum(8*6,16*2,runspeed,1,16,1);
		
		OLED_ShowString(8*8,16*2,(unsigned char *)"RunT:",16,1);
		OLED_ShowNum(8*13,16*2,runtime,2,16,1);
		
		OLED_ShowString(0,16*3,(unsigned char *)"Mode:",16,1);
		OLED_ShowNum(8*5,16*3,mode,1,16,1);
		
		OLED_ShowString(8*8,16*3,(unsigned char *)"Dist:",16,1);
		OLED_ShowNum(8*13,16*3,hc_distance,2,16,1);
//		OLED_ShowString(8*8,16*3,(unsigned char *)"J_id:",16,1);
//		OLED_ShowNum(8*13,16*3,job_id,2,16,1);
		
		OLED_Refresh();//更新屏幕内容
	}
}

//输入捕获
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2)
	{
		Hcsro4TimIcIsr(htim);
	}
}

//基本定时器config
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)//TIM3
	{
		t++;
		
		if(t % do_key_cycle == 0)//KEY
			key_flag=1;
		
		if(t % do_led_cycle == 0)//LED
			led_flag=1;
		
		if(t % do_oled_cycle == 0)//OLED
			oled_flag=1;
			
		if(t % do_data_cycle == 0)//DATA
			data_flag=1;
		
		if( (runtime!=0) && (isrun==1) )
		{
			runtime--;//ms级
			Motor_pwm();//pwm
		}
		
		
		
		if(rx_flag1 !=0)//UART1
		{
			rx_flag1--;
			if(rx_flag1 == 0)
				rx1_ok=1;
		}
		if(rx_flag2 !=0)//UART2
		{
			rx_flag2--;
			if(rx_flag2 == 0)
				rx2_ok=1;
		}
		
		
		if(t >= 20000)  { t=0; }
	}
	
	else if(htim->Instance == TIM2)//TIM2
	{
		Hcsro4TimOverflowIsr(htim);
	}
	
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
