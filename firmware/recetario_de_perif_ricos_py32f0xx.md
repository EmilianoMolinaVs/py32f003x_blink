# 📖 Recetario de Periféricos PY32F0xx (Librería HAL)

Este documento es una guía rápida ("receta de cocina") para configurar y utilizar los periféricos más comunes del microcontrolador PY32 usando la librería oficial HAL (Hardware Abstraction Layer). 

**Nota sobre los modos de ejecución:**
Todas las recetas a continuación utilizan el modo **Polling (Bloqueante)**. Esto significa que el microcontrolador se quedará esperando (con un `Timeout`) hasta que la acción termine. Es el método más fácil para empezar.

---

## 1. 🎛️ ADC (Lecturas Analógicas)
**Teoría:** Convierte un voltaje analógico (0 a VCC) en un número digital (0 a 4095 para 12 bits). Ideal para potenciómetros, sensores de luz (LDR) o medidores de batería.
**Pines:** Cualquier pin etiquetado como `ADC_INx` (usualmente los PA0 a PA7).

### Funciones clave:
1. `HAL_ADC_Start(&hadc)`: Enciende el módulo ADC y comienza a medir.
2. `HAL_ADC_PollForConversion(&hadc, Timeout)`: Espera a que la medición termine.
3. `HAL_ADC_GetValue(&hadc)`: Extrae el número medido.
4. `HAL_ADC_Stop(&hadc)`: Apaga el módulo.

### 🍳 Receta de uso:
```c
uint32_t valor_analogo = 0;

// 1. Iniciar la conversión
HAL_ADC_Start(&hadc);

// 2. Esperar a que termine (Timeout de 10ms)
if (HAL_ADC_PollForConversion(&hadc, 10) == HAL_OK) {
    // 3. Leer el valor y guardarlo
    valor_analogo = HAL_ADC_GetValue(&hadc);
}

// 4. Detener el ADC
HAL_ADC_Stop(&hadc);
```

---

## 2. 💬 UART (Comunicación Serial)
**Teoría:** Protocolo asíncrono para enviar texto o datos byte a byte. Muy usado para enviar mensajes (printf) a la computadora o hablar con módulos Bluetooth/GPS.
**Pines:** TX (Transmisor) y RX (Receptor). Se cruzan con los del otro dispositivo (TX con RX).

### Funciones clave:
1. `HAL_UART_Transmit(&huart, pData, Size, Timeout)`: Envía un arreglo de datos.
2. `HAL_UART_Receive(&huart, pData, Size, Timeout)`: Lee datos entrantes.

### 🍳 Receta de uso (Enviar un mensaje):
```c
char mensaje[] = "Hola desde el PY32!\r\n";

// Enviar el mensaje completo
// Parámetros: Handler, datos (casteados a uint8_t), tamaño del mensaje, Timeout
HAL_UART_Transmit(&huart1, (uint8_t*)mensaje, sizeof(mensaje), 100);
```

### 🍳 Receta de uso (Recibir un byte):
```c
uint8_t dato_recibido;

// Escuchar el puerto serial esperando 1 byte (Timeout de 50ms)
if (HAL_UART_Receive(&huart1, &dato_recibido, 1, 50) == HAL_OK) {
    // Hacer algo con el dato recibido
    if (dato_recibido == 'A') {
        // Encender LED
    }
}
```

---

## 3. 🤝 I2C (Inter-Integrated Circuit)
**Teoría:** Bus de comunicación síncrono ideal para cortas distancias. Permite conectar múltiples sensores o pantallas OLED usando solo 2 cables. Cada esclavo tiene una dirección (Address) hexadecimal única.
**Pines:** SDA (Datos) y SCL (Reloj). Requieren resistencias Pull-Up.

### Funciones clave:
1. `HAL_I2C_IsDeviceReady(&hi2c, DevAddress, Trials, Timeout)`: Hace "ping" para ver si el dispositivo está conectado.
2. `HAL_I2C_Master_Transmit(&hi2c, DevAddress, pData, Size, Timeout)`: Escribe datos al esclavo.
3. `HAL_I2C_Master_Receive(&hi2c, DevAddress, pData, Size, Timeout)`: Pide datos al esclavo.

### 🍳 Receta de uso (Escribir en un registro de un sensor):
```c
uint16_t direccion_sensor = 0x68 << 1; // La dirección de 7 bits se desplaza 1 bit a la izquierda
uint8_t datos_a_enviar[2] = {0x00, 0xFF}; // {Registro, Valor a escribir}

// Enviar 2 bytes al sensor
HAL_I2C_Master_Transmit(&hi2c1, direccion_sensor, datos_a_enviar, 2, 100);
```

### 🍳 Receta de uso (Leer desde un sensor):
```c
uint8_t registro_a_leer = 0x01;
uint8_t valor_leido;

// Primero: Decirle al sensor qué registro queremos leer
HAL_I2C_Master_Transmit(&hi2c1, direccion_sensor, &registro_a_leer, 1, 100);

// Segundo: Pedirle el dato y guardarlo en valor_leido
HAL_I2C_Master_Receive(&hi2c1, direccion_sensor, &valor_leido, 1, 100);
```

---

## 4. 🚀 SPI (Serial Peripheral Interface)
**Teoría:** Bus muy rápido (full-duplex) para dispositivos que manejan muchos datos, como pantallas TFT a color o memorias SD.
**Pines:** MOSI (Sale del Master), MISO (Entra al Master), SCK (Reloj), CS/NSS (Chip Select, para elegir con quién hablar).

### Funciones clave:
1. `HAL_SPI_Transmit(&hspi, pData, Size, Timeout)`: Envía datos.
2. `HAL_SPI_Receive(&hspi, pData, Size, Timeout)`: Recibe datos.
3. `HAL_GPIO_WritePin(Puerto, Pin, Estado)`: Necesario para controlar el pin CS manualmente.

### 🍳 Receta de uso:
```c
uint8_t dato_enviar = 0x55;
uint8_t dato_recibir;

// 1. Bajar el pin CS a LOW (Activar el esclavo)
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

// 2. Enviar y recibir datos
// A) Solo enviar:
HAL_SPI_Transmit(&hspi1, &dato_enviar, 1, 100);

// B) O enviar y recibir al mismo tiempo:
// HAL_SPI_TransmitReceive(&hspi1, &dato_enviar, &dato_recibir, 1, 100);

// 3. Subir el pin CS a HIGH (Desactivar el esclavo)
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
```