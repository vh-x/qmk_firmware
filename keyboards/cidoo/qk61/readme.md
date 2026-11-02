# CIDOO QK61

A 61-key wireless (USB, 2.4 GHz, Bluetooth) 60% keyboard with per-key RGB and a three-LED logo bar. This port reproduces the behaviour of the factory firmware V0107.

* Keyboard Maintainer: [The QMK Community](https://github.com/qmk)
* Hardware Supported: CIDOO QK61 PCB, ES32FS026 microcontroller
* Hardware Availability: CIDOO

Make example for this keyboard (after setting up your build environment):

    make cidoo/qk61:default

Flashing example for this keyboard: copy the `.bin` file to the drive that appears in bootloader mode (`qmk flash` is not supported).

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 2 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (Esc) and plug in the keyboard. This also clears the QMK EEPROM (VIA keymap and lighting).
* **Physical reset button**: Briefly press the button on the back of the PCB.

## Keyboard settings

Mode, Bluetooth channel, Windows/Mac, Win lock, NKRO, debounce, sleep timeout and logo settings live on a separate flash page in the same 24-byte layout as the factory firmware, so they survive switching between this firmware and V0107. Holding Fn+Backspace for 3 seconds restores the factory defaults.

Keys that act after a 3 second hold: Fn+O (6KRO/NKRO), Fn+K (debounce 5 ms / 2 ms), Fn+I (wireless sleep timeout 1, 3, 10 or 30 minutes).
