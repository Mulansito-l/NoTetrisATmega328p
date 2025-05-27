/*
 * GLCD.h
 *
 * Created: 03/05/2025 10:46:58 p. m.
 * Author: Mulansit
 */

#ifndef GLCD_H_
#define GLCD_H_

#define TEST_MODE 0 // Start with 0

#define F_CPU 8000000 // Has to be here (not in .h or program.c) otherwise fucked up things happen

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h> // abs()
#include <string.h> // strlen()
#include <math.h>   // For sin, cos in glcd_arc
#include "SPI.h"
#include "Font.h" // Asegúrate de que este archivo contiene la definición de simpleFont
#include "ILI9341CMD.h" // Ensure this file defines the correct hexadecimal values for ILI9341 commands

#define MAX_X_PORTRAIT  240
#define MAX_Y_PORTRAIT  320

#define FONT_SPACE      6
#define FONT_X          8
#define FONT_Y          8

// --- Define common colors (example, ensure these are in ILI9341CMD.h or similar) ---
#define black   0x0000
#define red     0xF800
#define green   0x07E0
#define blue    0x001F
#define white   0xFFFF
#define yellow  0xFFE0
#define cyan    0x07FF
#define magenta 0xF81F

// No necesitamos max_x, max_y variables si siempre es portrait.
// Usaremos las macros directamente.
// unsigned int max_x, max_y; // Eliminado
unsigned int bgcolor = black; // Default background color

// CS = Chip Select (PB0)
void glcd_cs_low() {
    DDRB |= (1 << PB0);    // PB0 como salida
    PORTB &= ~(1 << PB0);  // PB0 en bajo
}
void glcd_cs_high() {
    DDRB |= (1 << PB0);
    PORTB |= (1 << PB0);
}

// DC = Data/Command (PB2)
void glcd_dc_low() {
    DDRB |= (1 << PB2);    // PB2 como salida
    PORTB &= ~(1 << PB2);  // PB2 en bajo
}
void glcd_dc_high() {
    DDRB |= (1 << PB2);
    PORTB |= (1 << PB2);
}

// LED = Retroiluminación (PD7)
void glcd_led_off() {
    DDRD |= (1 << PD7);    // PD7 como salida
    PORTD &= ~(1 << PD7);  // PD7 en bajo
}
void glcd_led_on() {
    DDRD |= (1 << PD7);
    PORTD |= (1 << PD7);
}

// RST = Reset (PB1)
void glcd_rst_off() {
    DDRB |= (1 << PB1);    // PB1 como salida
    PORTB |= (1 << PB1);  // Reset desactivado (alto)
}
void glcd_rst_on() {
    DDRB |= (1 << PB1);
    PORTB &= ~(1 << PB1); // Reset activado (bajo)
}

// Declaración de la fuente (debe estar definida en Font.h)
extern const unsigned char simpleFont[][8]; // Usa 'extern' si la definición está en otro .c

// Ya no necesitamos glcd_orientation; siempre es PORTRAIT
// unsigned char glcd_orientation; // Eliminado

void glcd_sendCmd(unsigned char data);
void glcd_sendData(unsigned char data);
void glcd_sendData16(unsigned int data);
void glcd_init(void);
void glcd_setX(unsigned int x0,unsigned int x1);
void glcd_setY(unsigned int y0,unsigned int y1);
// void glcd_setOrientation(char orientation); // Eliminado
void glcd_setXY(unsigned int x0, unsigned int y0);
void glcd_line(unsigned int x0,unsigned int y0,unsigned int x1, unsigned int y1, unsigned int color);
void glcd_hline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color);
void glcd_vline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color);
void glcd_rectangle(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width, unsigned int color);
void glcd_fillRectangle(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width, unsigned int color);
void glcd_circle(int x0, int y0, int r, unsigned int color);
void glcd_fillCircle(int x0, int y0, int r, unsigned int color);
void glcd_char(unsigned char ascii, unsigned int x0, unsigned int y0,unsigned int size, unsigned int fgcolor);
void glcd_string(char *argstring, unsigned int x0, unsigned int y0, unsigned int size,unsigned int fgcolor);
unsigned char glcd_number(unsigned int long_num,unsigned int x0, unsigned int y0,unsigned int size,unsigned int fgcolor);
unsigned char glcd_float(float floatNumber, unsigned int x0, unsigned int y0, unsigned int size, unsigned int fgcolor);
void glcd_kocka(unsigned int x0, unsigned int y0, unsigned int howmany, unsigned int size, unsigned int fgcolor);
void glcd_clr(unsigned int x0, unsigned int y0, unsigned int size);
void glcd_clrLine(unsigned int y0, unsigned int size);
void glcd_pixel(unsigned int x0, unsigned int y0, unsigned int color);
void glcd_bar(unsigned int x0, unsigned int y0, unsigned int howmany, unsigned int bar_max, unsigned int size, unsigned int fgcolor, unsigned int rectangle_color);
void glcd_eq(char *string, unsigned int x0, unsigned int y0, float howmany, unsigned int size, unsigned int fgcolor, unsigned int rectangle_color);
void glcd_bg(unsigned int color);
void glcd_arc(int x, int y, int r, int startAngle, int endAngle, int thickness, unsigned int color);
void glcd_title(char *string, unsigned int size, unsigned int fgcolor, unsigned int bgcolor);

