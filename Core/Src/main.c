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
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */
//SSID e password per la connessione al router
const char *SSID = "YourSSID";
const char *password = "Yourpassword";

//buffer per inserire risposte del modulo esp
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint16_t rx_index = 0;
uint8_t tx_buffer[128];

//parametri per connessione al broker
const char *MQTT_BROKER = "broker.hivemq.com";
const char *MQTT_PORT = "1883";
const char *MQTT_CLIENT_ID = "STM32Client";
const char *MQTT_TOPIC = "YourTopic";
const char *MQTT_MESSAGE = "Hello from STM32";

int c=0;
int response_received=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
// Attivo il modulo wifi e lo resetto
WiFi_Init();
send_at_command("AT");
HAL_Delay(2000);
send_at_command("AT+RST");
HAL_Delay(2000);


 // Attivo la modalità interrut per la ricezione della risposta
HAL_UART_Receive_IT(&huart6, rx_buffer + rx_index, 1);
HAL_Delay(2000);

//mando at commmand per impostare la modalità WiFi
  	  do{
  	  send_at_command("AT+CWMODE=1");
  	  HAL_Delay(500);
  	  	  if (strstr((char*)rx_buffer, "OK\r\n") != NULL) {
  		  HAL_UART_Transmit(&huart2, (uint8_t *)"Imposto AP\r\n", strlen("Imposto AP\r\n"), HAL_MAX_DELAY);
  		  response_received = 1;} // Imposta il flag della risposta attesa
  	          }while(response_received != 1);

  	  do{
  		  send_at_command("AT+CIPMUX=0");
  		  HAL_Delay(500);
  	  	  if (strstr((char*)rx_buffer, "OK\r\n") != NULL) {
  		  HAL_UART_Transmit(&huart2, (uint8_t *)"Connessione singola\r\n", strlen("Connessione singola\r\n"), HAL_MAX_DELAY);
  		  response_received = 1;} // Imposta il flag della risposta attesa
  	          }while(response_received != 1);

 //Mi connetto al router
  	  do{
  		  HAL_UART_Transmit(&huart6, (uint8_t *)"AT+CWJAP=\"", strlen("AT+CWJAP=\""), HAL_MAX_DELAY);
  	      HAL_UART_Transmit(&huart6, (uint8_t *)SSID, strlen((char *)SSID), HAL_MAX_DELAY);
  	      HAL_UART_Transmit(&huart6, (uint8_t *)"\",\"", 3, HAL_MAX_DELAY);
  	      HAL_UART_Transmit(&huart6, (uint8_t *)password, strlen((char *)password), HAL_MAX_DELAY);
  	      HAL_UART_Transmit(&huart6, (uint8_t *)"\"\r\n", 3, HAL_MAX_DELAY);
  	  	  HAL_Delay(8000);
  	  	if (strstr((char*)rx_buffer, "WIFI GOT IP\r\n\r\nOK\r\n") != NULL) {
  	  	  		  HAL_UART_Transmit(&huart2, (uint8_t *)"Connesso al router\r\n", strlen("Connesso al router\r\n"), HAL_MAX_DELAY);
  	  	  		  response_received = 1;} // Imposta il flag della risposta attesa

  	  	  }while(response_received != 1);


//inizializzo una connessione tcp per il collegamento al broker hivemqtt

  	 do{	send_at_command("AT+CIPSTART=\"TCP\",\"broker.hivemq.com\",1883");
  	 	 	 HAL_Delay(1500);
  	  	  	  if (strstr((char*)rx_buffer, "CONNECT\r\n") != NULL) {
  	  		  HAL_UART_Transmit(&huart2, (uint8_t *)"Connessione MQTT riuscita\r\n", strlen("Connessione MQTT riuscita\r\n"), HAL_MAX_DELAY);
  	  		  response_received = 1;} // Imposta il flag della risposta attesa
  	  	          }while(response_received != 1);



//Mando il pacchetto per la connessione e il client id
  	 do{	  	MQTT_Connect(MQTT_CLIENT_ID);

  	  	  	  	  if (strstr((char*)rx_buffer, "SEND OK\r\n") != NULL) {
  	  	  		  HAL_UART_Transmit(&huart2, (uint8_t *)"Client ID e pacchetto mandato\r\n", strlen("Client ID e pacchetto mandato\r\n"), HAL_MAX_DELAY);
  	  	  		  response_received = 1;} // Imposta il flag della risposta attesa
  	  	  	          }while(response_received != 1);


