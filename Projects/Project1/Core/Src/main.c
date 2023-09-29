/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#define LED7_DURATION 25
#define DOT_DURATION 100

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
int led_test_counter = 100;
int led_test_flag = 0;
int dot_counter = DOT_DURATION;
int dot_flag = 0;
int scanning_counter = LED7_DURATION;
int scanning_flag = 0;
int led_enabled = 0;
int hour = 15, minute = 8, second = 50;

const int MAX_LED = 4;
int index_led = 0;
int led_buffer[4] = {6, 2, 8, 9};

const int MAX_LED_MATRIX = 8;
int index_led_matrix = 0;
uint8_t matrix_buffer[8] = {0x00, 0xfc, 0x12, 0x11, 0x1, 0x12, 0xfc, 0x00};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void display7SEG(int);
void update7SEG(int);
void updateClockBuffer();
void updateLEDMatrix(int index);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int timer0_counter = 0;
int timer0_flag = 0;
int TIMER_CYCLE = 10;

void setTimer0(int duration)
{
	timer0_counter = duration / TIMER_CYCLE;
	timer0_flag = 0;
}

void timer_run()
{
	if (timer0_counter > 0)
	{
		timer0_counter--;
		if (timer0_counter == 0) timer0_flag = 1;
	}
}

int timer_matrix_counter = 0;
int timer_matrix_flag = 0;
void setTimerMatrix(int duration)
{
	timer_matrix_counter = duration / TIMER_CYCLE;
	timer_matrix_flag = 0;
}

void timerMatrixRun()
{
	if (timer_matrix_counter > 0)
	{
		--timer_matrix_counter;
		if (timer_matrix_counter == 0) timer_matrix_flag = 1;
	}
}
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  setTimer0(1000);
  setTimerMatrix(100);
  updateClockBuffer();
  update7SEG(index_led);
  while (1)
  {
	if (timer_matrix_flag == 1)
	{
		updateLEDMatrix(index_led_matrix);
		setTimerMatrix(100);
		++index_led_matrix;
		if (index_led_matrix >= MAX_LED_MATRIX)
		{
			index_led_matrix = 0;
		}
	}

	if (timer0_flag == 1)
	{
		++second;
		if (second >= 60)
		{
			second = 0;
			++minute;
		}
		if (minute >= 60)
		{
			minute = 0;
			++hour;
		}
		if (hour >= 24)
		{
			hour = 0;
		}
		updateClockBuffer();
		setTimer0(1000);
	}

	if (led_test_flag == 1)
	{
		led_test_flag = 0;
		led_test_counter = 100;
		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	}

	if (dot_flag == 1)
	{
		dot_flag = 0;
		dot_counter = DOT_DURATION;
		HAL_GPIO_TogglePin(DOT_GPIO_Port, DOT_Pin);
	}

	if (scanning_flag == 1)
	{
		++index_led;
		if (index_led >= MAX_LED)
		{
			index_led = 0;
		}
		update7SEG(index_led);
		scanning_flag = 0;
		scanning_counter = LED7_DURATION;
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, ENM0_Pin|ENM1_Pin|DOT_Pin|LED_RED_Pin
                          |EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin
                          |ENM2_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED7_0_Pin|LED7_1_Pin|LED7_2_Pin|ROW2_Pin
                          |ROW3_Pin|ROW4_Pin|ROW5_Pin|ROW6_Pin
                          |ROW7_Pin|LED7_3_Pin|LED7_4_Pin|LED7_5_Pin
                          |LED7_6_Pin|ROW0_Pin|ROW1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ENM0_Pin ENM1_Pin DOT_Pin LED_RED_Pin
                           EN0_Pin EN1_Pin EN2_Pin EN3_Pin
                           ENM2_Pin ENM3_Pin ENM4_Pin ENM5_Pin
                           ENM6_Pin ENM7_Pin */
  GPIO_InitStruct.Pin = ENM0_Pin|ENM1_Pin|DOT_Pin|LED_RED_Pin
                          |EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin
                          |ENM2_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED7_0_Pin LED7_1_Pin LED7_2_Pin ROW2_Pin
                           ROW3_Pin ROW4_Pin ROW5_Pin ROW6_Pin
                           ROW7_Pin LED7_3_Pin LED7_4_Pin LED7_5_Pin
                           LED7_6_Pin ROW0_Pin ROW1_Pin */
  GPIO_InitStruct.Pin = LED7_0_Pin|LED7_1_Pin|LED7_2_Pin|ROW2_Pin
                          |ROW3_Pin|ROW4_Pin|ROW5_Pin|ROW6_Pin
                          |ROW7_Pin|LED7_3_Pin|LED7_4_Pin|LED7_5_Pin
                          |LED7_6_Pin|ROW0_Pin|ROW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void updateClockBuffer()
{
	led_buffer[0] = hour / 10;
	led_buffer[1] = hour % 10;
	led_buffer[2] = minute / 10;
	led_buffer[3] = minute % 10;
}
void HAL_TIM_PeriodElapsedCallback ( TIM_HandleTypeDef * htim )
{
	timerMatrixRun();
	timer_run();

	--led_test_counter;
	if (led_test_counter <= 0)
	{
		led_test_flag = 1;
	}

	--dot_counter;
	if (dot_counter <= 0)
	{
		dot_flag = 1;
	}

	--scanning_counter;
	if (scanning_counter <= 0)
	{
		scanning_flag = 1;
	}
}

void display7SEG(int number)
{
	switch (number)
	{
	case 0:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_SET);
		break;
	case 1:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_SET);
		break;
	case 2:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_RESET);
		break;
	case 4:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_RESET);
		break;
	case 5:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_RESET);
		break;
	case 6:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_RESET);
		break;
	case 7:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_SET);
		break;
	case 8:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_RESET);
		break;
	case 9:
		HAL_GPIO_WritePin(LED7_0_GPIO_Port, LED7_0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_1_GPIO_Port, LED7_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_2_GPIO_Port, LED7_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_3_GPIO_Port, LED7_3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_4_GPIO_Port, LED7_4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_5_GPIO_Port, LED7_5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED7_6_GPIO_Port, LED7_6_Pin, GPIO_PIN_RESET);
		break;
	}
}

