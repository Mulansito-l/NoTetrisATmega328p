#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "Colors.h"
#include "GLCD.h"

#define IZQUIERDA 0
#define DERECHA 1
#define ABAJO 2

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

typedef struct s_Celda{
	uint8_t ocupada;
	unsigned int color;	
} Celda;

typedef struct s_Tablero{
	Celda celdas[10][24];
} Tablero;

Tablero tablero;
Pieza* actual;
Pieza pNueva;

void agregarPieza(){
	
	int numeroAleatorio = rand() % 7;
	pNueva.x = 3;
	pNueva.y = 0;
	
	switch(numeroAleatorio){
		case 0:
			pNueva.color = COLOR_I_PIECE;
			memcpy(pNueva.forma,piezaI, 16);
		break;
		case 1:
			pNueva.color = COLOR_L_PIECE;
			memcpy(pNueva.forma,piezaL, 16);
		break;
		case 2:
			pNueva.color = COLOR_O_PIECE;
			memcpy(pNueva.forma,piezaO, 16);
		break;
		case 3:
			pNueva.color = COLOR_J_PIECE;
			memcpy(pNueva.forma,piezaJ, 16);
		break;
		case 4:
			pNueva.color = COLOR_S_PIECE;
			memcpy(pNueva.forma,piezaS, 16);
		break;
		case 5:
			pNueva.color = COLOR_Z_PIECE;
			memcpy(pNueva.forma,piezaZ, 16);
		break;
		case 6:
			pNueva.color = COLOR_T_PIECE;
			memcpy(pNueva.forma,piezaT, 16);
		break;
	}
	
	actual = &pNueva;
}

void dibujarCayendo(){
	if(actual == NULL){
		return;
	}
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (actual->forma[j][i] == 1)
			{
				glcd_fillRectangle(((actual->x + i) * 16) + 80, (actual->y - 4 + j) * 16, 16 , 16, actual->color);
				glcd_rectangle(((actual->x + i) * 16) + 80, (actual->y - 4 + j) * 16, 16 , 16, 0x0000);
			}
		}
	}
}

void dibujarTablero(){
	for (int i = 0; i < 10; i++)
	{
		for (int j = 4; j < 24; j++)
		{
			if (tablero.celdas[i][j].ocupada == 0)
			{
				glcd_fillRectangle((i * 16) + 80, (j - 4) * 16, 16 , 16, 0xf7ba);
			}else{
				glcd_fillRectangle((i * 16) + 80, (j - 4) * 16, 16 , 16, tablero.celdas[i][j].color);
				glcd_rectangle((i * 16) + 80, (j - 4) * 16, 16 , 16, 0x00000);
			}
		}
	}
}

int comprobarColisiones(int direccion){
	if (direccion == IZQUIERDA)
	{
		actual->x--;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if((actual->x + i) >= 0 && actual->forma[j][i] == 1 && tablero.celdas[actual->x + i][actual->y + j].ocupada == 1){
					actual->x++;
					return 1;
				}
				
				if(actual->forma[j][i] == 1 && (actual->x + i) < 0){
					actual->x++;
					return 1;
				}
			}
		}
	}else if(direccion == DERECHA){
		actual->x++;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if((actual->x + i) < 10 && actual->forma[j][i] == 1 && tablero.celdas[actual->x + i][actual->y + j].ocupada == 1){
					actual->x--;
					return 1;
				}
				
				if(actual->forma[j][i] == 1 && (actual->x + i) >= 10){
					actual->x--;
					return 1;
				}
			}
		}
	}else if(direccion == ABAJO){
		actual->y++;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if((actual->y + j) < 24 && actual->forma[j][i] == 1 && tablero.celdas[actual->x + i][actual->y + j].ocupada == 1){
					actual->y--;
					return 1;
				}
				
				if(actual->forma[j][i] == 1 && (actual->y + j) >= 24){
					actual->y--;
					return 1;
				}
			}
		}
	}
	
	return 0;
}

void colocarPiezaTablero(){
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if(actual->forma[j][i] == 1){
				tablero.celdas[actual->x + i][actual->y + j].ocupada = 1;
				tablero.celdas[actual->x + i][actual->y + j].color = actual->color;
			}
		}
	}
	actual = NULL;
}

void mover(int direccion){
	if(actual == NULL){
		return;
	}
	
	switch(direccion){
		case IZQUIERDA:
			comprobarColisiones(IZQUIERDA);
			break;
		case DERECHA:
			comprobarColisiones(DERECHA);
			break;
		case ABAJO:
			if(comprobarColisiones(ABAJO) == 1){
				colocarPiezaTablero();
			}
			break;
	}
}

int tiempo_caida = 1200;

void init_timer(){
	TCNT1 = 0;
	TCCR1B |= (1<<CS12)|(0<<CS11)|(1<<CS10);
	TCCR1B |= (1<<WGM12);
	OCR1A = (tiempo_caida / 1000) / (1.0f/(F_CPU / 1024.0f));
	TIMSK1 = (1<<OCIE1A);
	sei();
}

ISR(TIMER1_COMPA_vect){
	mover(ABAJO);
}

void iniciarADC() {
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}

uint16_t read_adc() {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADCW;
}

unsigned int generarSemilla() {
	unsigned int semilla = 0;
	int lecturas = 16;

	for (int i = 0; i < lecturas; i++) {
		uint16_t adc_value = read_adc();
		semilla ^= (adc_value & 0x000F) << (i % 12);
		_delay_us(10);
	}

	return semilla;
}


void inicializarTablero(){
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 24; j++)
		{
			tablero.celdas[i][j].ocupada = 0;
			tablero.celdas[i][j].color = 0xfffff;
		}
	}
}

int main(void)
{
	inicializarTablero();
	char strPuntaje[30];
	int score = 0;
	
	glcd_init();
	glcd_setOrientation(PORTRAIT);
	glcd_bg(0xf7ba);
	
	// Area de interfaz 80x320, Area de juego 160x320
	// tablero debe tener un tamaño de 10 de ancho y 20 de alto
	// cada cuadro del tablero será de 16x16 pixeles
	glcd_fillRectangle(0,0,80,320,0x8bf7);
	glcd_rectangle(0,0,79,319,0x0000);
    
	init_timer();
	iniciarADC();
	
	srand(generarSemilla());
	
	glcd_string("Hecho por", 12, 270, 1,0xffff);
	glcd_string("Andrea", 23, 280, 1,0xffff);
	glcd_string("y", 36, 290, 1,0xffff);
	glcd_string("Diego", 24, 300, 1,0xffff);

    while (1) 
    {
		glcd_string("NoTetris",16,10, 1,0xffff);
		sprintf(strPuntaje, "Puntaje:");
		glcd_string(strPuntaje, 10, 40, 1,0xffff);
		sprintf(strPuntaje, "%d",score);
		glcd_string(strPuntaje, 10, 50, 1,0xffff);
		dibujarTablero();
		dibujarCayendo();
		
		if(actual == NULL){
			agregarPieza();
		}
    }
}

