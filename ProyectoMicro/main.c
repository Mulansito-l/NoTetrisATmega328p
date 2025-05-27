#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "Colors.h"
#include "GLCD.h"
#include "joystick.h"
#include "UART.h"
#include "EEPROM.h"

#define IZQUIERDA 0
#define DERECHA 1
#define ABAJO 2
#define ROTACION 3

Joystick joystick;


//Definiciones de las piezas como matrices 4x4
uint8_t piezaI[4][4] = {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}};
uint8_t piezaL[4][4] = {{0,0,0,0},{0,0,1,0},{0,0,1,0},{0,0,1,1}};
uint8_t piezaO[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,1,1},{0,0,1,1}};
uint8_t piezaJ[4][4] = {{0,0,0,0},{0,0,1,0},{0,0,1,0},{0,1,1,0}};
uint8_t piezaS[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,1,1},{0,1,1,0}};
uint8_t piezaZ[4][4] = {{0,0,0,0},{0,0,0,0},{0,1,1,0},{0,0,1,1}};
uint8_t piezaT[4][4] = {{0,0,0,0},{0,0,0,0},{0,1,1,1},{0,0,1,0}};
	
uint8_t puntajesTop[] = {0,0,0,0,0};
	
int dibujar = 0;
int score = 0;
int prev_actual_x, prev_actual_y;


// Representacion de una pieza
typedef struct s_Pieza{
	unsigned int color;
	uint8_t forma[4][4];
	int x,y;
} Pieza;

// Celda del tablero
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

void init_buzzer() {
	DDRD |= (1 << 7); //buzzer
		PORTD &= ~(1 << 7);
}

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

// Dibuja unicamente la pieza cayendo actualmente
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

// Se encarga de dibujar las celdas ocupadas en el tablero
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

// Comprueba colisiones al momento de mover

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
	} else if(direccion == ROTACION){
		rotarPieza(actual);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if((actual->x + i) >= 0 && actual->forma[j][i] == 1 && tablero.celdas[actual->x + i][actual->y + j].ocupada == 1){
					rotarPieza(actual);
					rotarPieza(actual);
					rotarPieza(actual);
					return 1;
				}
				
				if(actual->forma[j][i] == 1 && (actual->x + i) < 0){
					rotarPieza(actual);
					rotarPieza(actual);
					rotarPieza(actual);
					return 1;
				}
				
				if((actual->x + i) < 10 && actual->forma[j][i] == 1 && tablero.celdas[actual->x + i][actual->y + j].ocupada == 1){
					rotarPieza(actual);
					rotarPieza(actual);
					rotarPieza(actual);
					return 1;
				}
				
				if(actual->forma[j][i] == 1 && (actual->x + i) >= 10){
					rotarPieza(actual);
					rotarPieza(actual);
					rotarPieza(actual);
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
		case ROTACION:
			TCNT1 = 0;
			comprobarColisiones(ROTACION);
			break;
	}
}

int tiempo_caida = 1500;

void init_timer(){
	TCNT1 = 0;
	TCCR1B |= (1<<CS12)|(0<<CS11)|(1<<CS10);
	TCCR1B |= (1<<WGM12);
	OCR1A = (tiempo_caida / 1000.0f) / (1.0f/(F_CPU / 1024.0f));
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
void UART_print_number(int number) {
	char buffer[10];
	int i = 0;

	if (number == 0) {
		UART_write('0');
		return;
	}

	if (number < 0) {
		UART_write('-');
		number = -number;
	}

	while (number > 0) {
		buffer[i++] = (number % 10) + '0';
		number /= 10;
	}

	while (i > 0) {
		UART_write(buffer[--i]);
	}
}
void rotarPieza(Pieza* p) {
	uint8_t temp[4][4];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp[i][j] = p->forma[3 - j][i];
		}
	}

	memcpy(p->forma, temp, 16);
}

int eliminarFilasCompletas() {
	int completas = 0;
	
	for (int j = 23; j >= 4; j--) {
		int completa = 1;

		for (int i = 0; i < 10; i++) {
			if (!tablero.celdas[i][j].ocupada) {
				completa = 0;
				break;
			}
		}

		if (completa) {
			completas++;
			// Mover todas las filas de arriba una hacia abajo
			for (int y = j; y > 0; y--) {
				for (int x = 0; x < 10; x++) {
					tablero.celdas[x][y] = tablero.celdas[x][y - 1];
				}
			}
			// Limpiar la fila superior (y = 0)
			for (int x = 0; x < 10; x++) {
				tablero.celdas[x][0].ocupada = 0;
				tablero.celdas[x][0].color = 0xf7ba; // color fondo
			}
			j++;
		}
	}
	return completas;
}

int verificarPerdida() {
	// Revisa las primeras filas (por ejemplo, las filas 0 a 3 del tablero)
	for (int j = 0; j <= 3; j++) {
		for (int i = 0; i < 10; i++) {
			if (tablero.celdas[i][j].ocupada) {
				return 1;  // Ya hay bloque en esa posición, se pierde
			}
		}
	}
	return 0; // No se perdió todavía
}


