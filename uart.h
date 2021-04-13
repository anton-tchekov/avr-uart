#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

#define UART_BAUD                 9600

void uart_init(void);
uint8_t uart_available(void);
void uart_tx(char c);
int16_t uart_rx(void);
void uart_tx_str(const char *s);
void uart_tx_str_P(const char *s);

#endif /* __UART_H__ */
