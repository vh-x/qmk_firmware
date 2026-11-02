// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define OS_DETECTION_SINGLE_REPORT

/* One Bluetooth name for all three channels. The radio module rewrites the names only when this ID
 * differs from the one it holds (factory: 0x002D), so every name change needs a new ID. */
#undef USER_BlE_ID
#undef USER_BlE1_NAME
#undef USER_BlE2_NAME
#undef USER_BlE3_NAME
#define USER_BlE_ID    (0X002E)
#define USER_BlE1_NAME "CIDOO QK61"
#define USER_BlE2_NAME "CIDOO QK61"
#define USER_BlE3_NAME "CIDOO QK61"
