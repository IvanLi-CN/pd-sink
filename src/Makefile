all : flash

TARGET:=main

ADDITIONAL_C_FILES+=i2c.c
ADDITIONAL_C_FILES+=ina226.c
ADDITIONAL_C_FILES+=husb238.c
ADDITIONAL_C_FILES+=st7735.c

CH32V003FUN:=../ch32v003fun/ch32v003fun

include ../ch32v003fun/ch32v003fun/ch32v003fun.mk

flash : cv_flash
clean : cv_clean

