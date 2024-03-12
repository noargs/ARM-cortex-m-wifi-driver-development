#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "fifo.h"
#include "esp82xx_lib.h"

#define SSID_NAME                 "__hw_wifi__"
#define PASSKEY                   "dude@dude.com"



int main()
{
	esp82xx_init(SSID_NAME, PASSKEY);

	while(1)
	{

	}
}
