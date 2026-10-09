# Proyecto PY32F003: Configuración y Blink Básico

Este repositorio contiene la estructura base y la configuración necesaria para programar microcontroladores **Puya PY32F003** utilizando lenguaje C, el toolchain de GNU Arm y pyOCD en un entorno de Windows (VS Code + Git Bash).

## 🛠️ 1. Herramientas Necesarias (Prerrequisitos)

Para compilar y cargar el código en Windows, necesitas instalar los siguientes paquetes:

### A. GNU Make
Encargado de leer el archivo `Makefile` y automatizar la compilación.
*   **Instalación:** Abre PowerShell y ejecuta:
    ```bash
    winget install GnuWin32.Make
    ```
*   **Nota de entorno:** Asegúrate de agregar la ruta `C:\Program Files (x86)\GnuWin32\bin` a las variables de entorno (`Path`) del sistema.

### B. ARM GNU Embedded Toolchain
El compilador cruzado que traduce tu código C a código máquina para la arquitectura ARM Cortex-M0+.
*   **Instalación:** Ejecuta en tu terminal:
    ```bash
    winget install Arm.GnuArmEmbeddedToolchain
    ```
*   **Nota de entorno:** Marca la casilla "Add path to environment variable" durante la instalación, o agrega manualmente la carpeta `bin` del compilador a tu `Path`.

### C. pyOCD (Programador)
Herramienta en Python para interactuar con sondas de depuración (DAPLink/J-Link) y flashear el chip.
*   **Instalación:** Necesitas tener Python instalado. Ejecuta:
    ```bash
    pip install pyOCD
    ```

---

## ⚙️ 2. Configuración del Makefile

El archivo `Makefile` es el corazón del proyecto. Si descargas una plantilla de Linux, debes hacer estos ajustes para que funcione en Windows:

1.  **Rutas del compilador:** En `rules.mk` o en el `Makefile`, asegúrate de que el compilador no tenga prefijos de Linux (`/usr/bin/`). Debe llamarse directamente:
    ```makefile
    PREFIX = arm-none-eabi-
    ```
2.  **Archivos fuente (`CFILES`):** Debes listar explícitamente los archivos `.c` que vas a compilar. Si usas librerías HAL, debes incluirlas aquí:
    ```makefile
    CFILES := src/main.c \
              Libraries/PY32F0xx_HAL_Driver/Src/py32f0xx_hal.c \
              Libraries/PY32F0xx_HAL_Driver/Src/py32f0xx_hal_rcc.c \
              Libraries/PY32F0xx_HAL_Driver/Src/py32f0xx_hal_gpio.c \
              Libraries/PY32F0xx_HAL_Driver/Src/py32f0xx_hal_cortex.c
    ```

---

## 💻 3. Interactuando con el PY32 (Librería HAL)

El código interactúa con el microcontrolador Puya mediante la **HAL (Hardware Abstraction Layer)**. Esta librería oficial de Puya te proporciona comandos en C (funciones) para no tener que modificar los registros de memoria manualmente.

Aquí tienes los comandos más comunes:

### Inicialización del Sistema
*   `HAL_Init();` : Es lo primero que debes llamar en `main()`. Configura el sistema base y el contador de milisegundos (`SysTick`).
*   `HAL_RCC_OscConfig(...)` y `HAL_RCC_ClockConfig(...)` : Configuran la velocidad del chip (ej. el oscilador interno HSI a 24MHz).

### Habilitar Relojes (¡Muy importante!)
Antes de usar cualquier puerto (pines, I2C, ADC), debes "encender" su reloj de energía, de lo contrario el chip ignorará tus comandos:
*   `__HAL_RCC_GPIOA_CLK_ENABLE();` : Enciende el reloj para todos los pines del Puerto A.
*   `__HAL_RCC_GPIOB_CLK_ENABLE();` : Enciende el reloj para el Puerto B.

### Control de Pines (GPIO)
Una vez configurado un pin (usando `HAL_GPIO_Init`), puedes controlarlo con estas funciones:
*   **Escribir:** Pone el pin en Alto (3.3V) o Bajo (0V).
    ```c
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Enciende pin
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // Apaga pin
    ```
*   **Invertir (Toggle):** Cambia el estado actual del pin (ideal para un Blink).
    ```c
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    ```
*   **Leer:** Lee el estado de un pin (ideal para botones o sensores).
    ```c
    uint8_t estado = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    ```

### Tiempos y Pausas
*   `HAL_Delay(500);` : Pausa el microcontrolador durante 500 milisegundos.
*   `HAL_GetTick();` : Devuelve cuántos milisegundos han pasado desde que el chip se encendió (muy útil para medir tiempos sin usar `HAL_Delay` que bloquea el código).

---

## 🚀 4. Comandos Básicos de Terminal

Dentro de la carpeta `firmware/`, utiliza estos comandos en Git Bash:

*   `make clean` : Borra archivos de compilaciones anteriores (¡úsalo si cambias algo en el Makefile o si da errores raros!).
*   `make` : Compila el código y genera el archivo `firmware.elf`/`.hex`/`.bin`.
*   `make flash` : Se conecta por pyOCD, borra la memoria del chip y graba tu nuevo programa.