#ifndef ESP82XX_LIB_H_
#define ESP82XX_LIB_H_

#include <stdio.h>
#include "esp82xx_driver.h"

void esp82xx_init(const char* ssid, const char* password);
void esp82xx_send_cmd(const char* cmd);

#endif /* ESP82XX_LIB_H_ */
