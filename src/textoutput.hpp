#pragma once
#include <stdint.h>
#include <stdarg.h>

struct TextOutput
{
  virtual void put_char(uint8_t c) = 0;

  void puts(const char *s);
  void puts_P(const char *s);
  void putn(int32_t n);
  void putx(uint8_t n);
  void putxw(uint16_t n);
  void printf(const char *fmt,...);
  void printf_P(const char *fmt,...);
};