void glcd_sendCmd(unsigned char data)
{
    glcd_dc_low();
    glcd_cs_low();
    SPI_MasterTransmit(data);
    glcd_cs_high();
}

void glcd_sendData(unsigned char data)
{
    glcd_dc_high();
    glcd_cs_low();
    SPI_MasterTransmit(data);
    glcd_cs_high();
}

void glcd_sendData16(unsigned int data)
{
    unsigned char data1 = data >> 8;
    unsigned char data2 = data & 0xff;
    glcd_dc_high();
    glcd_cs_low();
    SPI_MasterTransmit(data1);
    SPI_MasterTransmit(data2);
    glcd_cs_high();
}

void glcd_init(void)
{
    SPI_MasterInit();
    glcd_cs_high();
    glcd_dc_high();

    glcd_rst_on();       // Reset pin low
    _delay_ms(10);
    glcd_rst_off();      // Release reset (pin high)
    _delay_ms(120);      // Wait for reset to complete

    // --- Standard ILI9341 Initialization Sequence (common and reliable) ---

    // Software Reset
    glcd_sendCmd(ILI9341_SWRESET);
    _delay_ms(150); // Give enough time for the reset to complete

    // Sleep Out
    glcd_sendCmd(ILI9341_SLPOUT);
    _delay_ms(150); // Wait for oscillator to stabilize

    // Power Control 1 (Vcore, VRH)
    glcd_sendCmd(ILI9341_PWCTR1);
    glcd_sendData(0x23); // GVDD = 4.60V (common)

    // Power Control 2 (VCOM)
    glcd_sendCmd(ILI9341_PWCTR2);
    glcd_sendData(0x10); // VCOM = 1.00V (common)

    // VCOM Control 1 (VCOM offset)
    glcd_sendCmd(ILI9341_VMCTR1);
    glcd_sendData(0x3E); // VCOMH
    glcd_sendData(0x28); // VCOML (common for many modules)

    // VCOM Control 2
    glcd_sendCmd(ILI9341_VMCTR2);
    glcd_sendData(0x86); // Disable write read cmd to enable adjustment of VCM (common)

    // Memory Access Control (MADCTL)
    // ESTE ES EL CRÍTICO para la orientación.
    // 0x48: MY=0, MX=1, MV=0, ML=0, BGR=1, MH=0
    // MV=0 significa que no hay intercambio de filas/columnas.
    // MX=1 invierte la dirección de X (si tu pantalla la necesita para empezar de izquierda a derecha).
    // BGR=1 para filtro de color BGR.
    glcd_sendCmd(ILI9341_MADCTL);
    glcd_sendData(0x48); // Configuración estándar para PORTRAIT (240x320)

    // Pixel Format Set
    glcd_sendCmd(ILI9341_COLMOD);
    glcd_sendData(0x55); // CRITICAL FIX: 16-bit color (RGB565)

    // Frame Rate Control (In Normal Mode/Full Colors)
    glcd_sendCmd(ILI9341_FRMCTR1);
    glcd_sendData(0x00); // Fosc / (1 x 2 + 0x00) = 60Hz (common)
    glcd_sendData(0x18); // 18 frames/sec

    // Display Function Control
    glcd_sendCmd(ILI9341_DFUNCTR);
    glcd_sendData(0x08); // Common for many modules (standard values)
    glcd_sendData(0x82);
    glcd_sendData(0x27);

    // Enable 3 Gamma Control
    glcd_sendCmd(ILI9341_3GAMMA_EN); // Assuming ILI9341_3GAMMA_EN is 0xF2
    glcd_sendData(0x00);

    // Gamma Set
    glcd_sendCmd(ILI9341_GAMSET);
    glcd_sendData(0x01); // Gamma curve 1

    // Positive Gamma Correction (These are typical values, adjust if needed for color accuracy)
    glcd_sendCmd(ILI9341_GMCTRP1);
    glcd_sendData(0x0F); glcd_sendData(0x31); glcd_sendData(0x2B); glcd_sendData(0x0C);
    glcd_sendData(0x0E); glcd_sendData(0x08); glcd_sendData(0x4E); glcd_sendData(0xF1);
    glcd_sendData(0x37); glcd_sendData(0x07); glcd_sendData(0x10); glcd_sendData(0x03);
    glcd_sendData(0x0E); glcd_sendData(0x09); glcd_sendData(0x00);

    // Negative Gamma Correction (These are typical values, adjust if needed for color accuracy)
    glcd_sendCmd(ILI9341_GMCTRN1);
    glcd_sendData(0x00); glcd_sendData(0x0E); glcd_sendData(0x14); glcd_sendData(0x03);
    glcd_sendData(0x11); glcd_sendData(0x07); glcd_sendData(0x31); glcd_sendData(0xC1);
    glcd_sendData(0x48); glcd_sendData(0x08); glcd_sendData(0x0F); glcd_sendData(0x0C);
    glcd_sendData(0x31); glcd_sendData(0x36); glcd_sendData(0x0F);

    // Exit Sleep Mode
    glcd_sendCmd(ILI9341_SLPOUT);
    _delay_ms(120);

    // Display ON
    glcd_sendCmd(ILI9341_DISPON);
    _delay_ms(20); // Short delay after display on

    // Ready for drawing
    glcd_sendCmd(ILI9341_RAMWR); // Prepare for first pixel write (Memory Write)
    glcd_bg(bgcolor); // Fill background
}

