#ifndef HUSB238_H
#define HUSB238_H

#include "ch32v003fun.h"
#include "i2c.h"

#define HUSB238_ADDR 0x08
#define HUSB238_REGISTER_PD_STATUS_0 0x00
#define HUSB238_REGISTER_PD_STATUS_1 0x01
#define HUSB238_REGISTER_SRC_PDO_5V 0x02
#define HUSB238_REGISTER_SRC_PDO_9V 0x03
#define HUSB238_REGISTER_SRC_PDO_12V 0x04
#define HUSB238_REGISTER_SRC_PDO_15V 0x05
#define HUSB238_REGISTER_SRC_PDO_18V 0x06
#define HUSB238_REGISTER_SRC_PDO_20V 0x07
#define HUSB238_REGISTER_SRC_PDO 0x08
#define HUSB238_REGISTER_GO_COMMAND 0x09

typedef enum {
  HUSB238_CURRENT_0_5A = 0x00,
  HUSB238_CURRENT_0_7A = 0x01,
  HUSB238_CURRENT_1_0A = 0x02,
  HUSB238_CURRENT_1_25A = 0x03,
  HUSB238_CURRENT_1_5A = 0x04,
  HUSB238_CURRENT_1_75A = 0x05,
  HUSB238_CURRENT_2_0A = 0x06,
  HUSB238_CURRENT_2_25A = 0x07,
  HUSB238_CURRENT_2_5A = 0x08,
  HUSB238_CURRENT_2_75A = 0x09,
  HUSB238_CURRENT_3_0A = 0x0A,
  HUSB238_CURRENT_3_25A = 0x0B,
  HUSB238_CURRENT_3_5A = 0x0C,
  HUSB238_CURRENT_4_0A = 0x0D,
  HUSB238_CURRENT_4_5A = 0x0E,
  HUSB238_CURRENT_5_0A = 0x0F
} HUSB238_Current;

typedef enum {
  HUSB238_SRC_PDO_5V = 0x10,
  HUSB238_SRC_PDO_9V = 0x20,
  HUSB238_SRC_PDO_12V = 0x30,
  HUSB238_SRC_PDO_15V = 0x80,
  HUSB238_SRC_PDO_18V = 0x90,
  HUSB238_SRC_PDO_20V = 0x10
} HUSB238_SrcPdo;
#define HUSB238_SRC_PDO_MASK 0xF0

typedef enum {
  HUSB238_COMMAND_REQUEST = 0x01,
  HUSB238_COMMAND_GET_SRC_CAP = 0x04,
  HUSB238_COMMAND_HARD_RESET = 0x10,
} HUSB238_Command;

// u8 husb238_get_status_0(void);
// u8 husb238_get_status_1(void);
// u8 husb238_get_src_pdo_5v(void);
// u8 husb238_get_src_pdo_9v(void);
// u8 husb238_get_src_pdo_12v(void);
// u8 husb238_get_src_pdo_15v(void);
// u8 husb238_get_src_pdo_18v(void);
// u8 husb238_get_src_pdo_20v(void);
// u8 husb238_get_src_pdo(void);

u8 husb238_get_src_pdo(HUSB238_SrcPdo* src_pdo);
u8 husb238_set_src_pdo(HUSB238_SrcPdo src_pdo);

u8 husb238_go_command(HUSB238_Command command);

#endif