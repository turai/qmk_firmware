#pragma once
#include <stdint.h>
#include <stdbool.h>

// Clears all pairing records
void iton_clear_pairing(void);

// Enters pairing mode for the current profile
void iton_enter_pairing(void);

// Switches to the nth profile.
// Some versions might only support 3
void iton_switch_profile(uint8_t profile);

// macOS mode
void iton_macos_mode(void);

// Windows mode
void iton_windows_mode(void);

// Switch between USB and bluetooth
void iton_toggle_bt(void);
void iton_switch_bt(void);
void iton_switch_usb(void);

// Change the bluetooth name.
void iton_change_name(char *name, uint8_t len);
