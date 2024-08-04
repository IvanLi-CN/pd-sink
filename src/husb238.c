
#include "husb238.h"

/** Get Source PDO */
u8 husb238_get_src_pdo(HUSB238_SrcPdo* src_pdo) {
  u8 err = i2c_recv(HUSB238_ADDR, HUSB238_REGISTER_SRC_PDO, (u8*)src_pdo, 1);

  *src_pdo = *src_pdo & HUSB238_SRC_PDO_MASK;
  return err;
}

/** Set Source PDO */
u8 husb238_set_src_pdo(HUSB238_SrcPdo src_pdo) {
  u8 buf[2] = {HUSB238_REGISTER_SRC_PDO, src_pdo};
  return i2c_send(HUSB238_ADDR, buf, 2);
}

u8 husb238_go_command(HUSB238_Command command) {
  u8 buf[2] = {HUSB238_REGISTER_GO_COMMAND, command};
  return i2c_send(HUSB238_ADDR, buf, 2);
}