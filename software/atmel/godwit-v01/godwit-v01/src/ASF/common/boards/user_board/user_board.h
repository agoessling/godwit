/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// CPU FREQ
#define CPU_FREQ	25000000UL
#define PBA_FREQ	CPU_FREQ
#define PBB_FREQ	CPU_FREQ
#define PBC_FREQ	CPU_FREQ
// DFLL FREQ
#define DFLL_FREQ	100000000UL


#endif // USER_BOARD_H
