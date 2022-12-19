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

#pragma once

#include "quantum.h"

enum K12_keycodes {
    BT_PROFILE1 = SAFE_RANGE,
    BT_PROFILE2,
    BT_PROFILE3,
    BT_PAIR,
    BT_TOGGLE,
    K12_SAFE_RANGE
};

#if defined(KEYBOARD_keychron_k12_rgb)
    #include "rgb.h"
#elif defined(KEYBOARD_keychron_k12_white)
    #error "nope"
    #include "white.h"
#endif
