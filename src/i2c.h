#ifndef _APP_I2C_H
#define _APP_I2C_H

#include <stdio.h>

#include "ch32v003fun.h"

#define I2C_CLKRATE 100000
#define I2C_PRERATE 2000000

#define I2C_TIMEOUT_MAX 100000

#define I2C_ERROR_TIMEOUT 1
#define I2C_ERROR_WAIT_FOR_RX_TIMEOUT 2
#define I2C_ERROR_BUSY 3

void init_i2c();
u8 i2c_send(u8 addr, u8 *data, u8 size);
u8 i2c_recv(u8 addr, u8 reg, u8 *data, u8 size);

#endif