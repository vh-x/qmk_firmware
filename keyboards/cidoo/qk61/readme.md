# CIDOO QK61

A 61-key wireless keyboard with USB, three Bluetooth channels and 2.4 GHz. It uses an ES32 FS026 microcontroller.

## Building

    qmk compile -kb cidoo/qk61 -km default

The ChibiOS-Contrib submodule uses a fork with an ES32 USB enumeration fix required for macOS.

## Flashing

1. With the slider set to USB, unplug the keyboard, hold Esc and plug it in.
2. Copy the compiled `.bin` file to the mass-storage drive that appears.
3. Unplug and reconnect the keyboard after the drive disappears.

Holding Esc invokes Bootmagic and erases EEPROM, including the VIA keymap and saved settings. Save your VIA layout first. `qmk flash` is not supported.

## Keymap

The default keymap uses USB OS detection to swap Alt and GUI for macOS and iOS. Fn+Z toggles the swap manually; detection reapplies it on the next USB connection.

Fn+Q/W/E selects Bluetooth channels 1/2/3, Fn+R selects 2.4 GHz, and Fn+T selects USB. Hold a wireless mode key for three seconds to pair. The physical slider takes precedence at startup.

6KRO is the default. NKRO can be enabled with Fn+N, but Bluetooth always uses 6KRO. Fn+Space displays the battery level, and Fn+Backslash toggles the key backlight while retaining indicators.

All Bluetooth channels advertise as `CIDOO QK61`. To change the name, also change `USER_BlE_ID` in `config.h` so the radio module accepts it. A host may need to forget and pair the device again.

## VIA

Keep `VIA_ENABLE = yes`; the keyboard does not initialize without it. VIA requires USB. Enable **Show Design tab** in VIA and load `CIDOO QK61 VIA.JSON` from this directory. The key backlight is under **Backlight** and the three-LED bar is under **logo**.

Vendor keycodes from 0x7E0A onward differ by one from the factory firmware. Reconfigure affected logo keys when importing a factory VIA layout.