void update7SEG(int index)
{
	switch (index)
	{
	case 0:
		display7SEG(led_buffer[index]);
		HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_SET);
		break;
	case 1:
		display7SEG(led_buffer[index]);
		HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_SET);
		break;
	case 2:
		display7SEG(led_buffer[index]);
		HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_SET);
		break;
	case 3:
		display7SEG(led_buffer[index]);
		HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_RESET);
		break;
	default:
		break;
	}
}

void updateLEDMatrix(int index)
{
	HAL_GPIO_WritePin(ROW0_GPIO_Port, ROW0_Pin, ((matrix_buffer[index_led_matrix] >> 0) & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, ((matrix_buffer[index_led_matrix] >> 1) & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, ((matrix_buffer[index_led_matrix] >> 2) & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, ((matrix_buffer[index_led_matrix] >> 3) & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, ((matrix_buffer[index_led_matrix] >> 4) & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW5_GPIO_Port, ROW5_Pin, ((matrix_buffer[index_led_matrix] >> 5) & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW6_GPIO_Port, ROW6_Pin, ((matrix_buffer[index_led_matrix] >> 6) & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW7_GPIO_Port, ROW7_Pin, ((matrix_buffer[index_led_matrix] >> 7) & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);

	switch (index_led_matrix)
	{
	case 0:
		HAL_GPIO_WritePin(ENM0_GPIO_Port, ENM0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ENM1_GPIO_Port, ENM1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM2_GPIO_Port, ENM2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM3_GPIO_Port, ENM3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM4_GPIO_Port, ENM4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM5_GPIO_Port, ENM5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM6_GPIO_Port, ENM6_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM7_GPIO_Port, ENM7_Pin, GPIO_PIN_SET);
		break;
	case 1:
		HAL_GPIO_WritePin(ENM0_GPIO_Port, ENM0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM1_GPIO_Port, ENM1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ENM2_GPIO_Port, ENM2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM3_GPIO_Port, ENM3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM4_GPIO_Port, ENM4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM5_GPIO_Port, ENM5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM6_GPIO_Port, ENM6_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM7_GPIO_Port, ENM7_Pin, GPIO_PIN_SET);
		break;
	case 2:
		HAL_GPIO_WritePin(ENM0_GPIO_Port, ENM0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM1_GPIO_Port, ENM1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM2_GPIO_Port, ENM2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ENM3_GPIO_Port, ENM3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM4_GPIO_Port, ENM4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM5_GPIO_Port, ENM5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM6_GPIO_Port, ENM6_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM7_GPIO_Port, ENM7_Pin, GPIO_PIN_SET);
		break;
	case 3:
		HAL_GPIO_WritePin(ENM0_GPIO_Port, ENM0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM1_GPIO_Port, ENM1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM2_GPIO_Port, ENM2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM3_GPIO_Port, ENM3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ENM4_GPIO_Port, ENM4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM5_GPIO_Port, ENM5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM6_GPIO_Port, ENM6_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM7_GPIO_Port, ENM7_Pin, GPIO_PIN_SET);
		break;
	case 4:
		HAL_GPIO_WritePin(ENM0_GPIO_Port, ENM0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM1_GPIO_Port, ENM1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM2_GPIO_Port, ENM2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM3_GPIO_Port, ENM3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM4_GPIO_Port, ENM4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ENM5_GPIO_Port, ENM5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM6_GPIO_Port, ENM6_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM7_GPIO_Port, ENM7_Pin, GPIO_PIN_SET);
		break;
	case 5:
		HAL_GPIO_WritePin(ENM0_GPIO_Port, ENM0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM1_GPIO_Port, ENM1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM2_GPIO_Port, ENM2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM3_GPIO_Port, ENM3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM4_GPIO_Port, ENM4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM5_GPIO_Port, ENM5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ENM6_GPIO_Port, ENM6_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM7_GPIO_Port, ENM7_Pin, GPIO_PIN_SET);
		break;
	case 6:
		HAL_GPIO_WritePin(ENM0_GPIO_Port, ENM0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM1_GPIO_Port, ENM1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM2_GPIO_Port, ENM2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM3_GPIO_Port, ENM3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM4_GPIO_Port, ENM4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM5_GPIO_Port, ENM5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM6_GPIO_Port, ENM6_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ENM7_GPIO_Port, ENM7_Pin, GPIO_PIN_SET);
		break;
	case 7:
		HAL_GPIO_WritePin(ENM0_GPIO_Port, ENM0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM1_GPIO_Port, ENM1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM2_GPIO_Port, ENM2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM3_GPIO_Port, ENM3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM4_GPIO_Port, ENM4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM5_GPIO_Port, ENM5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM6_GPIO_Port, ENM6_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ENM7_GPIO_Port, ENM7_Pin, GPIO_PIN_RESET);
		break;
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
