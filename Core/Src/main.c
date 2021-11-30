/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */
extern uint8_t PWM_Value;
extern uint8_t PWM_ValueReq;
extern Direction PWM_ValueDirection;
extern Mode mode;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	PWM_Value = 0;
	PWM_ValueReq = 0;
	PWM_ValueDirection = Direction_DownUp;
	mode = Mode_Auto;
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	/* System interrupt init*/

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	TIM2_RegisterCallback(setDutyCycle);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		LL_mDelay(5000);
		mode = Mode_Man;
		PWM_ValueReq = 80;
		LL_mDelay(10000);
		PWM_ValueReq = 50;
		LL_mDelay(10000);
		PWM_ValueReq = 20;
		LL_mDelay(10000);
		PWM_ValueReq = 50;
		LL_mDelay(10000);
		PWM_ValueReq = 80;
		LL_mDelay(10000);
		mode = Mode_Auto;
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0) {
	}
	LL_RCC_HSI_Enable();

	/* Wait till HSI is ready */
	while (LL_RCC_HSI_IsReady() != 1) {

	}
	LL_RCC_HSI_SetCalibTrimming(16);
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {

	}
	LL_Init1msTick(8000000);
	LL_SetSystemCoreClock(8000000);
}

/* USER CODE BEGIN 4 */
void setDutyCycle(uint8_t D) {
	if (mode == Mode_Auto) {
		D = CountDutyCycleForModeAuto(D);
		PWM_ValueReq = D;
	} else if (mode == Mode_Man)
		D = CountDutyCycleForModeMan(D, PWM_ValueReq);
	LL_TIM_OC_SetCompareCH1(TIM2, D);
	PWM_Value = D;
}

uint8_t CountDutyCycleForModeAuto(uint8_t D) {
	if (D < PWM_VALUE_MAX && PWM_ValueDirection == Direction_DownUp)
		D += 1;
	else if (D >= PWM_VALUE_MAX && PWM_ValueDirection == Direction_DownUp) {
		PWM_ValueDirection = Direction_UpDown;
		D -= 1;
	} else if (D > PWM_VALUE_MIN && PWM_ValueDirection == Direction_UpDown)
		D -= 1;
	else if (D <= PWM_VALUE_MIN && PWM_ValueDirection == Direction_UpDown) {
		PWM_ValueDirection = Direction_DownUp;
		D += 1;
	}
	return D;
}
uint8_t CountDutyCycleForModeMan(uint8_t D, uint8_t reqD) {
	if (D != reqD) {
		D = CountDutyCycleForModeAuto(D);
	}
	return D;
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
