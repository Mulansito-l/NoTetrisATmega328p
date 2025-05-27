/*
 * joystick.c
 *
 * Created: 19/05/2025 03:39:48 p. m.
 *  Author: andre
 */ 
#include <avr/io.h>
#include "joystick.h"

void Joystick_init(Joystick* js, uint8_t xPin, uint8_t yPin, uint8_t zPin) {
	js->pinX = xPin;
	js->pinY = yPin;
	js->pinZ = zPin;

	DDRD &= ~(1 << js->pinZ);
	PORTD |= (1 << js->pinZ);

	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t readADC(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

int Joystick_readX(Joystick* js) {
	return readADC(js->pinX);
}

int Joystick_readY(Joystick* js) {
	return readADC(js->pinY);
}

int Joystick_readZ(Joystick* js) {
	return !(PIND & (1 << js->pinZ));
}
