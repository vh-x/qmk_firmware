// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "common/rdmctmzt_common.h"

/* Two layers, base and Fn. Mac mode is QMK's Alt/GUI swap: the host OS sets it on USB and
 * Fn+Z (AG_TOGG) flips it manually. */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_60_ansi(
        QK_GESC, KC_1,     KC_2,     KC_3,      KC_4,      KC_5,     KC_6,     KC_7,    KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,
        KC_TAB,  KC_Q,     KC_W,     KC_E,      KC_R,      KC_T,     KC_Y,     KC_U,    KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,
        KC_CAPS, KC_A,     KC_S,     KC_D,      KC_F,      KC_G,     KC_H,     KC_J,    KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,
        KC_LSFT, KC_Z,     KC_X,     KC_C,      KC_V,      KC_B,     KC_N,     KC_M,    KC_COMM,  KC_DOT,             KC_SLSH,            MT(MOD_RSFT, KC_UP),
        KC_LCTL, KC_LGUI,  KC_LALT,                        KC_SPC,                                MO(1),    KC_LEFT,  KC_DOWN,  KC_RGHT
    ),
    [1] = LAYOUT_60_ansi(
        KC_GRV,  KC_F1,    KC_F2,    KC_F3,     KC_F4,     KC_F5,    KC_F6,    KC_F7,   KC_F8,     KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_DEL,
        KC_TRNS, MD_BLE1,  MD_BLE2,  MD_BLE3,   MD_24G,    MD_USB,   KC_TRNS,  KC_TRNS, KC_INS,   KC_TRNS,  KC_PSCR,  RM_PREV,  RM_NEXT,  RGB_RTOG,
        KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,   KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS,   KC_TRNS,  KC_BRID,  KC_BRIU,            KC_TRNS,
        KC_TRNS, AG_TOGG,  KC_TRNS,  KC_TRNS,   KC_TRNS,   QK_BAT,   KC_TRNS,  KC_TRNS, KC_VOLD,   KC_VOLU,            KC_MUTE,            KC_PGUP,
        KC_TRNS, KC_TRNS,  KC_TRNS,                        KC_TRNS,                                KC_TRNS,  KC_HOME,  KC_PGDN,  KC_END
    )
};

void keyboard_post_init_user(void) {
    // The keyboard turns on layer 1 at boot in saved Mac mode; here layer 1 is Fn.
    layer_off(1);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == MO(1)) {
        Key_Fn_Status = record->event.pressed; // light the current mode key while Fn is held
    }
    return true;
}

// Detection only runs over USB; Fn+Z sets the swap manually.
bool process_detected_host_os_user(os_variant_t detected_os) {
    keymap_config.swap_lalt_lgui = detected_os == OS_MACOS || detected_os == OS_IOS;
    return true;
}
