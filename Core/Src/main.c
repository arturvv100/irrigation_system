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

#include "main.h"

uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


#define SENSOR_AIRVALUE 		3100		//replace the value from calibration in air
#define SENSOR_WATERVALUE 		1200		//replace the value from calibration in water


ADC_HandleTypeDef hadc1;
GPIO_InitTypeDef relay;
GPIO_InitTypeDef sensor1;
uint32_t adc_value;
uint8_t soilmoisturepercent = 0;


static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);



int main(void)
{

  HAL_Init();

  MX_GPIO_Init();
  MX_ADC1_Init();

  while (1)
  {

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 200);
	adc_value = HAL_ADC_GetValue(&hadc1);
	soilmoisturepercent = (uint8_t)(map(adc_value, SENSOR_AIRVALUE, SENSOR_WATERVALUE, 0, 100));
	HAL_ADC_Stop(&hadc1);

	printf("Soil moisture = %ld (%ld%)\n", adc_value, soilmoisturepercent);

	if(soilmoisturepercent < 40)  // change this at what level the pump turns on
	{
	  printf("Zbyt sucho, Zalaczanie pompy");

	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);  // Low percent high signal to relay to turn on pump
	  HAL_Delay(3000);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	}
	else if(soilmoisturepercent >85) // max water level should be
	{
	  printf("Zbyt mokro, Wylaczanie pompy");
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);  // high percent water high signal to relay to turn on pump
	  HAL_Delay(6000);
	}

	HAL_Delay(1000);

  }
}



static void MX_ADC1_Init(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	ADC_ChannelConfTypeDef sConfig = {0};

	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

	HAL_ADC_Init(&hadc1);


	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}



static void MX_GPIO_Init(void)
{

	__HAL_RCC_GPIOA_CLK_ENABLE();

	relay.Pin = GPIO_PIN_9;
	relay.Mode = GPIO_MODE_OUTPUT_PP;
	relay.Pull = GPIO_NOPULL;
	relay.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &relay);

	sensor1.Pin = GPIO_PIN_0;
	sensor1.Mode = GPIO_MODE_ANALOG;
	sensor1.Pull = GPIO_NOPULL;
	sensor1.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &sensor1);
}

