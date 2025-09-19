/* USER CODE BEGIN Header */
/* ------------------ IMPORTANT NOTE ------------------
      IMPORTANT NOTE: IF CODE IS REGENERATED WITH CUBEMX,
      YOU MUST COMMENT OUT THE ENABLE IRQ LINE IN MX_GPIO_INIT
      THE INTERRUPT MUST ONLY BE ENABLED ONCE THE POWER SETTLES, EVERYTHIGN IS POWERED ON AND READY TO GO
      I INCLUDED THE SEQUENCE IN THE enabelMotor() FUNCTION.
  -------------------------------------------------------*/

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
CAN_HandleTypeDef hcan1;

DAC_HandleTypeDef hdac;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// Timers, counters and interrupts
uint32_t one_ms_counter = 0;

// Communication global variables
uint8_t uart_received_character;
uint16_t uart_message_length_counter = 0;
uint8_t uart_reception_complete = false;
char uart_received_message[100];
uint8_t can_message_pending = true;
double can_rx = 0;
volatile bool sample_flag = false;

// Debug functions/variables
void displayMessage(void);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_CAN1_Init(void);
static void MX_DAC_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void CAN_Filter_Config(void);
void CAN_Start(void);

void enableMotor(void);
void __AttemptEnergizeMotor(void);
void __ConsentToEnergizeMotor(void);
void displayMessage(void);


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
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_CAN1_Init();
  MX_DAC_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  CAN_Filter_Config();
  CAN_Start();

  setvbuf(stdin, NULL, _IONBF, 0);// Disable buffering for stdin, required for scanf to work

  // 3 lines below disabled for debug

  HAL_Delay(2000);
  printf("Consenting to energize motor...");
  __ConsentToEnergizeMotor();
  HAL_Delay(1000);
  printf(" Done\n\rAttempting to energize motor...");
  __AttemptEnergizeMotor();
  printf("Done");
  HAL_Delay(2000);
  displayMainMenu();
  handleMainMenu(getCharacter());

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    displayMessage();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = ENABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 2799;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 60000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 42000;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, NOT_TRIP_Pin|ENERGIZE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RED_LED_Pin|SIGNAL_OUT_1_Pin|SIGNAL_OUT_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NOT_TRIP_Pin ENERGIZE_Pin */
  GPIO_InitStruct.Pin = NOT_TRIP_Pin|ENERGIZE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RELAY_OBSERVE_Pin */
  GPIO_InitStruct.Pin = RELAY_OBSERVE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RELAY_OBSERVE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RED_LED_Pin */
  GPIO_InitStruct.Pin = RED_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RED_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SIGNAL_OUT_1_Pin SIGNAL_OUT_2_Pin */
  GPIO_InitStruct.Pin = SIGNAL_OUT_1_Pin|SIGNAL_OUT_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void CAN_Start(void)
{
  if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING |CAN_IT_BUSOFF) != HAL_OK)
	{
		Error_Handler();
	}


	if( HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
}

void CAN_Filter_Config(void)
{
	// Initialize CAN filter setup structure
	CAN_FilterTypeDef CAN1_Filter;

	// Configure CAN reception filter
	CAN1_Filter.FilterActivation = ENABLE;
	CAN1_Filter.FilterBank  = 0;
	CAN1_Filter.FilterFIFOAssignment = CAN_RX_FIFO0;
	CAN1_Filter.FilterIdHigh = 0x0000;
	CAN1_Filter.FilterIdLow = 0x0000;
	CAN1_Filter.FilterMaskIdHigh = 0x0000;
	CAN1_Filter.FilterMaskIdLow = 0x0000;
	CAN1_Filter.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN1_Filter.FilterScale = CAN_FILTERSCALE_32BIT;

	// Set CAN filter configuration and verify settings
	if( HAL_CAN_ConfigFilter(&hcan1,&CAN1_Filter) != HAL_OK)
	{
		Error_Handler();
	}
}

void enableMotor(void){
  //IMPORTANT: ALWAYS DISABLE INTERRUPT BY COMMENTING OUT THE LINES IN THE MX_GPIO_Init FUNCTION
  // CUBE MX automatically enables the interrupt for the pin, so you have to disable it manually
  // if you regenerate the code, make sure they are enabled below and only at this point in the code

  __ConsentToEnergizeMotor();
  __AttemptEnergizeMotor();
}

