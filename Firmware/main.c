/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dmx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUM_FADERS 7U
#define DMX_CHANNEL_COUNT 512U
#define DMX_FRAME_SIZE    513U

//FOR Button Inputs
#define UI_BUTTON_COUNT             12U

#define UI_FIRST_BUTTON_CHANNEL      8U

#define UI_BUTTON_RELEASED_VALUE     0U
#define UI_BUTTON_PRESSED_VALUE      255U

#define UI_SCAN_PERIOD_MS            10U
#define UI_DEBOUNCE_COUNT            3U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart1;

/* Definitions for DMX_TX_Task */
osThreadId_t DMX_TX_TaskHandle;
const osThreadAttr_t DMX_TX_Task_attributes = {
  .name = "DMX_TX_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for ADC_Task */
osThreadId_t ADC_TaskHandle;
const osThreadAttr_t ADC_Task_attributes = {
  .name = "ADC_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for Control_Task */
osThreadId_t Control_TaskHandle;
const osThreadAttr_t Control_Task_attributes = {
  .name = "Control_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal2,
};
/* Definitions for UI_Task */
osThreadId_t UI_TaskHandle;
const osThreadAttr_t UI_Task_attributes = {
  .name = "UI_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for Debug_Task */
osThreadId_t Debug_TaskHandle;
const osThreadAttr_t Debug_Task_attributes = {
  .name = "Debug_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow2,
};
/* USER CODE BEGIN PV */

volatile uint16_t adcRawValues[NUM_FADERS] = {0U};
uint8_t dmxFrame[DMX_FRAME_SIZE] = {0U};

static const uint16_t uiButtonPins[UI_BUTTON_COUNT] =
{
    GPIO_PIN_0,     // Button 1
    GPIO_PIN_1,     // Button 2
    GPIO_PIN_2,     // Button 3
    GPIO_PIN_3,     // Button 4
    GPIO_PIN_4,     // Button 5
    GPIO_PIN_5,     // Button 6
    GPIO_PIN_6,     // Button 7
    GPIO_PIN_7,     // Button 8
    GPIO_PIN_8,     // Button 9
    GPIO_PIN_9,     // Button 10
    GPIO_PIN_10,    // Button 11
    GPIO_PIN_12     // Button 12
};

MX_GPIO_Init();
MX_USART1_UART_Init();
MX_ADC1_Init();

DMX_Init();

osKernelInitialize();

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
void StartDMX_TX_Task(void *argument);
void StartADC_Task(void *argument);
void StartControl_Task(void *argument);
void StartUI_Task(void *argument);
void StartDebug_Task(void *argument);

/* USER CODE BEGIN PFP */
static uint8_t ADC_ConvertToDMX(uint32_t adcValue);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static uint8_t ADC_ConvertToDMX(uint32_t adcValue)

{
    if (adcValue > 4095U)
    {
        adcValue = 4095U;
    }

    return (uint8_t)((adcValue * 255U) / 4095U);
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
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  dmxFrame[0] = 0U;

  for (uint32_t index = 1U; index < DMX_FRAME_SIZE; index++)
  {
      dmxFrame[index] = 0U;
  }
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of DMX_TX_Task */
  DMX_TX_TaskHandle = osThreadNew(StartDMX_TX_Task, NULL, &DMX_TX_Task_attributes);

  /* creation of ADC_Task */
  ADC_TaskHandle = osThreadNew(StartADC_Task, NULL, &ADC_Task_attributes);

  /* creation of Control_Task */
  Control_TaskHandle = osThreadNew(StartControl_Task, NULL, &Control_Task_attributes);

  /* creation of UI_Task */
  UI_TaskHandle = osThreadNew(StartUI_Task, NULL, &UI_Task_attributes);

  /* creation of Debug_Task */
  Debug_TaskHandle = osThreadNew(StartDebug_Task, NULL, &Debug_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
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
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_0;
  sConfigInjected.InjectedRank = 1;
  sConfigInjected.InjectedNbrOfConversion = 1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_NONE;
  sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset = 0;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  huart1.Init.BaudRate = 250000;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_2;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX;
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : Button_1_Pin Button_2_Pin Button_3_Pin Button_11_Pin
                           Button_12_Pin Button_4_Pin Button_5_Pin Button_6_Pin
                           Button_7_Pin Button_8_Pin Button_9_Pin Button_10_Pin */
  GPIO_InitStruct.Pin = Button_1_Pin|Button_2_Pin|Button_3_Pin|Button_11_Pin
                          |Button_12_Pin|Button_4_Pin|Button_5_Pin|Button_6_Pin
                          |Button_7_Pin|Button_8_Pin|Button_9_Pin|Button_10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDMX_TX_Task */
/**
  * @brief  Function implementing the DMX_TX_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDMX_TX_Task */
void StartDMX_TX_Task(void *argument)
{
  /* USER CODE BEGIN 5 */

	 (void)argument;

	  DMX_Init();

	  for (;;)
	  {
	      DMX_SendFrame();
	      osDelay(25U);
	  }

  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartADC_Task */
/**
* @brief Function implementing the ADC_Task thread.
* @param argument: Not used
* @retval None
*/

extern ADC_HandleTypeDef hadc1;


//static uint8_t ADC_ConvertToDMX(uint32_t adcValue)
//{
//    if (adcValue > 4095U)
//    {
//        adcValue = 4095U;
//    }

//    return (uint8_t)((adcValue * 255U) / 4095U);
//}


/* USER CODE END Header_StartADC_Task */
void StartADC_Task(void *argument)
{
  /* USER CODE BEGIN StartADC_Task */
  /* Infinite loop */
	HAL_StatusTypeDef adcStatus;

	  (void)argument;
  for(;;)
  {
          adcStatus = HAL_ADC_Start(&hadc1); //start ADC

          if (adcStatus == HAL_OK)
          {
              adcStatus = HAL_ADC_PollForConversion(&hadc1, 10U); //start test

              if (adcStatus == HAL_OK)
              {
            	  adcRawValues[0] = (uint16_t)HAL_ADC_GetValue(&hadc1);

            	 // dmxFrame[1] = adcDMXValue; //connects to fader
              }

              HAL_ADC_Stop(&hadc1);
          }

          osDelay(10U);
      }
  /* USER CODE END StartADC_Task */
}

/* USER CODE BEGIN Header_StartControl_Task */
/**
* @brief Function implementing the Control_Task thread.
* @param argument: Not used
* @retval None
*/


/* USER CODE END Header_StartControl_Task */
void StartControl_Task(void *argument)
{
  /* USER CODE BEGIN StartControl_Task */
	uint32_t faderIndex; //loop counter
	    uint16_t rawADC;
	    uint8_t dmxValue;

	    (void)argument;
  /* Infinite loop */
  for(;;)
  {
	  for (faderIndex = 0U; faderIndex < NUM_FADERS; faderIndex++) //7 faders
	          {
	              rawADC = adcRawValues[faderIndex];

	              dmxValue = ADC_ConvertToDMX(rawADC); //ADC to DMX

	              dmxFrame[faderIndex + 1U] = dmxValue;
	          }

	                  osDelay(10U);
	              }

  /* USER CODE END StartControl_Task */
}

/* USER CODE BEGIN Header_StartUI_Task */
/**
* @brief Function implementing the UI_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUI_Task */
void StartUI_Task(void *argument)
{
  /* USER CODE BEGIN StartUI_Task */
  /* Infinite loop */
	GPIO_PinState rawButtonState[UI_BUTTON_COUNT];
	    GPIO_PinState stableButtonState[UI_BUTTON_COUNT];

	    uint8_t debounceCount[UI_BUTTON_COUNT];
	    uint8_t buttonIndex;
	    uint16_t dmxChannel;
  for(buttonIndex = 0U; buttonIndex < UI_BUTTON_COUNT; buttonIndex++)
  {
	  rawButtonState[buttonIndex] =
	              HAL_GPIO_ReadPin(GPIOB, uiButtonPins[buttonIndex]);

	          stableButtonState[buttonIndex] =
	              rawButtonState[buttonIndex];

	          debounceCount[buttonIndex] = 0U;

	          /*
	           * Buttons use pull-up resistors:
	           *
	           * GPIO_PIN_RESET = button pressed
	           * GPIO_PIN_SET   = button released
	           */
	          dmxChannel = UI_FIRST_BUTTON_CHANNEL + buttonIndex;

	          if (stableButtonState[buttonIndex] == GPIO_PIN_RESET)
	          {
	              dmxFrame[dmxChannel] = UI_BUTTON_PRESSED_VALUE;
	          }
	          else
	          {
	              dmxFrame[dmxChannel] = UI_BUTTON_RELEASED_VALUE;
	          }
	      }

	      /*
	       * Main UI task loop.
	       *
	       * The task wakes every 10 ms, scans all 12 buttons, debounces them,
	       * and updates DMX channels 8 through 19.
	       */
	      for (;;)
	      {
	          for (buttonIndex = 0U;
	               buttonIndex < UI_BUTTON_COUNT;
	               buttonIndex++)
	          {
	              GPIO_PinState newButtonState;

	              newButtonState =
	                  HAL_GPIO_ReadPin(GPIOB, uiButtonPins[buttonIndex]);

	              /*
	               * Check whether the raw GPIO state differs from the current
	               * debounced state.
	               */
	              if (newButtonState != stableButtonState[buttonIndex])
	              {
	                  /*
	                   * The state must remain changed for multiple scans before
	                   * it is accepted. This rejects mechanical contact bounce.
	                   */
	                  debounceCount[buttonIndex]++;

	                  if (debounceCount[buttonIndex] >= UI_DEBOUNCE_COUNT)
	                  {
	                      stableButtonState[buttonIndex] =
	                          newButtonState;

	                      debounceCount[buttonIndex] = 0U;

	                      /*
	                       * Button 0 controls channel 8.
	                       * Button 1 controls channel 9.
	                       * ...
	                       * Button 11 controls channel 19.
	                       */
	                      dmxChannel =
	                          UI_FIRST_BUTTON_CHANNEL + buttonIndex;

	                      /*
	                       * Active-low button logic:
	                       *
	                       * Pressed  -> GPIO reads RESET -> send 255
	                       * Released -> GPIO reads SET   -> send 0
	                       */
	                      if (stableButtonState[buttonIndex] ==
	                          GPIO_PIN_RESET)
	                      {
	                          dmxFrame[dmxChannel] =
	                              UI_BUTTON_PRESSED_VALUE;
	                      }
	                      else
	                      {
	                          dmxFrame[dmxChannel] =
	                              UI_BUTTON_RELEASED_VALUE;
	                      }
	                  }
	              }
	              else
	              {
	                  /*
	                   * The raw reading returned to the existing stable state,
	                   * so any incomplete debounce sequence is cancelled.
	                   */
	                  debounceCount[buttonIndex] = 0U;
	              }
	          }

	                  /*
	                   * Allow lower-priority tasks to run and establish the button
	                   * scanning period.
	                   */
	                  osDelay(UI_SCAN_PERIOD_MS);
	              }
	          }
  /* USER CODE END StartUI_Task */


/* USER CODE BEGIN Header_StartDebug_Task */
/**
* @brief Function implementing the Debug_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebug_Task */
void StartDebug_Task(void *argument)
{
  /* USER CODE BEGIN StartDebug_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDebug_Task */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM5 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM5)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
