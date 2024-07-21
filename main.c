/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint32_t time0;
uint32_t time1;
uint32_t time2;
uint32_t time3;

uint32_t min_switch_detected = UINT32_MAX;
uint32_t max_switch_detected = UINT32_MAX;

uint32_t DEBOUNCE_DELAY = 20;

uint8_t state = 0;

uint32_t MAX_ACTUATOR_TRAVEL_TIME = 10000;
uint32_t MAX_ACTUATOR_TRAVEL_TIME2 = 20000;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void move_actuator_extend(void);
void move_actuator_shrink(void);
void stop_actuator(void);
void homing_function(void);
void handle_fault(uint8_t actuatorFault);
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  homing_function(); // Calling homing function
	  HAL_Delay(10);
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ACTUATOR_EXTEND_Pin|ACTUATOR_SHRINK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : END_SWITCH_MIN_Pin END_SWITCH_MAX_Pin */
  GPIO_InitStruct.Pin = END_SWITCH_MIN_Pin|END_SWITCH_MAX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ACTUATOR_EXTEND_Pin ACTUATOR_SHRINK_Pin */
  GPIO_InitStruct.Pin = ACTUATOR_EXTEND_Pin|ACTUATOR_SHRINK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void homing_function(void) { // predefined motions that are required in order to configure the system's absolute position after power up
    switch(state) {
        case 0: // Initial state, start shrinking
            move_actuator_shrink();
            time0 = HAL_GetTick();
            state = 1;
            break;
        case 1: // Stop at the min switch, record time1, start extending
            if (HAL_GetTick() - time0 > MAX_ACTUATOR_TRAVEL_TIME) { // Fault detection
                 handle_fault(1);
            } else if(HAL_GPIO_ReadPin(END_SWITCH_MIN_GPIO_Port, END_SWITCH_MIN_Pin) == GPIO_PIN_SET) {
            	if(HAL_GetTick() - min_switch_detected > DEBOUNCE_DELAY){ // Wait for debounce period
            		stop_actuator();
            		state = 2;
            		time1 = HAL_GetTick(); // Record the time
            		move_actuator_extend();
            		min_switch_detected = UINT32_MAX;
            	}
            } else {
            	min_switch_detected = UINT32_MAX;
            }
            break;
        case 2: // Stop at the max switch, record time2, start shrinking
            if (HAL_GetTick() - time1 > MAX_ACTUATOR_TRAVEL_TIME2) { // Fault detection
                 handle_fault(2);
        	} else if(HAL_GPIO_ReadPin(END_SWITCH_MAX_GPIO_Port, END_SWITCH_MAX_Pin) == GPIO_PIN_SET) {
            	if(HAL_GetTick() - max_switch_detected > DEBOUNCE_DELAY){ // Wait for debounce period
            		stop_actuator();
            		state = 3;
            		time2 = HAL_GetTick(); // Record the time
            		move_actuator_shrink();
            		max_switch_detected = UINT32_MAX;
            	}
            } else {
            	max_switch_detected = UINT32_MAX;
            }
            break;
        case 3: // Stop at the min switch, record time3, start extending
            if (HAL_GetTick() - time2 > MAX_ACTUATOR_TRAVEL_TIME) { // Fault detection
                 handle_fault(1);
            } else if(HAL_GPIO_ReadPin(END_SWITCH_MIN_GPIO_Port, END_SWITCH_MIN_Pin) == GPIO_PIN_SET) {
            	if(HAL_GetTick() - min_switch_detected > DEBOUNCE_DELAY){ // Wait for debounce period
            		stop_actuator();
            		state = 4;
            		time3 = HAL_GetTick(); // Record the time
            		move_actuator_extend();
            		min_switch_detected = UINT32_MAX;
            	}
            } else {
            	min_switch_detected = UINT32_MAX;
            }
            break;
        case 4: // Stop when the actuator reaches middle position
            if(HAL_GetTick() - time3 >= (time2 -time1)/2) {
            	stop_actuator();
            }
            break;
    }
}


void move_actuator_extend(void) {
   HAL_GPIO_WritePin(ACTUATOR_SHRINK_GPIO_Port, ACTUATOR_SHRINK_Pin, GPIO_PIN_RESET);
   HAL_Delay(10); // Driver protection delay
   HAL_GPIO_WritePin(ACTUATOR_EXTEND_GPIO_Port, ACTUATOR_EXTEND_Pin, GPIO_PIN_SET);
}

void move_actuator_shrink(void) {
   HAL_GPIO_WritePin(ACTUATOR_EXTEND_GPIO_Port, ACTUATOR_EXTEND_Pin, GPIO_PIN_RESET);
   HAL_Delay(10); // Driver protection delay
   HAL_GPIO_WritePin(ACTUATOR_SHRINK_GPIO_Port, ACTUATOR_SHRINK_Pin, GPIO_PIN_SET);
}

void stop_actuator(void) {
   HAL_GPIO_WritePin(ACTUATOR_EXTEND_GPIO_Port, ACTUATOR_EXTEND_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(ACTUATOR_SHRINK_GPIO_Port, ACTUATOR_SHRINK_Pin, GPIO_PIN_RESET);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) { // Record the time when the switch is first triggered
    UNUSED(GPIO_Pin);
    if (GPIO_Pin == END_SWITCH_MIN_Pin) min_switch_detected = HAL_GetTick();
    if (GPIO_Pin == END_SWITCH_MAX_Pin) max_switch_detected = HAL_GetTick();
}

void handle_fault(uint8_t actuatorFault) {
    stop_actuator();
    switch (actuatorFault) {
        case 1:
            printf("Fault: Actuator not reaching the min switch.\n");
            break;
        case 2:
            printf("Fault: Actuator not reaching the max switch/\n");
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
