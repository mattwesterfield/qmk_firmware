/* Copyright 2017 Zach White <skullydazed@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Portions of this file are based on LEDControl by Eberhard Fahle. You can
 * find the original code here: <https://github.com/wayoda/LedControl>
 *
 * The license for that code follows.
 *
 *    LedControl.h - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    This permission notice shall be included in all copies or
 *    substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */
#include "spi_master.h"
#include "2021.h"

// Configure our MAX7219's
#define MAX7219_DATA B2
#define MAX7219_CLK B1
#define MAX7219_LOAD B0
#define MAX7219_DEVICES 2 // Rename this to MAX7219_CONTROLLERS
#define MAX_BYTES MAX7219_DEVICES * 2

// Opcodes for the MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

// Other stuff
#define OPERATION_MODE 0x0C
#define SHUTDOWN_MODE 0x0C00
#define NORMAL_OPERATION 0x0C01

#define BRIGHTNESS 0x0A
#define BRIGHTNESS_1 0x0A00
#define BRIGHTNESS_2 0x0A01
#define BRIGHTNESS_3 0x0A02
#define BRIGHTNESS_4 0x0A03
#define BRIGHTNESS_5 0x0A04
#define BRIGHTNESS_6 0x0A05
#define BRIGHTNESS_7 0x0A06
#define BRIGHTNESS_8 0x0A07
#define BRIGHTNESS_9 0x0A08
#define BRIGHTNESS_10 0x0A09
#define BRIGHTNESS_11 0x0A0A
#define BRIGHTNESS_12 0x0A0B
#define BRIGHTNESS_13 0x0A0C
#define BRIGHTNESS_14 0x0A0D
#define BRIGHTNESS_15 0x0A0E
#define BRIGHTNESS_16 0x0A0F

#define SCAN_LIMIT 0x0B
#define SCAN_LIMIT_ALL 0x0B07

#define ROW_0 0x01
#define ROW_1 0x02
#define ROW_2 0x03
#define ROW_3 0x04
#define ROW_4 0x05
#define ROW_5 0x06
#define ROW_6 0x07
#define ROW_7 0x08

uint8_t buffer[2];
uint8_t status[64];
uint8_t spidata[16];  // fixme: rename to spi_data?


void shiftOut(uint8_t val) {
      uint8_t i;

      for (i = 0; i < 8; i++)  {
            writePin(MAX7219_DATA, !!(val & (1 << (7 - i))));
            writePinHigh(MAX7219_CLK);
            writePinLow(MAX7219_CLK);
      }
}

void spiTransfer(int addr, volatile uint8_t opcode, volatile uint8_t data) { // FIXME: Rename to max_spi_transfer or max_write or something
	// Create an array with the data to shift out
	int offset=addr*2;

	for (int i=0; i<MAX_BYTES; i++) {
		spidata[i]=0x00;
	}
	spidata[offset+1]=opcode;
	spidata[offset]=data;

	// Bitbang the data
	writePinLow(MAX7219_LOAD);
	for(int i=MAX_BYTES; i>0; i--) {
		shiftOut(spidata[i-1]);
	}
	writePinHigh(MAX7219_LOAD);
	
/* using the lufa spi support
	// Send the data
	spi_status_t status=spi_transmit(spidata, MAX_BYTES);
        if (status != SPI_STATUS_SUCCESS) {
                xprintf("Error sending data to max7219: %d\n", status);
        }
*/


}

void clearDisplay(int addr) {
    if (addr<0 || addr>=MAX7219_DEVICES) {
        return;
    }

    int offset=addr*8;
    for(int i=0; i<8; i++) {
        status[offset+i]=0;
        spiTransfer(addr, i+1, status[offset+i]);
    }
}

void max_set_intensity(int addr, int intensity) {
    if (addr<0 || addr>=MAX7219_DEVICES) {
        return;
    }

    if (intensity>=0 && intensity<16) {
	    spiTransfer(addr, OP_INTENSITY, intensity);
    }
}

void max_shutdown(int addr, bool is_in_shutdown) {
    if (addr<0 || addr>=MAX7219_DEVICES) {
        return;
    }

    if (is_in_shutdown) {
        spiTransfer(addr, OP_SHUTDOWN, 0);
    } else {
        spiTransfer(addr, OP_SHUTDOWN, 1);
    }
}

void set_led(int addr, int row, int column, bool state);
void set_led(int addr, int row, int column, bool state) {
    int offset;
    uint8_t val=0x00;

    if (addr<0 || addr>=MAX7219_DEVICES) {
        return;
    }

    if (row<0 || row>7 || column<0 || column>7) {
        return;
    }

    offset=addr*8;
    val=0b10000000 >> column;

    if (state) {
        status[offset+row]=status[offset+row]|val;
    } else {
        val=~val;
        status[offset+row]=status[offset+row]&val;
    }
    spiTransfer(addr, row+1, status[offset+row]);
}

