#ifndef ESP82XX_LIB_H_
#define ESP82XX_LIB_H_

#include <stdio.h>
#include "esp82xx_driver.h"

void esp82xx_init(const char* ssid, const char* password);
uint8_t esp82xx_create_tcp_conn(char* ip_addr);
uint8_t esp82xx_send_tcp_pckt(char* pckt);
uint8_t esp82xx_close_tcp_connection(void);

#endif /* ESP82XX_LIB_H_ */
