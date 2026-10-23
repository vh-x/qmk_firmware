// Copyright 2026 vh-x
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// Report the detected host OS once per USB connection
#define OS_DETECTION_SINGLE_REPORT

// Force 6KRO whenever the board is in Mac mode (saved NKRO preference still applies in Windows mode)
#define NKRO_OFF_IN_MAC_MODE
