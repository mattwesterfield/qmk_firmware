/*
 * Copyright (c) 2021 Zach White <skullydazed@gmail.com>
 * Copyright (c) 2007 Eberhard Fahle
 *
 *    max7219.c - A library for controling Leds with a MAX7219/MAX7221
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

/*
 * This driver is a port of Arduino's LedControl to QMK. The original
 * Arduino code can be found here:
 *
 * https://github.com/wayoda/LedControl
 *
 * Since we are bitbanging the SPI interface you can use any pins
 * and it should work with any processors.
 */

#include "max7219.h"

/* Bitbang data by pulling the MAX7219_DATA pin high/low and pulsing the MAX7219_CLK pin between bits.
 */
void shift_out(uint8_t val) {
    uint8_t i;

    for (i=0; i < 8; i++)  {
        writePin(MAX7219_DATA, !!(val & (1 << (7 - i))));
        writePinHigh(MAX7219_CLK);
        writePinLow(MAX7219_CLK);
    }
}

/* Write data to a single max7219
 */
void max7219_write(int device_num, volatile uint8_t opcode, volatile uint8_t data) {
    // Create an array with the data to shift out
    uint8_t offset = device_num*2;
    uint8_t spidata[MAX_BYTES];

    spidata[offset+1] = opcode;
    spidata[offset] = data;

    // Bitbang the data
    writePinLow(MAX7219_LOAD);
    for(int i = MAX_BYTES; i>0; i--) {
        shift_out(spidata[i-1]);
    }
    writePinHigh(MAX7219_LOAD);
}

/* Turn off all the LEDs
 */
void max7219_clear_display(int device_num) {
    xprintf("max7219_clear_display(%d);\n", device_num);

    if (device_num<0 || device_num >= MAX7219_CONTROLLERS) {
        return;
    }

    int offset = device_num * 8;

    for(int i = 0; i<8; i++) {
        status[offset+i] = 0;
        max7219_write(device_num, i+1, status[offset+i]);
    }
}

/* Enable the display test (IE turn on all 64 LEDs)
 */
void max7219_display_test(int device_num, bool enabled) {
    xprintf("max7219_display_test(%d,  %d);\n", device_num, enabled);

    if (device_num<0 || device_num >= MAX7219_CONTROLLERS) {
        return;
    }

    max7219_write(device_num, OP_DISPLAYTEST, enabled);
}

/* Initialize the max7219 system and set the controller(s) to a default state.
 */
void max7219_init(void) {
    wait_ms(1500);
    xprintf("max7219_init()\n");

    setPinOutput(MAX7219_DATA);
    setPinOutput(MAX7219_CLK);
    setPinOutput(MAX7219_LOAD);
    writePinHigh(MAX7219_LOAD);

    for (int i=0; i<MAX7219_CONTROLLERS; i++) {
        max7219_shutdown(i, true);
    }

    for (int i=0; i<MAX7219_CONTROLLERS; i++) {
        // Reset everything to defaults and enable the display
        max7219_display_test(i, false);
        max7219_set_scan_limit(i, 7);
        max7219_set_decode_mode(i, 0);
        max7219_clear_display(i);
        max7219_set_intensity(i, 8);
        max7219_shutdown(i, false);
    }

#ifdef MAX7219_LED_TEST
    while (true) {
        for (int i=0; i<MAX7219_CONTROLLERS; i++) {
            max7219_display_test(i, true);
            wait_ms(500);
            max7219_display_test(i, false);
        }
    }
#endif
}

/* Set the decode mode of the controller. You probably don't want to change this.
 */
void max7219_set_decode_mode(int device_num, int mode) {
    xprintf("max7219_set_decode_mode(%d,  %d);\n", device_num, mode);

    if (device_num<0 || device_num >= MAX7219_CONTROLLERS) {
        return;
    }

    max7219_write(device_num, OP_DECODEMODE, mode);
}

/* Set the intensive (brightness) for the LEDs.
 */
void max7219_set_intensity(int device_num, int intensity) {
    xprintf("max7219_set_intensity(%d,  %d);\n", device_num, intensity);

    if (device_num<0 || device_num >= MAX7219_CONTROLLERS) {
        return;
    }

    if (intensity >= 0 && intensity<16) {
            max7219_write(device_num, OP_INTENSITY, intensity);
    }
}

/* Control a single LED.
 */
void max7219_set_led(int device_num, int row, int column, bool state) {
    xprintf("max7219_set_led(%d,  %d, %d, %d);\n", device_num, row, column, state);

    int offset;
    uint8_t val = 0x00;

    if (device_num<0 || device_num >= MAX7219_CONTROLLERS) {
        return;
    }

    if (row<0 || row>7 || column<0 || column>7) {
        return;
    }

    offset = device_num*8;
    val = 0b10000000 >> column;

    if (state) {
        status[offset+row] = status[offset+row]|val;
    } else {
        val = ~val;
        status[offset+row] = status[offset+row]&val;
    }
    max7219_write(device_num, row+1, status[offset+row]);
    /* You are looking at status, and how it seems to only be used for temporary LED state. You're wondering if you can use it for all LED state, or if you should just write your own based around a 2d array.
     */
}

/* Set a whole row of LEDs.
 */
void max7219_set_row(int device_num, int row, unsigned char value) {
    xprintf("max7219_set_row(%d, %d, %x);\n", device_num, row, value);
}

/* Set a whole column of LEDs.
 */
void max7219_set_col(int device_num, int col, unsigned char value) {
    xprintf("max7219_set_col(%d, %d, %x);\n", device_num, col, value);
}

/* Set the number of digits (rows) to be scanned.
 */
void max7219_set_scan_limit(int device_num, int limit) {
    xprintf("max7219_set_scan_limit(%d, %d);\n", device_num, limit);

    if (device_num<0 || device_num >= MAX7219_CONTROLLERS) {
        return;
    }

    if (limit >= 0 && limit < 8) {
        max7219_write(device_num, OP_SCANLIMIT, limit);
    }
}

/* Enable or disable the controller.
 */
void max7219_shutdown(int device_num, bool is_in_shutdown) {
    xprintf("max7219_shutdown(%d, %d);\n", device_num, is_in_shutdown);

    if (device_num<0 || device_num >= MAX7219_CONTROLLERS) {
        return;
    }

    max7219_write(device_num, OP_SHUTDOWN, is_in_shutdown);
}
