/*
 * EEPROM.h
 *
 * Created: 30/03/2025 12:27:45 p. m.
 *  Author: Mulansito
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

unsigned char EEPROM_read(unsigned int);
void EEPROM_write(unsigned int, unsigned char);
void EEPROM_update(unsigned int, unsigned char);

unsigned char EEPROM_read(unsigned int uiAddress){
	while(EECR & (1 << EEPE));
	EEAR = uiAddress;
	EECR |= (1 << EERE);
	return EEDR;
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData){
	while(EECR & (1 << EEPE));
	EEAR = uiAddress;
	EEDR = ucData;
	EECR |= (1<<EEMPE);
	EECR |=(1<<EEPE);
}

void EEPROM_update(unsigned int uiAddress, unsigned char ucData){
	if(EEPROM_read(uiAddress) != ucData)
		EEPROM_write(uiAddress,ucData);
}

#endif /* EEPROM_H_ */