void glcd_setX(unsigned int x0,unsigned int x1)
{
    glcd_sendCmd(ILI9341_CASET); // Column Address Set
    glcd_sendData16(x0);
    glcd_sendData16(x1);
}

void glcd_setY(unsigned int y0,unsigned int y1)
{
    glcd_sendCmd(ILI9341_PASET); // Page Address Set
    glcd_sendData16(y0);
    glcd_sendData16(y1);
}

void glcd_setXY(unsigned int x0, unsigned int y0)
{
    // Solo lógica para PORTRAIT
    glcd_setX(x0, x0);
    glcd_setY(y0, y0);
    glcd_sendCmd(ILI9341_RAMWR); // Memory Write (prepare for pixel data)
}

void glcd_pixel(unsigned int x0, unsigned int y0, unsigned int color)
{
    if(x0 >= MAX_X_PORTRAIT || y0 >= MAX_Y_PORTRAIT) return; // Basic bounds check

    glcd_setX(x0, x0);
    glcd_setY(y0, y0);
    glcd_sendCmd(ILI9341_RAMWR); // Memory Write (prepare for pixel data)
    glcd_sendData16(color);
}

void glcd_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color)
{
    int dx = abs((int)x1 - (int)x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs((int)y1 - (int)y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;  // error value e_xy
    int e2;

    for (;;) {
        glcd_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { // e_xy+e_x > 0
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) { // e_xy+e_y < 0
            err += dx;
            y0 += sy;
        }
    }
}

void glcd_hline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color)
{
    if (x0 + length > MAX_X_PORTRAIT) length = MAX_X_PORTRAIT - x0; // Clamp length
    if (y0 >= MAX_Y_PORTRAIT) return; // Clamp y

    glcd_setX(x0, x0 + length - 1); // x1 should be x0 + length - 1
    glcd_setY(y0, y0);
    glcd_sendCmd(ILI9341_RAMWR); // Memory Write
    for(unsigned int i=0; i<length; i++)
        glcd_sendData16(color);
}

void glcd_vline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color)
{
    if (y0 + length > MAX_Y_PORTRAIT) length = MAX_Y_PORTRAIT - y0; // Clamp length
    if (x0 >= MAX_X_PORTRAIT) return; // Clamp x

    glcd_setX(x0, x0);
    glcd_setY(y0, y0 + length - 1); // y1 should be y0 + length - 1
    glcd_sendCmd(ILI9341_RAMWR); // Memory Write
    for(unsigned int i=0; i<length; i++)
        glcd_sendData16(color);
}

