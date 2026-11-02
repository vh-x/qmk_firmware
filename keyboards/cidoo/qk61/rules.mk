# Build Options
#   change yes to no to disable
#
# ES32FS026 (Cortex-M0), from lib/chibios-contrib/os/hal/ports/ES32/FS026
ARMV = 6
MCU_FAMILY = ES32
MCU_SERIES = FS026
MCU_LDSCRIPT = FS026
MCU_STARTUP = FS026

BOARD = FS026
EEPROM_DRIVER = custom
NO_USB_STARTUP_CHECK = yes

DEBOUNCE_TYPE = custom
VIA_ENABLE = yes

# Common library
VPATH += keyboards/cidoo/qk61/common
SRC += rdmctmzt_common.c
SRC += three_mode.c
SRC += user_battery.c
SRC += user_debounce.c
SRC += user_eeprom.c
SRC += user_emi.c
SRC += user_led_custom.c
SRC += user_logo_custom.c
SRC += user_spi.c
SRC += user_system.c
