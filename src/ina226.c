#include "ina226.h"

#include "i2c.h"

float ina226_calibration = 0;
float ina226_current_lsb = 0;
u8 buff[3] = {0};

u8 ina226_init(void) {
  u8 err;

  ina226_current_lsb = (float)INA226_BUS_MAX_CURRENT_AMPERE / (float)(1 << 15);
  float shunt_resistor_ohm = (float)INA226_SHUNT_RESISTOR_MILLI_OHM / 1000.0;
  ina226_calibration = 0.00512 / (ina226_current_lsb * shunt_resistor_ohm);

  buff[0] = INA226_REGISTER_CONFIG;
  buff[1] = INA226_CONFIG >> 8;
  buff[2] = INA226_CONFIG & 0xff;
  printf("ina226_config: %02x\n", buff[1]);

  err = i2c_send(INA226_I2C_ADDR, buff, 2);

  if (err) return err | 0x80;

  buff[0] = INA226_REGISTER_CALIBRATION;
  buff[1] = (uint16_t)ina226_calibration >> 8;
  buff[2] = (uint16_t)ina226_calibration & 0xff;
  err = i2c_send(INA226_I2C_ADDR, buff, 3);
  printf("ina226_current_lsb: %ld\n",
         (int32_t)(ina226_current_lsb * 1000.0 * 1000.0));

  return err;
}

u8 ina226_shunt_voltage(float *voltage) {
  u8 err;

  err = i2c_recv(INA226_I2C_ADDR, INA226_REGISTER_SHUNT_VOLTAGE, buff, 2);
  int16_t raw = buff[0] << 8 | buff[1];

  printf("ina226_shunt_voltage: %d\n", raw);
  *voltage = (float)raw * 2.5;

  return err;
}

u8 ina226_read_voltage(float *voltage) {
  u8 err;

  err = i2c_recv(INA226_I2C_ADDR, INA226_REGISTER_BUS_VOLTAGE, buff, 2);
  int16_t raw = buff[0] << 8 | buff[1];

  //   printf("ina226_read_voltage: %d\n", raw);
  *voltage = (float)raw * 1.25 / 1000.0;

  return err;
}

u8 ina226_read_current(float *current) {
  u8 err;

  err = i2c_recv(INA226_I2C_ADDR, INA226_REGISTER_CURRENT, buff, 2);
  int16_t raw = buff[0] << 8 | buff[1];

  printf("ina226_read_current: %d\n", raw);
  *current = (float)raw * ina226_current_lsb;

  return err;
}

u8 ina226_read_power(float *power) {
  u8 err;

  err = i2c_recv(INA226_I2C_ADDR, INA226_REGISTER_POWER, buff, 2);
  int16_t raw = buff[0] << 8 | buff[1];

  printf("ina226_read_power: %d\n", raw);
  *power = (float)raw * ina226_current_lsb * 25.0;

  return err;
}