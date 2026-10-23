# CIDOO QK61 V2

Wireless 61-key 60% keyboard: USB, three Bluetooth channels and 2.4 GHz, selected with the slider on the back. 64 RGB LEDs (61 keys plus the 3-LED bar left of Esc).

* Keyboard maintainer: [vh-x](https://github.com/vh-x)
* Hardware: Essemi ES32 FS026 (Cortex-M0, 128 KB flash, 16 KB RAM), radio module on SPI2, WS2812 LEDs on PWM+DMA
* Factory firmware: `CIDOO-QK61-V2-CSF718-V0102-20241107.bin`

This port was started by [aliveoutside](https://github.com/aliveoutside/qmk_firmware) as a C rewrite of the vendor library. It was then verified against a decompilation of the factory V0102 image; the differences that were found are fixed here, and the remaining intentional ones are listed at the end.

## Building

    qmk compile -kb cidoo/qk61 -km default
    qmk compile -kb cidoo/qk61 -km osdetect

The `lib/chibios-contrib` submodule points at [vh-x/ChibiOS-Contrib](https://github.com/vh-x/ChibiOS-Contrib) (branch `qk61-es32-usb-address`). It carries a one-function fix to the ES32 USB driver without which the board never finishes enumerating on macOS. Nothing else in the submodule differs from QMK's.

Tested toolchain: Arm GNU Toolchain 14.2.rel1. The QMK default toolchain works as well.

## Flashing

The chip has a ROM mass-storage bootloader; `qmk flash` is not supported.

1. Unplug the keyboard, hold Esc, plug it in with the slider at USB. A FAT drive named `NO NAME` (108 KB, USB ID `03EB:2045`) appears.
2. Copy the `.bin` onto it. The drive disappears when the write is accepted.
3. Unplug and plug the keyboard in again. The board does not restart on its own after a write.

Entering the bootloader with Esc is QMK Bootmagic, so it also **erases the EEPROM**: the VIA keymap, the lighting settings and the vendor settings (mode, Mac/Win, LED on/off) all return to defaults. Save your layout from VIA first.

`QK_BOOT` and a raw-HID bootloader command do reset into the bootloader, but the drive only shows up after a power cycle, so they save nothing over the Esc method. There is no reset button on the PCB.

## Keymaps

Layers: 0 Windows, 1 Mac (Alt and GUI swapped), 2 Fn for Windows, 3 Fn for Mac (adds brightness and media keys on the number row). Fn is the key right of RCtrl.

* `default`: factory behaviour. Fn+A selects Windows mode, Fn+S Mac mode; the choice is saved.
* `osdetect`: same layout, plus the host OS detected over USB selects the mode (macOS/iOS to Mac, Windows/Linux to Windows, unknown leaves it), and 6KRO is forced while in Mac mode because NKRO breaks the Caps Lock input-source switch on macOS.

Fn layer (default keymap, Windows layer shown):

| Keys | Function |
|---|---|
| Fn + 1 … = | F1 … F12 |
| Fn + Esc | ` |
| Fn + Q / W / E | Bluetooth channel 1 / 2 / 3; hold 3 s to pair |
| Fn + R | 2.4 GHz; hold 3 s to pair with the dongle |
| Fn + A / S | Windows / Mac mode |
| Fn + T / Y / U | Print Screen / Scroll Lock / Pause |
| Fn + G / H / J, B / N / M | Insert / Home / PgUp, Delete / End / PgDn |
| Fn + / and RAlt / Menu / RCtrl | Up and Left / Down / Right |
| Fn + Space | RGB on/off (QMK `RM_TOGG`, also turns indicators off) |
| Fn + Z | next RGB effect |
| Fn + [ ] | effect speed, Fn + ; ' hue, Fn + C V saturation, Fn + , . brightness |
| Fn + \ | next RGB effect |
| Fn + LGUI | lock the GUI key (saved, cleared in Mac mode) |
| Fn + RShift | show battery level on the number row |

Vendor keycodes available in VIA (0x7E00 and up): mode select for 2.4G/BLE1/BLE2/BLE3/USB, battery display, GUI lock, 6KRO/NKRO switch, `RTOG` (key backlight on/off that keeps the indicators working), `U_EE_CLR` (hold 3 s: factory reset), debounce switch, and `LG_*` for the logo bar (on/off, effect, hue, saturation, brightness, speed).

Slider at USB with a Bluetooth or 2.4G mode saved: the slider wins at power-on, as on the factory firmware.

## VIA

VIA needs USB; the raw-HID interface is not exposed over Bluetooth or 2.4 GHz. Enable **Show Design tab** in VIA's settings and load `CIDOO QK61 VIA.JSON` from this folder. The definition exposes the key backlight under **Backlight** and the logo bar under **logo**, and declares the RGB Matrix keycode set (`RM_*`).

Keep `VIA_ENABLE = yes`; the port does not initialise without it.

## Bluetooth name

All three channels advertise as `CIDOO QK61`. The radio module keeps its own copy of the names and only accepts new ones when the ID the firmware sends (`USER_BlE_ID` in `config.h`) differs from the one it stored, so a rename needs a new ID. Hosts cache the name of a paired device: forget it and pair again to see a change.

## Differences from the factory V0102 firmware

* Bluetooth names (`CIDOO QK61` instead of `CIDOO QK61-1/-2/-3`) and the module ID (0x002E instead of 0x002D).
* `osdetect` keymap (OS detection, 6KRO in Mac mode). The `default` keymap behaves like the factory firmware.
* USB `bcdDevice` 0.0.5 instead of 0.0.2, VIA protocol 13 instead of 12.
* Vendor keycodes from 0x7E0A up are one higher than in V0102 because the debounce keycode was inserted. A VIA layout saved under the factory firmware shows the logo keys shifted by one; set them again once.
