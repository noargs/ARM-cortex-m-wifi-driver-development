#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "fifo.h"
#include "esp82xx_lib.h"

#define SSID_NAME                 "__hw_wifi__"
#define PASSKEY                   "dude@dude.com"

char packet[] = "GET /data/2.5/weather?q=Dublin&appid={API_KEYS_HERE} HTTP/1.1\r\nHost: api.openweathermap.org\r\n\r\n";


int main()
{
	esp82xx_init(SSID_NAME, PASSKEY);

	while(1)
	{
		if (esp82xx_create_tcp_conn("82.196.7.246"))
		{
			esp82xx_send_tcp_pckt(packet);
		}
		esp82xx_close_tcp_connection();
	}
}
