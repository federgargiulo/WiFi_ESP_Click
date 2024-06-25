# WiFi_ESP_Click
Questo progetto si basa sull'uso:
1) Nucleo STM32f401re con Arduino Uno Click Shield 
2) Modulo Wifi-Esp-Click
3) Broker Hivemq

Cominucazione 
a) modulo-nucleo: realizzata attraverso Usart6 con modalità interrupt per la gestione delle risposte da parte del modulo;
b)modulo-terminale: realizzata attraverso Usart2.

Sia per Usart6 che per Usart 2 i parametri utilizzati sono i seguenti:
BaudRate = 115200;
WordLength = UART_WORDLENGTH_8B;
StopBits = UART_STOPBITS_1;
Parity = UART_PARITY_NONE;
Mode = UART_MODE_TX_RX;
HwFlowCtl = UART_HWCONTROL_NONE;
OverSampling = UART_OVERSAMPLING_16;

Hardware
PB6-PB0 corrispondono rispettivamente a Enable-Reset del modulo da impostati come GPIO_Output dal file .ioc del progetto 
Usart2 è collegata a PA2 e PA3 
Usart6 è collegata a PC7 e PC8


Note: 
Come terminale ho utilizzato TeraTerm dove alcuni caratteri inizialmente non erano visualizzati in quanto bisogna impostare come linguaggio di comunicazione UTF-8.
