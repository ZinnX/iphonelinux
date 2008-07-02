#ifndef HW_I2C_H
#define HW_I2C_H

#include "hardware/s5l8900.h"

// Device
#define I2C0 0x3C600000
#define I2C1 0x3C900000

// Registers
#define IICCON 0x0
#define IICSTAT 0x4
#define IICADD 0x8
#define IICDS 0xC
#define IICLC 0x10
#define IICREG14 0x14
#define IICREG18 0x18
#define IICREG1C 0x1C
#define IICREG20 0x20

// Values
#define IICCON_INIT 0x3F00
#define IICCON_ACKGEN (1 << 7)
#define IICCON_TXCLKSRC_FPCLK16 (0 << 6)
#define IICCON_TXCLKSRC_FPCLK512 (1 << 6)
#define IICCON_INTENABLE (1 << 5)
#define IICCON_INTPENDING (1 << 4)
#define IICCON_TXCLKPRESCALEMASK 0xF

#define I2C0_SCL_GPIO 0xB02
#define I2C0_SDA_GPIO 0xB03
#define I2C1_SCL_GPIO 0x801
#define I2C1_SDA_GPIO 0x802

#define I2C0_CLOCKGATE 0x24
#define I2C1_CLOCKGATE 0x26

#define TXFREQ 25000

#endif