int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

int fputc(int ch, FILE *f) {
    (void)f; // Cast to void to avoid unused parameter warning
    return __io_putchar(ch);
}

int __io_getchar(void) {
    uint8_t ch = 0;

    /* Clear the Overrun flag just before receiving the first character */
  __HAL_UART_CLEAR_OREFLAG(&huart2);

    HAL_UART_Receive(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

int fgetc(FILE *f) {
    (void)f; // Cast to void to avoid unused parameter warning
    return __io_getchar();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /*
  if (GPIO_Pin == RELAY_INTERRUPT_Pin)
  {
    // Debounce the interrupt using a moving average filter
    static uint32_t last_interrupt_time = 0;
    static uint32_t debounce_buffer[10] = {0};
    static uint8_t buffer_index = 0;
    uint32_t current_time = HAL_GetTick();

    // Update debounce buffer
    debounce_buffer[buffer_index] = current_time - last_interrupt_time;
    buffer_index = (buffer_index + 1) % 10;

    // Calculate moving average
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 10; i++) {
      sum += debounce_buffer[i];
    }
    uint32_t average_time = sum / 10;

    if (average_time > 200) // 200 ms debounce time
    {
      // Handle the interrupt
      // Disable relay pin
      HAL_GPIO_WritePin(RELAY_SIGNAL_GPIO_Port, RELAY_SIGNAL_Pin, GPIO_PIN_RESET);
      // Turn on red LED
      HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);

      // Re-enable interrupts after a short delay
      HAL_Delay(200); // Adjust the delay as needed
      HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    }
    last_interrupt_time = current_time;
  }
  *//*
 if (GPIO_Pin == RELAY_INTERRUPT_Pin)
  {
    // Debounce the interrupt
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = HAL_GetTick();
    if (current_time - last_interrupt_time > 200) // 200 ms debounce time
    {
      // Disable interrupts
      HAL_NVIC_DisableIRQ(EXTI1_IRQn);
      // Disable relay pin
      HAL_GPIO_WritePin(RELAY_SIGNAL_GPIO_Port, RELAY_SIGNAL_Pin, GPIO_PIN_RESET);
      // Turn on red LED
      HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
      // Lock out forever
      while(1);
    }
    last_interrupt_time = current_time;
  }*/
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
	//printf("CAN Callback");
	(void)hcan; // Cast to void to avoid unused parameter warning
//most_recent_value_time = HAL_GetTick();
	// Create variable to receive the message
	uint8_t rx_data[7];

	// Create Reception header to configure message reception
	CAN_RxHeaderTypeDef rxHeader;


	// Receive message
	if(HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&rxHeader,rx_data) != HAL_OK){
		Error_Handler();
	}

	//printf(":%d\n\r",rxHeader.StdId);

	if (rxHeader.StdId == TMCM1636_CAN_ID){
		//printf(" (TMCM1636)");
		TMCM1636_CAN_Receive(rx_data);
	}
	if (rxHeader.StdId == ARDUINO_CAN_ID){
		//TODO: turn listening to the strain gauge on/off
		Arduino_CAN_Receive(rx_data);
	}

	else {
	        CAN_RxHeaderTypeDef dummyHdr;
	        uint8_t dummyBuf[7];
	        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &dummyHdr, dummyBuf);
	    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  (void)huart; // Cast to void to avoid unused parameter warning
	// Run when message reception complete through UART
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		HAL_TIM_Base_Stop_IT(&htim6);
	}

	else if(htim->Instance == TIM7)
	{
		one_ms_counter += 1;
		sample_flag = true;
	}
}

void displayMessage(void){
//Wait for reply and store values
while(can_message_pending == true); //Wait for reply and store values
printf("%.0f,", can_rx);
}


void __AttemptEnergizeMotor(void){
	// Private function
	// brings the energize pin high briefly
	HAL_GPIO_WritePin(ENERGIZE_GPIO_Port, ENERGIZE_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ENERGIZE_GPIO_Port, ENERGIZE_Pin, GPIO_PIN_RESET);
}

void __ConsentToEnergizeMotor(void){
	HAL_GPIO_WritePin(NOT_TRIP_GPIO_Port, NOT_TRIP_Pin, GPIO_PIN_SET);  // bring NOT_TRIP high, ie do not trip the safety circuit
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