int main(void)
{
	int x, y, z;
	
	for (int i = 0; i < 5; i++)
	{
		//EEPROM_write(i,0);
		puntajesTop[i] = EEPROM_read(i);
	}
	
	Joystick js;
	Joystick_init(&js, 0, 1, 2);
	
	inicializarTablero();
	char strPuntaje[30];
	
	glcd_init();
	glcd_bg(0xf7ba);
	
	glcd_string("PRESIONA EL", 20, 50, 3, 0xffff);
	glcd_string("STICK PARA", 30, 80, 3, 0xffff);
	glcd_string("EMPEZAR", 60, 110, 3, 0xffff);
	while(Joystick_readZ(&js) == 0){
	}
	
	// Area de interfaz 80x320, Area de juego 160x320
	// tablero debe tener un tamaño de 10 de ancho y 20 de alto
	// cada cuadro del tablero será de 16x16 pixeles
	glcd_fillRectangle(0,0,80,320,0x8bf7);
	glcd_rectangle(0,0,79,319,0xf7ba);
	
	init_timer();
	iniciarADC();
	
	srand(generarSemilla());
	
	init_buzzer();
	
	glcd_string("Hecho por", 12, 270, 1,0xffff);
	glcd_string("Andrea", 23, 280, 1,0xffff);
	glcd_string("y", 36, 290, 1,0xffff);
	glcd_string("Diego", 24, 300, 1,0xffff);

	while (1)
	{
		if(verificarPerdida() == 0){
			PORTD &= ~(1 << 7);
			x = Joystick_readX(&js);
			y = Joystick_readY(&js);
			z = Joystick_readZ(&js);
			if (z == 1) {
				mover(ROTACION);
			}

			if (y < 400){
				if (actual != NULL) {
					prev_actual_x = actual->x;
					prev_actual_y = actual->y;
				}
				mover(IZQUIERDA);
			}else if (y > 600){
				if (actual != NULL) {
					prev_actual_x = actual->x;
					prev_actual_y = actual->y;
				}
				mover(DERECHA);
			}
			if (x < 400){
				if (actual != NULL) {
					prev_actual_x = actual->x;
					prev_actual_y = actual->y;
				}
				mover(ABAJO);
			}

			_delay_ms(150);

			glcd_string("NoTetris",16,10, 1,0xffff);
			sprintf(strPuntaje, "Puntaje:");
			glcd_string(strPuntaje, 10, 40, 1,0xffff);
			sprintf(strPuntaje, "%d",score);
			glcd_string(strPuntaje, 10, 50, 1,0xffff);
			
			
			glcd_string("Puntajes top", 2, 60, 1, 0xffff);
			sprintf(strPuntaje, "Top 1: %d",puntajesTop[0]);
			glcd_string(strPuntaje, 10, 70, 1,0xffff);
			sprintf(strPuntaje, "Top 2: %d",puntajesTop[1]);
			glcd_string(strPuntaje, 10, 80, 1,0xffff);
			sprintf(strPuntaje, "Top 3: %d",puntajesTop[2]);
			glcd_string(strPuntaje, 10, 90, 1,0xffff);
			sprintf(strPuntaje, "Top 4: %d",puntajesTop[3]);
			glcd_string(strPuntaje, 10, 100, 1,0xffff);
			sprintf(strPuntaje, "Top 5: %d",puntajesTop[4]);
			glcd_string(strPuntaje, 10, 110, 1,0xffff);
			
			dibujarTablero();
			dibujarCayendo();
			
			if(actual == NULL){
				agregarPieza();
			}
			
			int filasCompletas = eliminarFilasCompletas();
			
			if (filasCompletas > 0)
			{
				score += filasCompletas;
				PORTD |= (1 << 7);
				_delay_ms(100);
				PORTD &= ~(1 << 7);
			}
			
			if(verificarPerdida() == 1){
				
				int pos = -1;
				for (int i = 0; i < 5; i++)
				{
					if(score > puntajesTop[i] && pos == -1){
						pos = i;
					}
				}
				
				if(pos != - 1){
					for (int i = 4; i > pos; i--)
					{
						puntajesTop[i] = puntajesTop[i - 1];
						EEPROM_update(i, puntajesTop[i - 1]);
					}
					
					puntajesTop[pos] = score;
					EEPROM_update(pos, score);
				}
				
				glcd_bg(0xffff);
			}
			
		} else{
			z = Joystick_readZ(&js);
			glcd_string("PERDISTE!", 40, 50, 3, 0xffff);
			sprintf(strPuntaje, "Puntaje:");
			glcd_string(strPuntaje, 40, 80, 3,0xffff);
			sprintf(strPuntaje, "%d",score);
			glcd_string(strPuntaje, 40, 110, 3,0xffff);
			
			if (z == 1) {
				inicializarTablero();
				score = 0;
				glcd_bg(0xf7ba);
				// Area de interfaz 80x320, Area de juego 160x320
				// tablero debe tener un tamaño de 10 de ancho y 20 de alto
				// cada cuadro del tablero será de 16x16 pixeles
				glcd_fillRectangle(0,0,80,320,0x8bf7);
				glcd_rectangle(0,0,79,319,0x0000);
				
				glcd_string("Hecho por", 12, 270, 1,0xffff);
				glcd_string("Andrea", 23, 280, 1,0xffff);
				glcd_string("y", 36, 290, 1,0xffff);
				glcd_string("Diego", 24, 300, 1,0xffff);
			}
			PORTD |= (1 << 7);
		}
	}
}

