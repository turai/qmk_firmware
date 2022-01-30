#include "../config.h"
#include <stdint.h>

#ifndef BT_MAX_PROFILES
#define BT_MAX_PROFILES 3
#endif

typedef enum {
    ITON_OS_MAC = 0x74,
    ITON_OS_WIN = 0x75,
} iton_os_type_t;


void iton_hid_report(uint8_t *data);
void iton_nkro_report(uint8_t *data);
void iton_consumer_report(uint16_t data);
void iton_system_report(uint8_t data);
void iton_fn_report(uint8_t data);
void iton_control(uint8_t cmd, uint8_t data);
void iton_mode_usb(void);
void iton_mode_bt(void);
void iton_clear_pairing(void);
void iton_pairing_mode(void);
void iton_profile(uint8_t profile);
void iton_os(iton_os_type_t type);