void glcd_rectangle(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width, unsigned int color)
{
    glcd_hline(x0, y0, length, color);
    glcd_hline(x0, y0 + width, length, color);
    glcd_vline(x0, y0, width, color);
    glcd_vline(x0 + length, y0, width, color);
}

void glcd_fillRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int color) {
	// Basic bounds checking
	if (x >= MAX_X_PORTRAIT || y >= MAX_Y_PORTRAIT) return;
	if (x + width > MAX_X_PORTRAIT) width = MAX_X_PORTRAIT - x;
	if (y + height > MAX_Y_PORTRAIT) height = MAX_Y_PORTRAIT - y;
	if (width == 0 || height == 0) return;

	glcd_setX(x, x + width - 1);
	glcd_setY(y, y + height - 1);
	glcd_sendCmd(ILI9341_RAMWR); // Prepare to write to GRAM

	// Total pixels in the rectangle
	unsigned long total_pixels = (unsigned long)width * height;

	// Send the color data for all pixels
	for (unsigned long i = 0; i < total_pixels; i++) {
		glcd_sendData16(color);
	}
}

void glcd_circle(int x0, int y0, int r, unsigned int color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do
    {
        glcd_pixel(x0-x, y0+y,color);
        glcd_pixel(x0+x, y0+y,color);
        glcd_pixel(x0+x, y0-y,color);
        glcd_pixel(x0-x, y0-y,color);
        e2 = err;
        if (e2 <= y)
        {
            err += ++y*2+1;
            if (-x == y && e2 <= x)
            e2 = 0;
        }
        if (e2 > x)
        err += ++x*2+1;
    } while (x <= 0);
}

void glcd_fillCircle(int x0, int y0, int r, unsigned int color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do
    {
        glcd_vline(x0-x, y0-y, 2*y + 1, color); // Corrected length to 2*y + 1
        glcd_vline(x0+x, y0-y, 2*y + 1, color); // Corrected length to 2*y + 1

        e2 = err;
        if (e2 <= y)
        {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x)
        err += ++x*2+1;
    } while (x <= 0);
}

void glcd_char(unsigned char ascii, unsigned int x0, unsigned int y0, unsigned int size, unsigned int fgcolor)
{

	if (x0 >= MAX_X_PORTRAIT || y0 >= MAX_Y_PORTRAIT) return;

	if (ascii < 32 || ascii > 127) ascii = '?';
	unsigned char char_index = ascii - 32;

	unsigned int is_pixel_on;
	unsigned int color_to_draw;
	unsigned char font_byte;

	for (int font_col_idx = 0; font_col_idx < FONT_X; font_col_idx++) {
		font_byte = pgm_read_byte(&simpleFont[char_index][font_col_idx]);
		for (int font_row_idx = 0; font_row_idx < FONT_Y; font_row_idx++) {
			is_pixel_on = (font_byte >> font_row_idx) & 0x01; // Bit 0 is TOP
			color_to_draw = is_pixel_on ? fgcolor : bgcolor;
			glcd_fillRectangle(x0 + (font_col_idx * size),
			y0 + (font_row_idx * size),
			size, size, color_to_draw);
		}
	}
}


void glcd_string(char *string, unsigned int x0, unsigned int y0, unsigned int size, unsigned int fgcolor)
{
    while(*string)
    {
        glcd_char(*string, x0, y0, size, fgcolor);
        string++;

        // Calculate next character position, considering MAX_X_PORTRAIT
        if (x0 + FONT_SPACE * size < MAX_X_PORTRAIT) {
            x0 += FONT_SPACE * size;
        } else {
            // Option: Wrap to next line, or stop. For now, stop.
            break;
        }
    }
}

