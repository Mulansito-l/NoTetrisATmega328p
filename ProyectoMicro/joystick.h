#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>

typedef struct {
	uint8_t pinX;
	uint8_t pinY;
	uint8_t pinZ;
} Joystick;

void Joystick_init(Joystick* js, uint8_t xPin, uint8_t yPin, uint8_t zPin);
int Joystick_readX(Joystick* js);
int Joystick_readY(Joystick* js);
int Joystick_readZ(Joystick* js);

#endif

