/* Copyright 2021 'Matthew Westerfield'
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


#include QMK_KEYBOARD_H


enum layer_names {
    _BASE,
    _SYM_NUM,
    _FKEYS
};

enum custom_keycodes {
    BASE = SAFE_RANGE,
    SYM_NUM,
    FKEYS
};


#define CTL_ENT CTL_T(KC_ENT)
#define ALT_TAB ALT_T(KC_TAB)
#define SFT_BSPC SFT_T(KC_BSPC)

#define M_SYM MO(_SYM_NUM)
#define T_SYM TG(_SYM_NUM)
#define M_FKEYS MO(_FKEYS)
#define T_FKEYS TG(_FKEYS)


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x7_4(
        KC_GRAVE, KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_BSLS,    KC_EQL  , KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_MINS,
        KC_DEL  , KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , T_SYM  ,    T_FKEYS , KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT,
        M_FKEYS , KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_ESC ,    KC_RGUI , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, M_SYM  ,
                                    KC_LALT, M_SYM  , CTL_ENT, ALT_TAB,    SFT_BSPC, KC_SPC , M_FKEYS  , KC_RALT
    ),
    [_SYM_NUM] = LAYOUT_split_3x7_4(
        _______ , KC_EXLM, KC_AT  , KC_LCBR, KC_RCBR, KC_PIPE, _______,    _______, _______, KC_P7  , KC_P8  , KC_P9  , KC_PAST, KC_NLCK,
        _______ , KC_HASH, KC_DLR , KC_LPRN, KC_RPRN, KC_TILD, _______,    _______, _______, KC_P4  , KC_P5  , KC_P6  , KC_PPLS, _______,
        _______ , KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, _______, _______,    _______, KC_AMPR, KC_P1  , KC_P2  , KC_P3  , KC_PSLS, _______,
                                    _______, _______, _______, _______,    _______, _______, KC_P0  , KC_PDOT
    ),
    [_FKEYS] = LAYOUT_split_3x7_4(
        _______ , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , _______,    _______, KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 ,
        _______ , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , _______,    _______, _______, KC_LEFT, KC_DOWN, KC_UP  , KC_RGHT, KC_F12 ,
        _______ , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , _______,    _______, _______, KC_HOME, KC_PGDN, KC_PGUP, KC_END , _______,
                                    _______, _______, _______, _______,    _______, _______, _______, _______
    )
};


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    /* if (record->event.pressed)
    {
         set_timelog();
    }

    switch (keycode)
    {
    case SYM_NUM:
        if (record->event.pressed)
        {
            layer_on(_SYM_NUM);
            // update_tri_layer(_SYM_NUM, _FKEYS, _BASE);
        }
        else
        {
            layer_off(_SYM_NUM);
            // update_tri_layer(_SYM_NUM, _FKEYS, _BASE);
        }
        return false;
        break;
    case FKEYS:
        if (record->event.pressed)
        {
            layer_on(_FKEYS);
            // update_tri_layer(_SYM_NUM, _FKEYS, _BASE);
        }
        else
        {
            layer_off(_FKEYS);
            // update_tri_layer(_SYM_NUM, _FKEYS, _BASE);
        }
        return false;
        break;
    default:
        break;
    } */
    return true;
}
