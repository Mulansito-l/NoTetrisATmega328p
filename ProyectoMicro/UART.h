/*
 * UART.h
 *
 * Created: 19/05/2025 03:44:01 p. m.
 *  Author: andre
 */ 


#ifndef UART_H_
#define UART_H_

void UART_init();             	// Inicializa UART: As�ncrono, 8-bit, 1-bit stop, 9600 baud
unsigned char UART_read();    	// Funci�n para leer datos
void UART_write(unsigned char);   // Funci�n para escribir un dato
void UART_string(char*);      	// Funci�n para escribir una cadena
int UART_read_two_digit_number();

void UART_init(int baud_rate, int data_size, int parity_mode, int stop_bits) {
	UCSR0A &= ~(1 << U2X0);    	// Deshabilita doble velocidad USART
	UCSR0A &= ~(1 << MPCM0);   	// Deshabilita modo multiprocesador

	UCSR0B |= (1 << TXEN0);    	// Habilita transmisi�n
	UCSR0B |= (1 << RXEN0);    	// Habilita recepci�n
	//UCSR0B |= (1 << UDRIE0);   	// Habilita interrupci�n por buffer vac�o
	//UCSR0B |= (1 << TXCIE0);   	// Habilita interrupci�n de transmisi�n completa
	//UCSR0B |= (1 << RXCIE0);   	// Habilita interrupci�n de recepci�n completa

	UCSR0C &= ~((1 << UMSEL01) | (1 << UMSEL00));  // Modo: 00 = As�ncrono
	
	UCSR0C &= ~((1 << UPM01) | (1 << UPM00));
	
	switch(parity_mode){
		case 0:
		UCSR0C &= ~((1 << UPM01) | (1 << UPM00));   // Paridad: 00 = Ninguna
		break;
		case 2:
		UCSR0C &= ~((1 << UPM01) | (1 << UPM00));   // Paridad: 10 = Par
		UCSR0C |= 1 << UPM01;
		break;
		case 3:
		UCSR0C &= ~((1 << UPM01) | (1 << UPM00));   // Paridad: 11 = Impar
		UCSR0C |= ((1 << UPM01) | (1 << UPM00));
		break;
		default:
		UCSR0C &= ~((1 << UPM01) | (1 << UPM00));   // Paridad: 00 = Ninguna
	}

	
	switch(stop_bits){
		case 0:
		UCSR0C &= ~(1 << USBS0);   	// Bit de parada: 0 = 1-bit
		break;
		case 1:
		UCSR0C |= (1 << USBS0);   	// Bit de parada: 1 = 1-bit
		default:
		UCSR0C &= ~(1 << USBS0);   	// Bit de parada: 0 = 1-bit
	}

	switch(data_size){
		case 0:
		UCSR0C &= ~((1 << UCSZ01) | (1 << UCSZ00));  // Tama�o de datos: 00 = 5-bit
		break;
		case 1:
		UCSR0C &= ~((1 << UCSZ01) | (1 << UCSZ00));
		UCSR0C |= (0 << UCSZ01) | (1 << UCSZ00);  // Tama�o de datos: 01 = 6-bit
		break;
		case 2:
		UCSR0C &= ~((1 << UCSZ01) | (1 << UCSZ00));
		UCSR0C |= (1 << UCSZ01);  // Tama�o de datos: 10 = 7-bit
		break;
		case 3:
		UCSR0C &= ~((1 << UCSZ01) | (1 << UCSZ00));
		UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);  // Tama�o de datos: 11 = 8-bit
		break;
		case 7:
		UCSR0C &= ~((1 << UCSZ01) | (1 << UCSZ00));
		UCSR0C |= (1 << UCSZ02)|(1 << UCSZ01) | (1 << UCSZ00);  // Tama�o de datos: 111 = 9-bit
		break;
		default:
		UCSR0C &= ~((1 << UCSZ01) | (1 << UCSZ00));
		UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);  // Tama�o de datos: 11 = 8-bit
		break;
	}
	
	UCSR0C &= ~(1 << UCPOL0);  	// Polaridad de reloj: 0 para as�ncrono

	UBRR0 = (16000000.0f)/(16.0*baud_rate) - 1;
}

unsigned char UART_read() {
	if (UCSR0A & (1 << RXC0)) { // Si el flag de recepci�n completa est� activado
		return UDR0;       	// Retorna el dato recibido en UDR0
	}
	return 0;              	// Si no, retorna 0
}



void UART_write(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));  // Espera mientras el registro de datos est� lleno
	UDR0 = data;                  	// Env�a el dato
}

void UART_string(char* string) {
	while (*string != 0) {         	// Mientras no sea el final de la cadena
		UART_write(*string);       	// Env�a el car�cter actual
		string++;                  	// Apunta al siguiente car�cter
	}
}

int UART_read_two_digit_number() {
	int number = -1;
	unsigned char digit1, digit2;

	do {
		digit1 = UART_read();
	} while (digit1 < '0' || digit1 > '9');
	
	do {
		digit2 = UART_read();
	} while (digit2 < '0' || digit2 > '9');

	if (digit1 >= '0' && digit1 <= '9' && digit2 >= '0' && digit2 <= '9') {
		number = (digit1 - '0') * 10 + (digit2 - '0');
	}

	return number;
}


#endif /* UART_H_ */
