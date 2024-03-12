## Esp8266-01S Wifi chip library (ARM Cortex Mx STM32) using FIFO/Circular buffer

Download [ESP8266 (AT Commands instruction set) datasheet](https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf)
     
      
## Pinout:
**UART Modules:** 
* _UART1_ of STM32 (to use with ESP8266-01S) 
* _UART2_ of STM32 (to use with Desktop Application to get the Debug messages). [Nucleo F411re User Guide: page 25, 6.8 USART communication](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf) 

| ESP-01S pins | STM32F4 pins |
|:-------------|-------------:|
| RS (RESET)   | PA8 or 3.3v  |
| EN           | 3.3v         |
| I01 (GPIO 0) | 3.3v         |
| I02 (GPIO 2) | 3.3v         |
| VCC          | 3.3v         |
| GND          | GND          |
| Tx           | PA10 (Rx)    |
| Rx           | PA9  (Tx)    |    
     
      
