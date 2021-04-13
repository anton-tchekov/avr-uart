#include "uart.h"
#include <avr/pgmspace.h>

int main(void)
{
	int16_t c;
	uart_init();
	uart_tx_str_P(PSTR("Hello World!"));
	for(;;)
	{
		c = uart_rx();
		if(c > 0)
		{
			uart_tx(c);
		}
	}

	return 0;
}
