/* Copyright 2022 Dimitris Mantzouranis <d3xter93@gmail.com>
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

#include "k12.h"
#ifdef BLUETOOTH_ENABLE
#include "iton_bt.h"
#include "outputselect.h"
bool iton_bt_initialized = false;
#endif

bool dip_switch_update_kb(uint8_t index, bool active) {
    if(index == 1) {
        #ifdef BLUETOOTH_ENABLE
        if(active) {
            set_output(OUTPUT_USB);
        } else {
            if(!iton_bt_initialized) {
                iton_bt_mode_bt();
                iton_bt_initialized = true;
            }
            set_output(OUTPUT_BLUETOOTH);
        }
        #endif
    }
    return dip_switch_update_user(index, active);
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if(record->event.pressed) {
        switch (keycode) {
            #ifdef BLUETOOTH_ENABLE
                case BT_PAIR:
                    iton_bt_enter_pairing();
                    return false;
                break;
                case BT_PROFILE1:
                    iton_bt_switch_profile(0);
                    return false;
                break;
                case BT_PROFILE2:
                    iton_bt_switch_profile(1);
                    return false;
                break;
                case BT_PROFILE3:
                    iton_bt_switch_profile(2);
                    return false;
                break;
            #endif
            default:
                break;
        }
    }
    return process_record_user(keycode, record);
}
