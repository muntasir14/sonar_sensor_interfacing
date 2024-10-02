/*
 * usart_lib.h
 *
 * Created: 9/19/2024 6:05:29 PM
 *  Author: Ikramuzzaman Muntasir
 */ 

#ifndef USART_LIB_H_
#define USART_LIB_H_

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void USART_Transmit_String(char *str);


#endif /* USART_LIB_H_ */