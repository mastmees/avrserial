#include <avr/pgmspace.h>
#include "textoutput.hpp"

void TextOutput::puts(const char *s)
{
  while (s && *s) {
    put_char(*s++);
  }
}

void TextOutput::puts_P(const char *s)
{
  uint8_t c;
  while (s) {
    c=pgm_read_byte(s++);
    if (!c)
      break;
    put_char(c);
  }
}

void TextOutput::putn(int16_t n)
{
  if (n<0) {
    put_char('-');
    n=0-n;
  }
  if (n>9)
    putn(n/10);
  put_char((n%10)+'0');
}

void TextOutput::putx(uint8_t n)
{
  n=(n&15)+'0';
  put_char(n>'9'?n+7:n);
}

void TextOutput::putxw(uint16_t n)
{
  putx(n>>12);
  putx(n>>8);
  putx(n>>4);
  putx(n);
}

void TextOutput::printf(const char *fmt,...)
{
  va_list args;
  va_start(args,fmt);
  uint8_t c;
  while (fmt && *fmt) {
    c=*fmt++;
    if (c=='%' && *fmt) {
      c=*fmt++;
      switch (c) {
        case 'd':
          putn(va_arg(args,uint16_t));
          break;
        case 's':
          puts(va_arg(args,const char*));
          break;
        case 'c':
          put_char(va_arg(args,uint16_t));
          break;
        case 'x':
          putxw(va_arg(args,uint16_t));
          break;
        default:
          put_char(c);
          break;
      }
    }
    else {
      put_char(c);
    }
  }
  va_end(args);
}

void TextOutput::printf_P(const char *fmt,...)
{
  va_list args;
  va_start(args,fmt);
  uint8_t c;
  while (fmt && (c=pgm_read_byte(fmt))) {
    fmt++;
    if (c=='%') {
      c=pgm_read_byte(fmt);
      switch (c) {
        case 'd':
          putn(va_arg(args,uint16_t));
          break;
        case 's':
          puts(va_arg(args,const char*));
          break;
        case 'c':
          put_char(va_arg(args,uint16_t));
          break;
        case 'x':
          putxw(va_arg(args,uint16_t));
          break;
        default:
          put_char(c);
          break;
      }
      fmt++;
    }
    else {
      put_char(c);
    }
  }
  va_end(args);
}
