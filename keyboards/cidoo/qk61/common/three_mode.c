/* Copyright 2024 Finalkey
 * Copyright 2024 LiWenLiu <https://github.com/LiuLiuQMK>
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
#include "rdmctmzt_common.h"

uint16_t Usb_Suspend_Delay = 0;
uint16_t Usb_Change_Mode_Delay = 0;
bool Usb_Change_Mode_Wakeup = false;
bool Mode_Synchronization_Signal = false;

uint16_t g_usb_sof_frame_id = 0;
uint16_t g_usb_sof_frame_id_last = 0;
bool Usb_Dis_Connect = false;

void Mode_Synchronization(void) {
    switch(Keyboard_Info.Key_Mode) {
        case QMK_BLE_MODE: {
            switch (Keyboard_Info.Ble_Channel) {
                case QMK_BLE_CHANNEL_1: Spi_Send_Commad(USER_SWITCH_BLE_1_MODE);    break;
                case QMK_BLE_CHANNEL_2: Spi_Send_Commad(USER_SWITCH_BLE_2_MODE);    break;
                case QMK_BLE_CHANNEL_3: Spi_Send_Commad(USER_SWITCH_BLE_3_MODE);    break;
                default:                                                            break;
            }
            break;
        }
        case QMK_2P4G_MODE:             Spi_Send_Commad(USER_SWITCH_2P4G_MODE);     break;
        case QMK_USB_MODE:              Spi_Send_Commad(USER_SWITCH_USB_MODE);      break;
        default:                                                                    break;
    }
}

void Ble_Name_Synchronization(void) {
    switch(Ble_Name_Spi_Count) {
        case QMK_BLE_CHANNEL_1:         Spi_Send_Commad(USER_BLE1_WRITE_NAME);      Ble_Name_Spi_Count++;   break;
        case QMK_BLE_CHANNEL_2:         Spi_Send_Commad(USER_BLE2_WRITE_NAME);      Ble_Name_Spi_Count++;   break;
        case QMK_BLE_CHANNEL_3:         Spi_Send_Commad(USER_BLE3_WRITE_NAME);      Ble_Name_Spi_Count++;   break;
        default:                                                                                            break;
    }

    if(Ble_Name_Spi_Count >  QMK_BLE_CHANNEL_3) {
        Ble_Name_Spi_Count = QMK_BLE_CHANNEL_1;
        Ble_Name_Spi_Send = false;
    }
}

void Sleep_Time_Synchronization(void) {
    Spi_Send_Commad(USER_SLEEP_TIME_DATA);
    Sleep_Time_Spi_Send = false;
}

void Spi_Cmd_16_Synchronization(void) {
    Spi_Send_Commad(USER_SPI_CMD_16_DATA);
    Spi_Cmd_16_Spi_Send = false;
}

static uint8_t es_keyboard_leds(void) {
    switch (Keyboard_Info.Key_Mode) {
        case QMK_2P4G_MODE:
        case QMK_BLE_MODE:
            return Keyboard_Status.System_Led_Status;
        case QMK_USB_MODE:
        default:
            break;
    }

    if(es_qmk_driver) {
        return((*es_qmk_driver->keyboard_leds)());
    }

    return 0;
}

static void es_send_keyboard(report_keyboard_t *report) {
    switch (Keyboard_Info.Key_Mode) {
        case QMK_2P4G_MODE: User_bluetooth_send_keyboard((uint8_t *)report,sizeof(report_keyboard_t));  break;
        case QMK_BLE_MODE:  User_bluetooth_send_keyboard((uint8_t *)report,sizeof(report_keyboard_t));  break;
        case QMK_USB_MODE:  break;
        default:            break;
    }

    if(es_qmk_driver) {
        (*es_qmk_driver->send_keyboard)(report);
    }
}

static void es_send_nkro_report(report_nkro_t *report) {
    switch (Keyboard_Info.Key_Mode) {
        case QMK_2P4G_MODE: User_bluetooth_send_keyboard((uint8_t *)report,sizeof(report_nkro_t));  break;
        case QMK_BLE_MODE:  User_bluetooth_send_keyboard((uint8_t *)report,sizeof(report_nkro_t));  break;
        case QMK_USB_MODE:  break;
        default:            break;
    }

    if(es_qmk_driver) {
        (*es_qmk_driver->send_nkro)(report);
    }
}

/* V0107 hybrid report, done here so tmk_core/quantum stay stock. With NKRO on, QMK hands this wrapper
 * its full NKRO bitmap; the wrapper replays the key changes the way the factory report code does:
 * the first six keys held take the 6KRO slots (sent with the mods through es_send_keyboard), later
 * keys only exist in an NKRO report that carries just those extra keys and no mods. Pressing or
 * releasing an extra key sets the overflow state, a key taking or leaving a 6KRO slot or a change
 * of the real mods clears it, and the NKRO report is sent only while it is set. Both reports reach
 * USB and the 2.4G/BLE radio queue through the same es_send_* paths. */