unsigned char glcd_number(unsigned int long_num,unsigned int x0, unsigned int y0,unsigned int size,unsigned int fgcolor)
{
    unsigned char char_buffer[11]; // Max 10 digits for uint32_t + null terminator
    unsigned char i = 0;
    unsigned char f = 0; // f will count characters printed

    if (long_num == 0) {
        glcd_char('0', x0, y0, size, fgcolor);
        return 1; // Only one character '0'
    }

    // Convert number to string in reverse order
    while (long_num > 0) {
        char_buffer[i++] = (long_num % 10) + '0'; // Convert digit to ASCII char
        long_num /= 10;
    }
    char_buffer[i] = '\0'; // Null-terminate the string

    // Print characters in correct order (reverse of how they were stored)
    while (i > 0) {
        glcd_char(char_buffer[--i], x0, y0, size, fgcolor);
        f++;
        if (x0 + FONT_SPACE * size < MAX_X_PORTRAIT) {
            x0 += FONT_SPACE * size;
        } else {
            break; // Stop if exceeding display width
        }
    }
    return f;
}

unsigned char glcd_float(float floatNumber, unsigned int x0, unsigned int y0, unsigned int size, unsigned int fgcolor)
{
    unsigned char decimal_places = 2; // Fixed to 2 decimal places for simplicity
    unsigned int temp_int;
    float frac_part;
    float rounding = 0.5;
    unsigned char chars_printed = 0;

    // Handle negative numbers
    if (floatNumber < 0.0) {
        glcd_char('-', x0, y0, size, fgcolor);
        x0 += FONT_SPACE * size;
        chars_printed++;
        floatNumber = -floatNumber;
    }

    // Add rounding for the specified decimal places
    for (unsigned char i = 0; i < decimal_places; ++i) {
        rounding /= 10.0;
    }
    floatNumber += rounding;

    // Print integer part
    temp_int = (unsigned int)floatNumber;
    chars_printed += glcd_number(temp_int, x0, y0, size, fgcolor);
    x0 += (chars_printed - (floatNumber < 0 ? 1 : 0)) * FONT_SPACE * size; // Adjust x0 based on integer part length

    // Print decimal point
    if (decimal_places > 0) {
        glcd_char('.', x0, y0, size, fgcolor);
        x0 += FONT_SPACE * size;
        chars_printed++;
    }

    // Print fractional part
    frac_part = floatNumber - temp_int;
    for (unsigned char i = 0; i < decimal_places; i++) {
        frac_part *= 10;
        temp_int = (unsigned int)frac_part;
        glcd_char((temp_int % 10) + '0', x0, y0, size, fgcolor); // Print digit and move x0
        x0 += FONT_SPACE * size;
        chars_printed++;
        frac_part -= temp_int; // Remove the printed digit
    }
    return chars_printed;
}


void glcd_kocka(unsigned int x0, unsigned int y0, unsigned int howmany, unsigned int size, unsigned int fgcolor)
{
    // The "kocka" is ASCII 96, which is '`' (backtick).
    // Assuming simpleFont[96] is a filled block character.
    for(unsigned int j=0; j<howmany; j++)
    {
        glcd_char('`', x0, y0, size, fgcolor); // Use glcd_char for the block
        if(x0 + FONT_SPACE * size < MAX_X_PORTRAIT) {
            x0 += FONT_SPACE * size;
        } else {
            break;
        }
    }
}

void glcd_clr(unsigned int x0, unsigned int y0, unsigned int size)
{
    // Clears a rectangle of size approx 10x10 characters (assuming FONT_SPACE*size is char width)
    glcd_fillRectangle(x0, y0, FONT_SPACE * size * 10, FONT_Y * size * 10, bgcolor);
}

void glcd_clrLine(unsigned int y0, unsigned int size)
{
    // Clears a line (approx. 10 character heights in total) at a given y0
    // Adjusting to clear from y0 to y0 + FONT_Y * size, for one full line height.
    glcd_fillRectangle(0, y0, MAX_X_PORTRAIT, FONT_Y * size, bgcolor);
}