//pubblico il messaggio in un determinato topic
  	 do{	  	MQTT_Publish(MQTT_TOPIC,MQTT_MESSAGE);

  	  	  	  	  if (strstr((char*)rx_buffer, "SEND OK\r\n") != NULL) {
  	  	  		  HAL_UART_Transmit(&huart2, (uint8_t *)"Messaggio pubblicato\r\n", strlen("Messaggio pubblicato\r\n"), HAL_MAX_DELAY);
  	  	  		  response_received = 1;} // Imposta il flag della risposta attesa
  	  	  	          }while(response_received != 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  //visualizzo tutte le risposte che ho nel buffer, ricevute dal modulo esp
	  while(c<1 ){
	 		  HAL_UART_Transmit(&huart2, (uint8_t*)&rx_buffer[c], RX_BUFFER_SIZE , HAL_MAX_DELAY);
	 		  c++;}

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Reset_esp_Pin|Enable_esp_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Reset_esp_Pin Enable_esp_Pin */
  GPIO_InitStruct.Pin = Reset_esp_Pin|Enable_esp_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void MQTT_Connect(const char* client_id) {
    char cmd[256];
    int client_id_len = strlen(client_id);
    int packet_len = 12 + client_id_len; // Lunghezza del pacchetto MQTT CONNECT
    int cmd_length;

    // Calcola la lunghezza del comando AT+CIPSEND
    cmd_length = sprintf(cmd, "AT+CIPSEND=%d\r\n", packet_len + 2);
    HAL_UART_Transmit(&huart6, (uint8_t*)cmd, cmd_length, HAL_MAX_DELAY);

    // Attendi per un po' (200 ms)
    HAL_Delay(200);

    // Costruisci il pacchetto MQTT CONNECT
    uint8_t connect_packet[256];
    int index = 0;
    connect_packet[index++] = 0x10; // Tipo di messaggio: CONNECT
    connect_packet[index++] = packet_len; // Lunghezza del rimanente

    // Variabile header
    connect_packet[index++] = 0x00; connect_packet[index++] = 0x04; // Lunghezza del protocollo
    connect_packet[index++] = 'M'; connect_packet[index++] = 'Q'; connect_packet[index++] = 'T'; connect_packet[index++] = 'T'; // Nome del protocollo
    connect_packet[index++] = 0x04; // Livello del protocollo
    connect_packet[index++] = 0x02; // Connessione flag: Clean session
    connect_packet[index++] = 0x00; connect_packet[index++] = 0x3C; // Keep-alive: 60 secondi

    // Payload
    connect_packet[index++] = 0x00; connect_packet[index++] = client_id_len; // Lunghezza del client ID
    memcpy(&connect_packet[index], client_id, client_id_len); // Client ID
    index += client_id_len;

    // Invia il pacchetto CONNECT
    HAL_UART_Transmit(&huart6, connect_packet, index, HAL_MAX_DELAY);

    // Attendi per un po' (2000 ms) per completare la connessione
    HAL_Delay(2000);
}

void MQTT_Publish(const char* topic, const char* message) {
    char cmd[256];
    int topic_len = strlen(topic);
    int message_len = strlen(message);
    int msg_len = topic_len + message_len + 2; // Lunghezza del topic + lunghezza del messaggio + 2 byte per la lunghezza del topic
    int cmd_length;

    // Calcola e invia il comando AT+CIPSEND
    cmd_length = sprintf(cmd, "AT+CIPSEND=%d\r\n", msg_len + 2);
    HAL_UART_Transmit(&huart6, (uint8_t*)cmd, cmd_length, HAL_MAX_DELAY);

    // Attendi per un po' (200 ms)
    HAL_Delay(200);

    // Costruisci il pacchetto MQTT PUBLISH
    uint8_t publish_packet[256];
    int index = 0;
    publish_packet[index++] = 0x30; // Tipo di messaggio: PUBLISH
    publish_packet[index++] = msg_len; // Lunghezza del rimanente

    // Variabile header
    publish_packet[index++] = 0x00; publish_packet[index++] = topic_len; // Lunghezza del topic
    memcpy(&publish_packet[index], topic, topic_len); // Topic
    index += topic_len;

    // Payload
    memcpy(&publish_packet[index], message, message_len); // Messaggio
    index += message_len;

    // Invia il pacchetto PUBLISH
    HAL_UART_Transmit(&huart6, publish_packet, index, HAL_MAX_DELAY);

    // Attendi per un po' (1000 ms) per completare la pubblicazione
    HAL_Delay(1000);
}




void send_at_command(const char *command) {
    strcpy((char *)tx_buffer, command);
    HAL_UART_Transmit(&huart6, tx_buffer, strlen(command), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart6, (uint8_t *)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);

}





void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART6) {
        if (rx_index < RX_BUFFER_SIZE - 1) {

rx_index++; }else {
           rx_index = 0;  // Gestione overflow
}
HAL_UART_Receive_IT(&huart6, rx_buffer + rx_index, 1); // Ripristina l'interrupt di ricezione
}}


void WiFi_Init()
{
 HAL_GPIO_WritePin(GPIOB, Enable_esp_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, Reset_esp_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, Reset_esp_Pin, GPIO_PIN_SET);
  HAL_Delay(1500);
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
