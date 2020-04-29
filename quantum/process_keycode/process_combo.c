/* Copyright 2016 Jack Humbert
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

#include "print.h"
#include "process_combo.h"

#ifndef COMBO_VARIABLE_LEN
__attribute__((weak)) combo_t key_combos[COMBO_COUNT] = {};
#else
extern combo_t  key_combos[];
extern int      COMBO_LEN;
#endif

__attribute__((weak)) void process_combo_event(uint8_t combo_index, bool pressed) {}

// Combos State
static uint16_t timer               = 0;
static uint16_t current_combo_index = 0;
static bool     b_combo_enable      = true;  // defaults to enabled
static uint8_t  buffer_size         = 0;
static uint8_t  cmb_keys_down       = 0;
static combo_t* toRelease           = 0;

// Helper Macros
#ifdef COMBO_ALLOW_ACTION_KEYS
static keyrecord_t key_buffer[MAX_COMBO_LENGTH];
#else
static uint16_t key_buffer[MAX_COMBO_LENGTH];
#endif

#define MULTIPLE_VALID_COMBOS   (0-1)
#define ALL_COMBO_KEYS_ARE_DOWN (((1 << count) - 1) == combo->state)
#define KEY_STATE_DOWN(key)         \
    do {                            \
        combo->state |= (1 << key); \
    } while (0)
#define KEY_STATE_UP(key)            \
    do {                             \
        combo->state &= ~(1 << key); \
    } while (0)
#define NO_COMBO_KEYS_ARE_DOWN (0 == combo->state)

static inline void send_combo(uint16_t action, bool pressed) {
    if (action) {
        if (pressed) {
            register_code16(action);
        } else {
            unregister_code16(action);
        }
    } else {
        process_combo_event(current_combo_index, pressed);
    }
}
static inline void dump_key_buffer(bool emit) {
    if (buffer_size == 0) {
        return;
    }

    if (emit) {
        for (uint8_t i = 0; i < buffer_size; i++) {
#ifdef COMBO_ALLOW_ACTION_KEYS
          const action_t action = store_or_get_action(key_buffer[i].event.pressed, key_buffer[i].event.key);
          process_action(&(key_buffer[i]), action);
#else
          register_code16(key_buffer[i]);
          send_keyboard_report();
#endif
        }
    }

    buffer_size = 0;
}

// Search all combos for KC, sets combo state
bool keycodeInCombos(uint16_t kc, bool pressed) {
  bool wasFound = false;
#ifndef COMBO_VARIABLE_LEN
    for (current_combo_index = 0; current_combo_index < COMBO_COUNT; ++current_combo_index)
#else
    for (current_combo_index = 0; current_combo_index < COMBO_LEN; ++current_combo_index) 
#endif
    {
      combo_t *combo = &key_combos[current_combo_index];
      int8_t   count = 0;
      for (const uint16_t *keys = combo->keys;; ++count) {
          uint16_t key = pgm_read_word(&keys[count]);
          if (kc == key) {
            wasFound = true;
            if (pressed) KEY_STATE_DOWN(count);
            else         KEY_STATE_UP(count);   
          }
          if (key == COMBO_END) break;
      }
    }

    return wasFound;
}
// Return # of valid combos
// if a single combo is valid, current_combo_index is set otherwise to MULTIPLE_VALID_COMBOS
uint16_t validCombos(void) {
  uint16_t nValid = 0;
  uint16_t lastValid = MULTIPLE_VALID_COMBOS;

  //Make sure we even have a buffer
  if (buffer_size == 0) return 0;

#ifndef COMBO_VARIABLE_LEN
  for (current_combo_index = 0; current_combo_index < COMBO_COUNT; ++current_combo_index)
#else
  for (current_combo_index = 0; current_combo_index < COMBO_LEN; ++current_combo_index)
#endif
  {
    combo_t *combo      = &key_combos[current_combo_index];
    int8_t   count      = 0;
    int8_t   keysFound  = 0;

    // Check validity of keys in combo
    for (uint8_t i = 0; i < buffer_size; i++) {
      // Try to find key in combo
      count = 0;
      for (const uint16_t *keys = combo->keys;; ++count) {
          uint16_t key = pgm_read_word(&keys[count]);
          // Found, look for next
#ifdef COMBO_ALLOW_ACTION_KEYS
          if (key == get_record_keycode(&key_buffer[i], false))
#else
          if (key == key_buffer[i])
#endif
          {
            keysFound++;
            break;
          }

          // End of combo
          if (key == COMBO_END) break;
      }
    }

    // Is the entire buffer valid? 
    if (keysFound == buffer_size) {
      nValid++;
      lastValid = current_combo_index;
    }
  }

  if (nValid == 1) current_combo_index = lastValid;
  else             current_combo_index = MULTIPLE_VALID_COMBOS;

  return nValid;
}

// Check if kc exists in any combos, sets key state
bool process_combo(uint16_t keycode, keyrecord_t *record) {
    bool is_combo_key          = false;

    // Combo Toggle housekeeping
    if (keycode == CMB_ON && record->event.pressed) {
        combo_enable();
        return true;
    }

    if (keycode == CMB_OFF && record->event.pressed) {
        combo_disable();
        return true;
    }

    if (keycode == CMB_TOG && record->event.pressed) {
        combo_toggle();
        return true;
    }

    if (!is_combo_enabled()) {
        return true;
    }

    // Keycodes up, process/dump any buffers
    if (!record->event.pressed) {
      dump_key_buffer(true);
      if (toRelease) {
        send_combo(toRelease->keycode, false);
      }

      return true;
    }

    // Consume key
    if (buffer_size < MAX_COMBO_LENGTH) {
#ifdef COMBO_ALLOW_ACTION_KEYS
      key_buffer[buffer_size++] = *record;
#else
      key_buffer[buffer_size++] = keycode;
#endif
      uprintf("adding to buf %d\n", buffer_size);
    }
    
    // Check if keycode exists in any combos
    //    - Hold off on processing for perf resasons
    //    - Set pressed states
    is_combo_key   = keycodeInCombos(keycode, record->event.pressed);
    if      (is_combo_key && record->event.pressed)   cmb_keys_down++;
    else if (is_combo_key && !record->event.pressed)  cmb_keys_down--;

    // Calc number of matching combos setting internal states
    //    - If 1 send and reset
    //    - Buffer additional inputs
    if (is_combo_key) {
      // Top up timer
      timer = timer_read();


      // Check if we can emit yet
      int16_t nCombos = validCombos();
      uprintf("nCmb: %d nDown: %d nBuf: %d\n", nCombos, cmb_keys_down, buffer_size);

      // Buffer contains exactly one combo
      if (nCombos == 1) {
        combo_t *combo = &key_combos[current_combo_index];
        
        // Send combo, consume buffer
        if (record->event.pressed)  {
          uprintf("Sending Press");
          send_combo(combo->keycode, true);
          toRelease = combo;
          dump_key_buffer(false);
        } 
      // Buffer has exhausted possible combos, dump it
      } else if (nCombos == 0) {
        uprintf("Dropping buf emit\n");
        dump_key_buffer(true);
      }
    // Not a combo key, dump buffer
    } else {
      uprintf("Dropping buf emit\n");
      dump_key_buffer(true);
    }

    return !is_combo_key;
}

void matrix_scan_combo(void) {
    if (b_combo_enable && timer && timer_elapsed(timer) > COMBO_TERM) {
        // Emit combo if still held
        if (toRelease != 0) {
          send_combo(toRelease->keycode, false);
          toRelease = 0;
          dump_key_buffer(false);
        }

        // Otherwise disable combos
        dump_key_buffer(true);
    }
}

// Enable/Disable subsytem
void combo_enable(void) { b_combo_enable = true; }
void combo_disable(void) {
    b_combo_enable = false;
    timer          = 0;
    dump_key_buffer(true);
}
void combo_toggle(void) {
    if (b_combo_enable) combo_disable();
    else                combo_enable();
}
bool is_combo_enabled(void) { return b_combo_enable; }
