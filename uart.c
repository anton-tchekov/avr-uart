#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define UART_RECEIVE_BUFFER_SIZE  16
#define UART_TRANSMIT_BUFFER_SIZE 16
#define _BAUD                       (((F_CPU / (UART_BAUD * 16UL))) - 1)

/* receive buffer */
static volatile uint8_t _avail;
static volatile char _rb_start[UART_RECEIVE_BUFFER_SIZE],
	*_rb_end, *_rb_read, *_rb_write;

/* transmit buffer */
static volatile uint8_t _tb_available;
static volatile char _tb_start[UART_TRANSMIT_BUFFER_SIZE],
	*_tb_end, *_tb_read, *_tb_write;

void uart_init(void)
{
	UCSR0B = (1 << RXCIE0) | (1 << UDRIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0 = _BAUD;
	_rb_read = _rb_write = _rb_start;
	_tb_read = _tb_write = _tb_start;
}

uint8_t uart_available(void)
{
	return _avail;
}

void uart_tx(char c)
{
	if(UCSR0A & (1 << UDRE0))
	{
		UDR0 = c;
	}
	else
	{
		while(_tb_available >= UART_TRANSMIT_BUFFER_SIZE - 1) ;

		++_tb_available;
		*_tb_write++ = c;
		if(_tb_write == _tb_end)
		{
			_tb_write = _tb_start;
		}
	}
}

int16_t uart_rx(void)
{
	int16_t c = -1;
	if(_avail)
	{
		--_avail;
		c = *_rb_read++;
		if(_rb_read == _rb_end)
		{
			_rb_read = _rb_start;
		}
	}

	return c;
}

void uart_tx_str(const char *s)
{
	register char c;
	while((c = *s++))
	{
		uart_tx(c);
	}
}

void uart_tx_str_P(const char *s)
{
	register char c;
	while((c = pgm_read_byte(s++)))
	{
		uart_tx(c);
	}
}

ISR(USART_RX_vect)
{
	if(_avail < UART_RECEIVE_BUFFER_SIZE)
	{
		++_avail;
		*_rb_write++ = UDR0;
		if(_rb_write == _rb_end)
		{
			_rb_write = _rb_start;
		}
	}
}

ISR(USART_UDRE_vect)
{
	if(_tb_available)
	{
		--_tb_available;
		UDR0 = *_tb_read++;
		if(_tb_read == _tb_end)
		{
			_tb_read = _tb_start;
		}
	}
}

