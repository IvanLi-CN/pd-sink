/* Template app on which you can build your own. */

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "ch32v003fun.h"
#include "husb238.h"
#include "i2c.h"
#include "ina226.h"
#include "st7735.h"

#define LONG_PRESS_TIME_MS 500
#define PRESS_THRESHOLD_MS 10
// seconds * 24MHz = ticks
#define LONG_PRESS_TIME_TICKS \
  ((uint32_t)LONG_PRESS_TIME_MS * (uint32_t)24 * (uint32_t)1000)
#define PRESS_THRESHOLD_TICKS \
  ((uint32_t)PRESS_THRESHOLD_MS * (uint32_t)24 * (uint32_t)1000)

#define BTN_A_RELEASED() (btn_a_press_start == 0)
#define BTN_B_RELEASED() (btn_b_press_start == 0)

typedef enum {
  btn_a = 0,
  btn_b,
} btn_t;

u8 float_to_string(float num, char *str, u8 afterpoint);
u8 request_src_pdo(HUSB238_SrcPdo src_pdo);
void EXTI7_0_IRQHandler(void) __attribute__((interrupt));
void handle_btn_state_change(btn_t btn);

uint32_t btn_a_press_start = 0;
uint32_t btn_b_press_start = 0;
uint32_t btn_a_press_duration = 0;
uint32_t btn_b_press_duration = 0;

void EXTI7_0_IRQHandler(void) {
  printf("GPIOA->INDR: %02lx\n", GPIOA->INDR);
  if (EXTI->INTFR & EXTI_Line1) {
    if ((GPIOA->INDR & GPIO_Pin_1) == 0) {
      // Btn A is pressed
      btn_a_press_start = SysTick->CNT;
    } else {
      // Btn A is released
      btn_a_press_duration = SysTick->CNT - btn_a_press_start;
      btn_a_press_start = 0;
    }
    handle_btn_state_change(btn_a);
    // Acknowledge the interrupt
    EXTI->INTFR = EXTI_Line1;
  } else if (EXTI->INTFR & EXTI_Line2) {
    if ((GPIOA->INDR & GPIO_Pin_2) == 0) {
      // Btn B is pressed
      btn_b_press_start = SysTick->CNT;
    } else {
      // Btn B is released
      btn_b_press_duration = SysTick->CNT - btn_b_press_start;
      btn_b_press_start = 0;
    }
    handle_btn_state_change(btn_b);
    // Acknowledge the interrupt
    EXTI->INTFR = EXTI_Line2;
  }
}

void handle_btn_state_change(btn_t btn) {
  if (btn == btn_a && BTN_A_RELEASED()) {
    if (btn_a_press_duration > LONG_PRESS_TIME_TICKS) {
      printf("Btn A long pressed for %lu ms\n", btn_a_press_duration / 24000);
    } else if (btn_a_press_duration > PRESS_THRESHOLD_TICKS) {
      printf("Btn A short pressed for %lu ms\n", btn_a_press_duration / 24000);
    }
  } else if (btn == btn_b && BTN_B_RELEASED()) {
    if (btn_b_press_duration > LONG_PRESS_TIME_TICKS) {
      printf("Btn B long pressed for %lu ms\n", btn_b_press_duration / 24000);
    } else if (btn_b_press_duration > PRESS_THRESHOLD_TICKS) {
      printf("Btn B short pressed for %lu ms\n", btn_b_press_duration / 24000);
    }
  } else {
    // printf("nothing to do. btn: %d, btn_a: %d, btn_b: %d\n", btn,
    //        BTN_A_RELEASED(), BTN_B_RELEASED());
  }
}

u8 loop(void) {
  u8 err = 0;
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
  }
}

int main() {
  u8 err = 0;
  HUSB238_SrcPdo real_src_pdo = 0;
  HUSB238_SrcPdo target_src_pdo = 0;

  SystemInit();

  printf("LONG_PRESS_TIME_TICKS: %lu\n", LONG_PRESS_TIME_TICKS);

  funGpioInitAll();

  // PD2 is Output Control Pin
  funPinMode(PD2, GPIO_Speed_In | GPIO_CNF_OUT_PP);

  // PA1 is Btn A
  funPinMode(PA1, GPIO_Speed_In | GPIO_CNF_IN_PUPD);
  GPIOA->OUTDR |= GPIO_Pin_1;
  // PA2 is Btn B
  funPinMode(PA2, GPIO_Speed_In | GPIO_CNF_IN_PUPD);
  GPIOA->OUTDR |= GPIO_Pin_2;

  // External interrupt for PA1 and PA2
  AFIO->EXTICR = AFIO_EXTICR_EXTI1_PA | AFIO_EXTICR_EXTI2_PA;
  EXTI->INTENR = EXTI_INTENR_MR1 | EXTI_INTENR_MR2;
  EXTI->RTENR = EXTI_RTENR_TR1 | EXTI_RTENR_TR2;
  EXTI->FTENR = EXTI_FTENR_TR1 | EXTI_FTENR_TR2;
  NVIC_EnableIRQ(EXTI7_0_IRQn);

  // Init SysTick
  SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STRE | SYSTICK_CTLR_STCLK;

  init_i2c();

  tft_init();
  err = ina226_init();
  if (err) {
    printf("Failed to init ina226\n\r");
    return 1;
  }

  printf("Hello World!\n\r");
  err = husb238_get_src_pdo(&real_src_pdo);
  if (err) {
    printf("Failed to get src_pdo\n\r");
    return 1;
  }
  printf("src_pdo: %d\n\r", real_src_pdo);

  funDigitalWrite(PD2, FUN_HIGH);

  while (1) {
    loop();

    Delay_Ms(1000);
    init_i2c();
  }
}

u8 request_src_pdo(HUSB238_SrcPdo src_pdo) {
  u8 err = husb238_set_src_pdo(HUSB238_SRC_PDO_12V);
  if (err) {
    printf("Failed to set src_pdo\n\r");
    return 1;
  }

  err = husb238_go_command(HUSB238_COMMAND_REQUEST);
  if (err) {
    printf("Failed to go command\n\r");
    return 2;
  }

  return 0;
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