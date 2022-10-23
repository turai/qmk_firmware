# MCU name
MCU = SN32F248BF
BOOTLOADER = sn32-dfu

# Build Options
#   comment out to disable the options.
#
BOOTMAGIC_ENABLE 		= yes # Virtual DIP switch configuration
MAGIC_ENABLE 			= yes
MAGIC_KEYCODE_ENABLE 	= yes
MOUSEKEY_ENABLE 		= no    # Mouse keys
EXTRAKEY_ENABLE 		= yes   # Audio control and System control
CONSOLE_ENABLE 			= no     # Console for debug
COMMAND_ENABLE 			= no     # Commands for debug and configuration
NKRO_ENABLE 			= yes       # USB Nkey Rollover

WAIT_FOR_USB 			= no
KEYBOARD_SHARED_EP 		= yes

BLUETOOTH_ENABLE 		= yes
BLUETOOTH_DRIVER 		= ItonBT

CUSTOM_MATRIX 			= yes

# Custom RGB matrix handling
RGB_MATRIX_ENABLE 		= yes
RGB_MATRIX_DRIVER		= SN32F24xB

# project specific files
SRC = ../../../drivers/led/sn32/matrix_sn32f24xx.c

DEFAULT_FOLDER = keychron/k6/rgb/ansi
