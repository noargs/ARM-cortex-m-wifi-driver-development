#ifndef ESP82XX_LIB_H_
#define ESP82XX_LIB_H_

#include <stdint.h>

//void esp82xx_server_init(char *ssid, char *password);
void esp82xx_server_begin(void);
void esp82xx_thingspeak_send(char* apikey, int field_no, uint32_t sensor_value);
void esp82xx_device_init(char *ssid, char *password);


#endif /* ESP82XX_LIB_H_ */
