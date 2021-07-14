/* Good on you for modifying your layout! if you don't have
 * time to read the QMK docs, a list of keycodes can be found at
 *
 * https://github.com/qmk/qmk_firmware/blob/master/docs/keycodes.md
 *
 * There's also a template for adding new layers at the bottom of this file!
 */

#include QMK_KEYBOARD_H

#define BASE 0 // default layer
#define SYMB 1 // symbols
#define NUMB 2 // numbers/motion

// Blank template at the bottom
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Keymap 0: Basic layer (Workman)
 *
 * ,------------------------------.      ,--------------------------------.
 * |    Q |  D  |  R  |  W  |  B  |      |  J  |  F  |  U  |  P  |    :   |
 * |-----+-----+-----+-----+------|      |--------------------------------|
 * |CTRL/A|  S  |  H  |  T  |  G  |      |  Y  |  N  |  E  |  O  | CTRL/I |
 * |-----+-----+-----+-----+------+      |--------------------------------|
 * |SHFT/Z|  X  |  M  |  C  |  V  |      |  K  |  L  |  <  |  >  | SHFT/? |
 * `------+-----+-----+------+----'      `--------------------------------'
 *  .-------------------------.           .-----------------------.
 *  |META/ESC|ALT/ENT|SPC(SYM)|           |BSPC(NUM)|ALTGR/TAB|DEL|
 *  '-------------------------'           '-----------------------'
 */
[BASE] = LAYOUT_gergoplex(
    KC_Q,  			   KC_D, KC_R, KC_W, KC_B,  	KC_J, KC_F, KC_U,    KC_P, 	 KC_SCLN, 
    MT(MOD_LCTL, KC_A),KC_S, KC_H, KC_T, KC_G,  	KC_Y, KC_N, KC_E,    KC_O, 	 MT(MOD_RCTL, KC_I),
    MT(MOD_RSFT, KC_Z),KC_X, KC_M, KC_C, KC_V,  	KC_K, KC_L, KC_COMM, KC_DOT, MT(MOD_RSFT, KC_SLSH),

 		MT(MOD_LGUI, KC_ESC), MT(MOD_LALT, KC_ENT), LT(SYMB, KC_SPC),									// Left
		LT(NUMB, KC_BSPC), MT(MOD_RALT, KC_TAB), KC_DEL 												// Right
    ),
/* Keymap 1: Symbols layer
 * ,------------------------------.      ,--------------------------------.
 * |  !  |  @  |  {  |  }  |      |      | `   |  &  |  *  |  (  |    =   |
 * |-----+-----+-----+-----+------|      |--------------------------------|
 * |  #  |  $  |  (  |  )  |      |      |  '  |  $  |  %  |  ^  |    -   |
 * |-----+-----+-----+-----+------+      |--------------------------------|
 * |  %  |  ^  |  [  |  ]  |      |      |  \  |  !  |  @  |  #  |   +    |
 * `------+-----+-----+------+----'      `--------------------------------'
 *          .-----------------.           .------------------.
 *          |    |     |      |           | SPC | INS |  DEL |
 *          '-----------------'           '------------------'
 */
[SYMB] = LAYOUT_gergoplex(
    KC_EXLM, KC_AT,   KC_LCBR, KC_RCBR, KC_TRNS,   KC_GRV,  KC_AMPR, KC_ASTR, KC_LPRN, KC_EQL,
    KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN, KC_TRNS,   KC_QUOT, KC_DLR, KC_PERC, KC_CIRC, KC_MINS,
    KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_TRNS,   KC_BSLS, KC_EXLM,  KC_AT, KC_HASH,  KC_PLUS,
                      KC_TRNS, KC_TRNS, KC_TRNS,   KC_SPC,  KC_INS, KC_DEL
    ),
/* Keymap 2: Pad/Function layer
 * ,-----------------------------.       ,-------------------------------.
 * |      |  7  |  8  |  9  |     |      |  PLY | SKP | UP  |     |      |
 * |-----+-----+-----+-----+------|      |-------------------------------|
 * |      |  4  |  5  |  5  |     |      | HOME | LFT | DWN | RGT |  END |
 * |-----+-----+-----+-----+------+      |-------------------------------|
 * |      |  1  |  2  |  3  |     |      |VOLUP |VOLDN|     |     |      |
 * `------+-----+-----+------+----'      `-------------------------------'
 *          .-----------------.           .-----------------.
 *          |     | 0 |  SPC  |           |     |     |     |
 *          '-----------------'           '-----------------'
 */
[NUMB] = LAYOUT_gergoplex(
    KC_TRNS, KC_7, KC_8, KC_9, KC_TRNS,		KC_MPLY,  KC_MNXT, KC_UP,   KC_TRNS, KC_TRNS,
    KC_TRNS, KC_4, KC_5, KC_6, KC_TRNS,   	KC_HOME,  KC_LEFT, KC_DOWN, KC_RGHT, KC_END,
    KC_TRNS, KC_1, KC_2, KC_3, KC_TRNS,  	KC_VOLU, KC_VOLD, KC_TRNS, KC_TRNS, KC_TRNS,
     			   KC_TRNS,KC_0,KC_SPC,  	KC_TRNS,  KC_TRNS, KC_TRNS
    )
};
