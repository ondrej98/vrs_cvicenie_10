/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
uint8_t bufferUSART2dma[DMA_USART2_BUFFER_SIZE];
static void (*USART2_ProcessData)(uint8_t data) = 0;
/* USER CODE END 0 */

/* USART2 init function */

void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	LL_USART_InitTypeDef USART_InitStruct = { 0 };

	LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	/**USART2 GPIO Configuration
	 PA2   ------> USART2_TX
	 PA15   ------> USART2_RX
	 */
	GPIO_InitStruct.Pin = VCP_TX_Pin | VCP_RX_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* USART2 DMA Init */

	/* USART2_RX Init */
	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_6,
	LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_6,
	LL_DMA_PRIORITY_MEDIUM);

	LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_6, LL_DMA_MODE_NORMAL);

	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_6, LL_DMA_PERIPH_NOINCREMENT);

	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_6, LL_DMA_MEMORY_INCREMENT);

	LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_6, LL_DMA_PDATAALIGN_BYTE);

	LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_6, LL_DMA_MDATAALIGN_BYTE);

	/* USART2 interrupt Init */
	NVIC_SetPriority(USART2_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(USART2_IRQn);

	/* USER CODE BEGIN USART2_Init 1 */
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_6,
			LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_RECEIVE),
			(uint32_t) bufferUSART2dma,
			LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_6));
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_6, DMA_USART2_BUFFER_SIZE);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_6);
	//Interrupts
	LL_USART_EnableDMAReq_RX(USART2);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_6);
	LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_6);
	/* USER CODE END USART2_Init 1 */
	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART2, &USART_InitStruct);
	LL_USART_DisableIT_CTS(USART2);
	LL_USART_ConfigAsyncMode(USART2);
	LL_USART_Enable(USART2);
	/* USER CODE BEGIN USART2_Init 2 */
	LL_USART_EnableIT_IDLE(USART2);
	LL_USART_Enable(USART2);
	/* USER CODE END USART2_Init 2 */

}

/* USER CODE BEGIN 1 */
void USART2_RegisterCallback(void *callback) {
	if (callback != 0) {
		USART2_ProcessData = callback;
	}
}

void USART2_CheckDmaReception(void) {
	if (USART2_ProcessData == 0)
		return;
	static uint16_t old_pos = 0;

	uint16_t pos = DMA_USART2_BUFFER_SIZE
			- LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6);
	if (pos != old_pos) {
		if (pos > old_pos) {
			for (uint8_t i = old_pos; i < pos; i++) {
				uint8_t sign = bufferUSART2dma[i];
				USART2_ProcessData(sign);
			}
			if (LL_DMA_IsActiveFlag_HT6(DMA1) == SET
					|| LL_DMA_IsActiveFlag_TC6(DMA1) == SET) {
				LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_6);
				LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_6,
				DMA_USART2_BUFFER_SIZE);
				LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_6);
				pos = DMA_USART2_BUFFER_SIZE
						- LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6);
			}

		}
	}
	old_pos = pos;

	BufferCapacity.reserved = old_pos;
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
