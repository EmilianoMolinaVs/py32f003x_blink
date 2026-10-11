
#include "py32f0xx_hal.h"
#include <string.h>

// ==== Declaración de Variables Globales ====
UART_HandleTypeDef huart1; // Declaración del manejador del puerto serial

// ==== Prototipos de Funciones ====
void APP_SystemClockConfig(void);
void MX_UART1_Init(void);

int main(void)
{
    HAL_Init();              // Inicializar la librería HAL
    APP_SystemClockConfig(); // Configurar el reloj del sistema
    MX_UART1_Init();         // Inicializar el UART1

    char *mensaje = "ola cara de bola!\r\n";

    while (1)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)mensaje, strlen(mensaje), HAL_MAX_DELAY);
        HAL_Delay(1000);
    }

    /*
        // Habilitar el reloj para el puerto GPIOB
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 4. Configurar el pin como salida (PB5)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    */
}

// ==== DECLARACIÓN DE FUNCIONES ====
void MX_UART1_Init(void)
{
    // Habiilitar el reloj para el puerto y el USART
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configurar GPIOs en su función alternativa UART
    // PA0: TX Py32F003
    // PA1: RX Py32F003
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Alternate = GPIO_AF9_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configurar los parámetros del UART
    huart1.Instance = USART2;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    // Inicializar el hardware
    HAL_UART_Init(&huart1);
}

// Configuración básica del reloj interno (HSI)
void APP_SystemClockConfig(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

// Interrupción del sistema requerida para que funcione HAL_Delay()
void SysTick_Handler(void)
{
    HAL_IncTick();
}