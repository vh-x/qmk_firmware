# Build Options
#   change yes to no to disable
#
# Board: it should exist either in <chibios>/os/hal/boards/
#  or <this_dir>/boards
BOARD = 				FS026
EEPROM_DRIVER =			custom
NO_USB_STARTUP_CHECK = 	yes
BLUETOOTH_CUSTOM = 		yes

DEBOUNCE_TYPE = 		asym_eager_defer_pk
RAW_ENABLE            = yes   # provides raw_hid_send()
DYNAMIC_KEYMAP_ENABLE = yes   # explicit for clarity, VIA sets it implicitly
VIA_ENABLE = 			yes