void matrix_init_kb(void) {
    wait_ms(5000);
    xprintf("matrix_init_kb()\n");


    setPinOutput(MAX7219_DATA);
    setPinOutput(MAX7219_CLK);
    setPinOutput(MAX7219_LOAD);
    writePinHigh(MAX7219_LOAD);
/*
    spi_init();
    if (! spi_start(B0, false, 0, 8)) {
	    xprintf("spi_start failed!");
    }
*/

/*
    uint8_t led_status[32];
    for(int i=0;i<32;i++) {
        led_status[i]=0x00;
    }
*/

    for (int i=0; i<MAX7219_DEVICES; i++) {
	// Init from LedControl constructor
	spiTransfer(i, OP_DISPLAYTEST, 0);
	spiTransfer(i, OP_SCANLIMIT, 7);
	spiTransfer(i, OP_DECODEMODE, 0);
        clearDisplay(i);
	max_shutdown(i, true);

	// Init from arduino setup()
	max_shutdown(i, false);
	max_set_intensity(i, 8);
	clearDisplay(i);
    }

    while (true) {
        xprintf("Turning on LED1\n");
        //set_led(0, 0, 0, true);
	spiTransfer(0, OP_DISPLAYTEST, 1);
        wait_ms(1000);
        xprintf("Turning off LED1\n");
	spiTransfer(0, OP_DISPLAYTEST, 0);
        xprintf("Turning on LED2\n");
	spiTransfer(1, OP_DISPLAYTEST, 1);
        //set_led(0, 0, 0, false);
        wait_ms(1000);
        xprintf("Turning off LED2\n");
	spiTransfer(1, OP_DISPLAYTEST, 0);
    }
}

#if 0
// First attempt
void matrix_init_kb(void) {
    wait_ms(5000);
    xprintf("matrix_init_kb()\n");

    spi_status_t status;
    //uint8_t led_status[32];

    //for(int i=0;i<32;i++) led_status[i]=0x00;

    // Initialize the LED driver
    spi_init();
    if (! spi_start(B0, false, 0, 8)) {
	    xprintf("spi_start failed!");
    }
    buffer[0] = SCAN_LIMIT;
    buffer[1] = 0x07; // Enable all rows
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status0 error: %d\n", status);
    buffer[0] = BRIGHTNESS;
    buffer[1] = 0x07; // Med-low brightness
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status1 error: %d\n", status);

    // Blank all the LEDs
    buffer[1] = 0x00;
    buffer[0] = ROW_0;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status2 error: %d\n", status);
    buffer[0] = ROW_1;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status3 error: %d\n", status);
    buffer[0] = ROW_2;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status4 error: %d\n", status);
    buffer[0] = ROW_3;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status5 error: %d\n", status);
    buffer[0] = ROW_4;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status6 error: %d\n", status);
    buffer[0] = ROW_5;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status7 error: %d\n", status);
    buffer[0] = ROW_6;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status8 error: %d\n", status);
    buffer[0] = ROW_7;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status9 error: %d\n", status);

    // Enable the display
    buffer[0] = OPERATION_MODE;
    buffer[1] = 0x01;
    status = spi_transmit(buffer, 2);
    if (status < 0) xprintf("status10 error: %d\n", status);

    // Blink the first row
    buffer[0] = ROW_0;

    while (true) {
        xprintf("Turning on LED\n");
        buffer[1] = 0xFF;
        status = spi_transmit(buffer, 2);
        if (status < 0) xprintf("status11 error: %d\n", status);
        wait_ms(1000);
        xprintf("Turning off LED\n");
        buffer[1] = 0x00;
        status = spi_transmit(buffer, 2);
        if (status < 0) xprintf("status12 error: %d\n", status);
        wait_ms(1000);
    }
}
#endif

__attribute__ ((weak))
bool encoder_update_keymap(int8_t index, bool clockwise) {
    return false;
}

void encoder_update_kb(int8_t index, bool clockwise) {
    xprintf("Encoder spin\n");
    if (!encoder_update_keymap(index, clockwise)) {
        // Encoder 1, left
        if (index == 0 && clockwise) {
            tap_code(KC_MS_U);  // turned right
        } else if (index == 0) {
            tap_code(KC_MS_D);  // turned left
        }

        // Encoder 2, right
        else if (index == 1 && clockwise) {
            tap_code(KC_MS_R);  // turned right
        } else if (index == 1) {
            tap_code(KC_MS_L);  // turned left
        }
    }
}
