#pragma once

#include <stdint.h>
#include <stdarg.h>

template<typename T,uint8_t size=16> class FIFO
{
  volatile T buf[size];
  volatile uint8_t head;
  volatile uint8_t tail;
  volatile uint8_t _count;

public:
  FIFO() : head(0),tail(0),_count(0) {}
  
  uint8_t count() { return _count; }

  T pop()
  {
    T b=(T)0;
    if (_count) {
      b=buf[tail++];
      if (tail>=(sizeof(buf)/sizeof(buf[0])))
        tail=0;
      _count--;
    }
    return b;
  }
  
  bool push(T b)
  {
    if (_count<(sizeof(buf)/sizeof(buf[0]))) {
      buf[head++]=b;
      if (head>=(sizeof(buf)/sizeof(buf[0])))
        head=0;
      _count++;
      return true;
    }
    return false;
  }
  
  T peek() { return buf[tail]; }

  bool full() { return _count>=(sizeof(buf)/sizeof(buf[0])); }

  bool contains(T e)
  {
    uint8_t i=tail;
    uint8_t c=_count;
    while (c--) {
      if (buf[i++]==e)
        return true;
      if (i>=(sizeof(buf)/sizeof(buf[0])))
        i=0;
    }
    return false;
  }
  
};