static uint8_t Hybrid_Keys[KEYBOARD_REPORT_KEYS];
static uint8_t Hybrid_Bits[NKRO_REPORT_BITS];
static uint8_t Hybrid_Mods;
static bool Hybrid_Overflow;

void es_send_nkro(report_nkro_t *report) {
    if (Hybrid_Mods != get_mods()) {
        Hybrid_Mods = get_mods();
        Hybrid_Overflow = false;
    }

    for (uint16_t key = 0; key < (NKRO_REPORT_BITS * 8); key++) {
        uint8_t mask = 1 << (key & 7);
        uint8_t i = 0;
        if ((Hybrid_Bits[key >> 3] & mask) && !(report->bits[key >> 3] & mask)) {
            Hybrid_Bits[key >> 3] &= ~mask;
            while ((i < KEYBOARD_REPORT_KEYS) && (Hybrid_Keys[i] != key)) {
                i++;
            }
            if (i < KEYBOARD_REPORT_KEYS) {
                for (; i < (KEYBOARD_REPORT_KEYS - 1); i++) {
                    Hybrid_Keys[i] = Hybrid_Keys[i + 1];
                }
                Hybrid_Keys[KEYBOARD_REPORT_KEYS - 1] = 0;
                Hybrid_Overflow = false;
            } else {
                Hybrid_Overflow = true;
            }
        }
    }

    for (uint16_t key = 0; key < (NKRO_REPORT_BITS * 8); key++) {
        uint8_t mask = 1 << (key & 7);
        uint8_t i = 0;
        if (!(Hybrid_Bits[key >> 3] & mask) && (report->bits[key >> 3] & mask)) {
            Hybrid_Bits[key >> 3] |= mask;
            while ((i < KEYBOARD_REPORT_KEYS) && Hybrid_Keys[i]) {
                i++;
            }
            if (i < KEYBOARD_REPORT_KEYS) {
                Hybrid_Keys[i] = key;
                Hybrid_Overflow = false;
            } else {
                Hybrid_Overflow = true;
            }
        }
    }

    if (Hybrid_Overflow) {
        report_nkro_t nkro_report = {.report_id = report->report_id};
        memcpy(nkro_report.bits, Hybrid_Bits, sizeof(nkro_report.bits));
        for (uint8_t i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
            nkro_report.bits[Hybrid_Keys[i] >> 3] &= ~(1 << (Hybrid_Keys[i] & 7));
        }
        es_send_nkro_report(&nkro_report);
    } else {
        report_keyboard_t keyboard_report = {.mods = report->mods};
#ifdef KEYBOARD_SHARED_EP
        keyboard_report.report_id = REPORT_ID_KEYBOARD;
#endif
        memcpy(keyboard_report.keys, Hybrid_Keys, sizeof(keyboard_report.keys));
        es_send_keyboard(&keyboard_report);
    }
}

void es_send_mouse(report_mouse_t *report) {
    switch (Keyboard_Info.Key_Mode) {
        case QMK_2P4G_MODE: User_bluetooth_send_keyboard((uint8_t *)report,sizeof(report_mouse_t));  break;
        case QMK_BLE_MODE:  User_bluetooth_send_keyboard((uint8_t *)report,sizeof(report_mouse_t));  break;
        case QMK_USB_MODE:  break;
        default:            break;
    }

    if(es_qmk_driver) {
        (*es_qmk_driver->send_mouse)(report);
    }
}

