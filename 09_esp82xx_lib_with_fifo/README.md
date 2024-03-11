### Esp8266-01S Wifi chip library (ARM Cortex Mx STM32) using FIFO

Download [ESP8266 (AT Commands instruction set) datasheet](https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf)
     
      
#### Pinout:
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
     
      
**Following functionalities are implemented**     
      
- Send command to ESP82xx chip    
    
- Initialise string search in Rx data stream    
     
- Search for string in Rx data stream     
      
- Look for server response in Rx data stream     
      
- Copy content of UART DR into Fifo    
     
- Copy content of Fifo into UART DR    
        
- Output UART character     
      
- Callback function for ESP82xx UART    
     
- ESP82xx UART IRQ Handler     

- Replace your WiFi Name and Password in the `main.c` macros (i.e. `SSID_NAME` and `PASSKEY`)     

- Build/Run the code in your STM32 board and open Serial capture tool _RealTerm_ for Windows or _SerialTools_ for MacOS to get debug output. In order for debug output (USART2) to be seen correctly on Desktop we will keep **Baud Rate** as 115200 **Data bits** as 8, **Stop bits** 1 and **Parity** none
      
       
<img src="images/sc01_08_esp82xx_lib_server_test.png" alt="Serial capture tool" title="Debug output on SerialTools" style="display: inline-block; margin: 0 auto; max-width: 400px">

- Navigate to your Web browser and visit your _Local ip address_ (For it to work, your PC and Esp8266-01S wifi chip have to be connected to same WiFi)   

<img src="images/sc02_08_esp82xx_lib_server_test.png" alt="Serial capture tool" title="Debug output on SerialTools" style="display: inline-block; margin: 0 auto; max-width: 400px">