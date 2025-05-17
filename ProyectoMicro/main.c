#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "Colors.h"
#include "GLCD.h"

uint8_t piezaI[4][4] = {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}};
uint8_t piezaL[4][4] = {{0,0,0,0},{0,0,1,0},{0,0,1,0},{0,0,1,1}};
uint8_t piezaO[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,1,1},{0,0,1,1}};
uint8_t piezaJ[4][4] = {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,1,1,0}};
uint8_t piezaS[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,1,1},{0,1,1,0}};
uint8_t piezaZ[4][4] = {{0,0,0,0},{0,0,0,0},{0,1,1,0},{0,0,1,1}};
uint8_t piezaT[4][4] = {{0,0,0,0},{0,0,0,0},{0,1,1,1},{0,0,1,0}};
	
int dibujar = 0;

typedef struct s_Pieza{
	unsigned int color;
	uint8_t forma[4][4];
	int x,y;
} Pieza;

typedef struct s_Tablero{
	Pieza* celdas[10][24];
} Tablero;

Tablero tablero;
Pieza* actual;
Pieza pNueva;

void agregarPieza(){
	pNueva.color = 0xf67a;
	pNueva.x = 5;
	pNueva.y = 4;
	memcpy(pNueva.forma, piezaS, 16);
	
	actual = &pNueva;
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (pNueva.forma[j][i] == 1)
			{
				tablero.celdas[(pNueva.x - 2) + i][(pNueva.y - 6) + j] = actual;
			}
		}
	}
}

void dibujarTablero(Tablero* tablero){
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 24; j++)
		{
			if(tablero->celdas[i][j] != NULL){
				Pieza* p = tablero->celdas[i][j];
				glcd_fillRectangle(i * 16 + 80, j * 16, 16, 16, p->color);
			}else{
				glcd_fillRectangle(i * 16 + 80, j * 16, 16, 16, 0xf7ba);
			}
		}
	}
}

int tiempo_caida = 1000;

void init_timer(){
	TCNT1 = 0;
	TCCR1B |= (1<<CS12)|(0<<CS11)|(1<<CS10);
	TCCR1B |= (1<<WGM12);
	OCR1A = (tiempo_caida / 1000) / (1.0f/(F_CPU / 1024.0f));
	TIMSK1 = (1<<OCIE1A);
	sei();
}

ISR(TIMER1_COMPA_vect){
	for (int j = 23; j>= 0; j--)
	{
		for (int i = 0; i < 10; i++)
		{
			if(tablero.celdas[i][j] == actual){
				tablero.celdas[i][j] = NULL;
				tablero.celdas[i][j + 1] = actual;
			}
		}
	}
	
	dibujar = 1;
}

int main(void)
{
	agregarPieza();
	char strPuntaje[30];
	int score = 0;
	
	glcd_init();
	glcd_setOrientation(PORTRAIT);
	glcd_bg(0xf7ba);
	
	// Area de interfaz 80x320, Area de juego 160x320
	// tablero debe tener un tamaño de 10 de ancho y 20 de alto
	// cada cuadro del tablero será de 16x16 pixeles
	glcd_fillRectangle(0,0,80,320,0x8bf7);
    
	init_timer();
	
    while (1) 
    {
		glcd_string("NoTetris",16,10, 1,0xffff);
		sprintf(strPuntaje, "Puntaje:");
		glcd_string(strPuntaje, 10, 40, 1,0xffff);
		sprintf(strPuntaje, "%d",score);
		glcd_string(strPuntaje, 10, 50, 1,0xffff);
		glcd_string("Hecho por", 12, 270, 1,0xffff);
		glcd_string("Andrea", 23, 280, 1,0xffff);
		glcd_string("y", 36, 290, 1,0xffff);
		glcd_string("Diego", 24, 300, 1,0xffff);
		
		if(dibujar == 1){
			dibujarTablero(&tablero);
			dibujar = 0;
		}
    }
}