static void es_send_extra(report_extra_t *report) {
    switch (Keyboard_Info.Key_Mode) {
        case QMK_2P4G_MODE: User_bluetooth_send_keyboard((uint8_t *)report,sizeof(report_extra_t));  break;
        case QMK_BLE_MODE:  User_bluetooth_send_keyboard((uint8_t *)report,sizeof(report_extra_t));  break;
        case QMK_USB_MODE:  break;
        default:            break;
    }

    if(es_qmk_driver) {
        (*es_qmk_driver->send_extra)(report);
    }
}

#ifdef RAW_ENABLE
static void es_send_raw_hid(uint8_t *data, uint8_t length) {
    if(!es_qmk_driver) {
        return;
    }

    if(es_qmk_driver && es_qmk_driver->send_raw_hid) {
        (*es_qmk_driver->send_raw_hid)(data, length);
    }
}
#endif

volatile host_driver_t * es_qmk_driver = NULL;
const host_driver_t es_user_driver  = {
    .keyboard_leds = es_keyboard_leds,
    .send_keyboard = es_send_keyboard,
    .send_nkro = es_send_nkro,
    .send_mouse = es_send_mouse,
    .send_extra = es_send_extra,
#ifdef RAW_ENABLE
    .send_raw_hid = es_send_raw_hid,
#endif
};

void User_bluetooth_send_keyboard(uint8_t *report, uint32_t len) {
    if(app_2g4_buffer_full()) {
        return;
    }

    if (len > USER_KEYBOARD_LENGTH - 3) {
        len = USER_KEYBOARD_LENGTH - 3;
    }

    uint8_t Temp_Tx_Buf[USER_KEYBOARD_LENGTH] = {0};

    Temp_Tx_Buf[0] = USER_KEYBOARD_COMMAND;
    Temp_Tx_Buf[1] = USER_KEYBOARD_LENGTH;

    if (len == sizeof(report_keyboard_t)) {  // 6KRO
        memcpy((void *)&Temp_Tx_Buf[3], report, len);
        Temp_Tx_Buf[2] = USER_KEY_BYTE_DATA;
    } else {
        switch (report[0]) {
            case KB_REPORT_ID: {            // NKRO
                memcpy((void *)&Temp_Tx_Buf[3], report, len);
                Temp_Tx_Buf[2] = USER_KEY_BIT_DATA;
                break;
            }
            case SYS_REPORT_ID: {           // system control
                memcpy((void *)&Temp_Tx_Buf[3], report, len);
                Temp_Tx_Buf[2] = USER_SYSTEM_DATA;
                break;
            }
            case CON_REPORT_ID: {           // consumer
                memcpy((void *)&Temp_Tx_Buf[3], report, len);
                Temp_Tx_Buf[2] = USER_CONSUMER_DATA;
                break;
            }
            case MOUSE_REPORT_ID: {         // mouse
                memcpy((void *)&Temp_Tx_Buf[3], report, len);
                Temp_Tx_Buf[2] = USER_MOUSE_DATA;
                break;
            }
            default: {
                break;
            }
        }
    }

    memcpy(app_2g4_data[app_2g4_data_rev], Temp_Tx_Buf, USER_KEYBOARD_LENGTH);
    app_2g4_buffer_rev_add();
}

void User_Usb_Init(void) {
    /*Using USB_SOF to calibrate the internal clock*/
    md_rcu_enable_csu(RCU);
    CSU->CON |= CSU_CON_AUTOEN_MSK;
    CSU->CON |= CSU_CON_CNTEN_MSK;
}

void es_restart_usb_driver(void) {
    md_rcu_enable_usb(RCU);
    ald_usb_device_components_init();
    USB->TXIER = 0x7F;
    USB->RXIER = 0x7E;
    USB->IER = 0x2F;
    usb_lld_connect_bus(0);
    ald_usb_int_register();
}

void Usb_Disconnect(void) {
    ald_usb_int_unregister();
    usb_lld_disconnect_bus(0);

    md_rcu_enable_usb_reset(RCU);
    md_rcu_disable_usb_reset(RCU);
    md_rcu_disable_usb(RCU);
}

void User_Usb_Deinit(void) {
    md_rcu_enable_csu_reset(RCU);
    md_rcu_disable_csu_reset(RCU);
    md_rcu_disable_csu(RCU);
}
