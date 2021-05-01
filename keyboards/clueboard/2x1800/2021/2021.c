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
#include "spi_master.h"
#include "2021.h"

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

void matrix_init_kb(void) {
    // Initialize the LED driver
    spi_init();
    spi_start(B0, false, 0, 2);
	buffer[0] = SCAN_LIMIT;
    buffer[1] = 0x07; // Enable all rows
    spi_transmit(buffer, 2);
	buffer[0] = BRIGHTNESS;
    buffer[1] = 0x07; // Med-low brightness
    spi_transmit(buffer, 2);

    // Blank all the LEDs
    buffer[1] = 0x00;
    buffer[0] = ROW_0;
    spi_transmit(buffer, 2);
    buffer[0] = ROW_1;
    spi_transmit(buffer, 2);
    buffer[0] = ROW_2;
    spi_transmit(buffer, 2);
    buffer[0] = ROW_3;
    spi_transmit(buffer, 2);
    buffer[0] = ROW_4;
    spi_transmit(buffer, 2);
    buffer[0] = ROW_5;
    spi_transmit(buffer, 2);
    buffer[0] = ROW_6;
    spi_transmit(buffer, 2);
    buffer[0] = ROW_7;
    spi_transmit(buffer, 2);

    // Enable the display
    buffer[0] = OPERATION_MODE;
    buffer[1] = 0x01;
    spi_transmit(buffer, 2);

    // Blink the first row
    buffer[0] = ROW_0;

	/*
    while (true) {
        buffer[1] = 0xFF;
        spi_transmit(buffer, 2);
        wait_ms(200);
        buffer[1] = 0x00;
        spi_transmit(buffer, 2);
        wait_ms(200);
    }
	*/
}

__attribute__ ((weak))
bool encoder_update_keymap(int8_t index, bool clockwise) {
    return false;
}

void encoder_update_kb(int8_t index, bool clockwise) {
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