void glcd_bar(unsigned int x0, unsigned int y0, unsigned int howmany, unsigned int bar_max, unsigned int size, unsigned int fgcolor, unsigned int rectangle_color)
{
    unsigned int bar_width = FONT_SPACE * size * bar_max;
    unsigned int bar_height = FONT_Y * size; // Character height

    glcd_rectangle(x0, y0, bar_width, bar_height, rectangle_color); // Draw outer rectangle

    // Draw the "howmany" blocks
    unsigned int current_x = x0 + size; // Start slightly inside the rectangle
    for(unsigned int i = 0; i < howmany; i++) {
        glcd_kocka(current_x, y0, 1, size, fgcolor); // Draw one block
        current_x += FONT_SPACE * size; // Move to next position
        if (current_x >= x0 + bar_width) break; // Don't draw outside the bar
    }
}

void glcd_eq(char *string, unsigned int x0, unsigned int y0, float howmany, unsigned int size, unsigned int fgcolor, unsigned int rectangle_color)
{
    glcd_string(string,x0,y0,size,fgcolor);

    unsigned int string_pixel_width = strlen(string) * FONT_SPACE * size;
    unsigned int bar_max_val = 10; // Fixed max for the bar, can be a parameter
    unsigned int bar_start_x = x0 + string_pixel_width + FONT_SPACE * size; // Shift right after string

    glcd_bar(bar_start_x, y0, (unsigned int)howmany, bar_max_val, size, fgcolor, rectangle_color);

    unsigned int bar_end_x = bar_start_x + (FONT_SPACE * size * bar_max_val) + FONT_SPACE * size; // End of bar + some space
    glcd_float(howmany, bar_end_x, y0, size, fgcolor);
}

void glcd_bg(unsigned int color)
{
    glcd_fillRectangle(0,0,MAX_X_PORTRAIT,MAX_Y_PORTRAIT,color);
}

void glcd_arc(int x, int y, int r, int startAngle, int endAngle, int thickness, unsigned int color)
{
    // Clamp angles to 0-360
    startAngle %= 360;
    endAngle %= 360;
    if (startAngle < 0) startAngle += 360;
    if (endAngle < 0) endAngle += 360;

    // Handle full circle or overlapping angles
    if (startAngle == endAngle && thickness > 0) {
        // Draw a full circle if angles are the same and thickness is positive
        glcd_fillCircle(x,y,r,color); // Best to use fillCircle for solid arcs, or loop for lines
        glcd_fillCircle(x,y,r-thickness,bgcolor); // Clear inner part
        return;
    }
    
    // Ensure endAngle is greater than startAngle for iteration
    if (endAngle < startAngle) {
        endAngle += 360;
    }

    // Iterate through radii for thickness
    for (int current_r = r - thickness / 2; current_r <= r + thickness / 2; current_r++) {
        if (current_r <= 0) continue; // Avoid drawing zero or negative radius circles

        int px = x + round(cos(startAngle * M_PI / 180.0) * current_r);
        int py = y + round(sin(startAngle * M_PI / 180.0) * current_r);

        for (int angle = startAngle + 1; angle <= endAngle; angle++) {
            int cx = x + round(cos(angle * M_PI / 180.0) * current_r);
            int cy = y + round(sin(angle * M_PI / 180.0) * current_r);
            glcd_line(px, py, cx, cy, color); // Draw line segment
            px = cx;
            py = cy;
        }
    }
}


void glcd_title(char *string, unsigned int size, unsigned int fgcolor, unsigned int bgcolor)
{
    unsigned int text_width = strlen(string) * size * FONT_SPACE;
    unsigned int title_bar_height = FONT_Y * size * 2; // Two lines high for background
    unsigned int y0_text = (title_bar_height - FONT_Y * size) / 2; // Center text vertically
    unsigned int x0_text = (MAX_X_PORTRAIT / 2) - (text_width / 2);

    glcd_fillRectangle(0, 0, MAX_X_PORTRAIT, title_bar_height, bgcolor); // Fill title bar background
    glcd_string(string, x0_text, y0_text, size, fgcolor); // Draw text
}

#endif /* GLCD_H_ */