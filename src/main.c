/* Template app on which you can build your own. */

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "ch32v003fun.h"
#include "husb238.h"
#include "i2c.h"
#include "ina226.h"
#include "st7735.h"

u8 float_to_string(float num, char *str, u8 afterpoint);

int main() {
  u8 err = 0;
  HUSB238_SrcPdo src_pdo = 0;

  SystemInit();

  funGpioInitAll();
  funPinMode(PD2, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);

  init_i2c();

  tft_init();
  err = ina226_init();
  if (err) {
    printf("Failed to init ina226\n\r");
    return 1;
  }

  printf("Hello World!\n\r");
  err = husb238_get_src_pdo(&src_pdo);
  if (err) {
    printf("Failed to get src_pdo\n\r");
    return 1;
  }
  printf("src_pdo: %d\n\r", src_pdo);

  err = husb238_set_src_pdo(HUSB238_SRC_PDO_12V);
  if (err) {
    printf("Failed to set src_pdo\n\r");
    return 1;
  }

  err = husb238_go_command(HUSB238_COMMAND_REQUEST);
  if (err) {
    printf("Failed to go command\n\r");
    return 1;
  }
  funDigitalWrite(PD2, FUN_HIGH);

  uint32_t frame = 0;
  u8 str_len = 0;

  tft_fill_rect(0, 0, 160, 80, BLACK);
  tft_set_color(RED);
  tft_set_background_color(BLACK);

  char tmp_str[32] = {0};

  while (1) {
    tft_set_color(WHITE);

    tft_set_cursor(5, 1);
    tft_print_number(frame++, 4);

    float current_volts = 0;
    err = ina226_read_voltage(&current_volts);
    if (err) {
      printf("Failed to read voltage\n\r");
      return 1;
    }
    tft_set_cursor(5, 20);
    str_len = float_to_string(current_volts, tmp_str, 6);
    tmp_str[str_len] = 'V';
    tmp_str[str_len + 1] = 0;
    tft_print(tmp_str);

    float current_amps = 0;
    err = ina226_read_current(&current_amps);
    if (err) {
      printf("Failed to read current\n\r");
      return 1;
    }
    str_len = float_to_string(-current_amps, tmp_str, 6);
    tmp_str[str_len] = 'A';
    tmp_str[str_len + 1] = 0;
    tft_set_cursor(5, 35);
    tft_print(tmp_str);

    float current_watts = 0;
    err = ina226_read_power(&current_watts);
    if (err) {
      printf("Failed to read power\n\r");
      return 1;
    }
    str_len = float_to_string(current_watts, tmp_str, 6);
    tmp_str[str_len] = 'W';
    tmp_str[str_len + 1] = 0;
    tft_set_cursor(5, 50);
    tft_print(tmp_str);

    // float current_shunt_millivolts = 0;
    // err = ina226_shunt_voltage(&current_shunt_millivolts);
    // if (err) {
    //   printf("Failed to read shunt voltage\n\r");
    //   return 1;
    // }
    // tft_set_cursor(5, 65);
    // str_len = float_to_string(-current_shunt_millivolts, tmp_str, 6);
    // tmp_str[str_len] = 'm';
    // tmp_str[str_len + 1] = 'V';
    // tmp_str[str_len + 2] = 0;
    // tft_print(tmp_str);

    // Delay_Ms(1000);
  }
}

u8 float_to_string(float num, char *str, u8 afterpoint) {
  int ipart = (int)num;
  float fpart = num - (float)ipart;
  u8 i = 0;

  // 处理整数部分
  if (ipart == 0)
    str[i++] = '0';
  else {
    int temp = ipart;
    while (temp != 0) {
      str[i++] = (temp % 10) + '0';
      temp = temp / 10;
    }
    int j, k;
    for (j = 0, k = i - 1; j < k; j++, k--) {
      char tmp = str[j];
      str[j] = str[k];
      str[k] = tmp;
    }
  }

  if (afterpoint != 0) {
    str[i] = '.';
    i++;

    fpart = fpart * pow(10, afterpoint);
    int fpart_int = (int)(fpart + 0.5);
    for (int j = 0; j < afterpoint; j++) {
      str[i + afterpoint - 1 - j] = (fpart_int % 10) + '0';
      fpart_int /= 10;
    }
    i += afterpoint;
  }

  str[i] = '\0';

  return i;
}