#ifndef ESP82XX_LIB_H_
#define ESP82XX_LIB_H_

#include <stdint.h>

void esp82xx_server_init(char *ssid, char *password);
int8_t send_server_data(char *str, int link_id);

#endif /* ESP82XX_LIB_H_ */
