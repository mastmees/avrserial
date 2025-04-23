#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <avr/interrupt.h>
#include "fifo.hpp"
#include "textoutput.hpp"

class SoftwareUART : public TextOutput
{
protected:
  volatile uint8_t rxstate,nbits,ibyte,sticks,txstate,obyte,txtick;
  FIFO<uint8_t,64> rxfifo;
  FIFO<uint8_t,64> txfifo;
  
public:
  SoftwareUART(void) : rxstate(0),txstate(0),txtick(0) { }
  
  virtual void writebit(uint8_t b) = 0;
  virtual uint8_t readbit() { return 1; };
  
  uint8_t rxcount() { return rxfifo.count(); }
  uint8_t txcount() { return txfifo.count(); }
  bool txfull() { return txfifo.full(); }
  
  bool send(uint8_t b)
  {
    cli();
    b=txfifo.push(b);
    sei();
    return b;
  }

  void put_char(uint8_t c)
  {
    while (txfull())
      ;
    send(c);
  }
    
  uint8_t receive()
  {
    uint8_t b;
    cli();
    b=rxfifo.pop();
    sei();
    return b;
  }

  // this needs to run at 8x bitrate
  void run()
  {
    uint8_t bit;
    txtick++;
    if (!(txtick&7)) {
      switch (txstate) {
        default:
          txstate=0;
          // fall through
        case 0:
          if (txfifo.count()) {
            obyte=txfifo.pop();
            txstate=1;
          }
          else {
            break;
          }
          // fall through
        case 1:
          writebit(0); // start bit
          txstate++;
          break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
          writebit(obyte&1);
          obyte>>=1;
          txstate++;
          break;
        case 10: // stop bit
          writebit(1);
          txstate=0;
          break;
      }
    } 
    bit=readbit();
    switch (rxstate) {
      case 0: // waiting for start bit
        if (!bit)
          rxstate++;
        break;
      case 1: // wait for half of bit time to get in middle of bit
      case 2:
      case 3:
        if (bit)
          rxstate=0;
        else
          rxstate++;
        break;
      case 4:
        if (bit)
          rxstate=0;
        else {
          rxstate++;
          nbits=9;
          sticks=0;
        }
        // fall through
      case 5:
        if (!sticks)
        {
          sticks=8;
          ibyte>>=1;
          if (bit)
            ibyte|=0x80;
          nbits--;
          if (!nbits)
            rxstate++;
        }
        sticks--;
        break;
      case 6:
        if (!sticks)
          rxstate++;
        else
          sticks--;
        break;
      case 7:
        rxstate=0;
        if (bit) {
          rxfifo.push(ibyte);
        }
        break;          
    }
  }
  
};

class UART : public TextOutput
{
protected:
  FIFO<uint8_t,64> rxfifo;
  FIFO<uint8_t,64> txfifo;
public:
  uint8_t rxcount() { return rxfifo.count(); }
  uint8_t txcount() { return txfifo.count(); }
  bool txfull() { return txfifo.full(); }

  bool send(uint8_t b)
  {
    cli();
    b=txfifo.push(b);
    sei();
    return b;
  }

  void put_char(uint8_t c)
  {
    while (txfull())
      ;
    send(c);
  }
  
  uint8_t receive()
  {
    uint8_t b;
    cli();
    b=rxfifo.pop();
    sei();
    return b;
  }

  // meant to be run from interrupt handler, with interrupts disabled
  void run(void)
  {
    if (txcount() && (UCSR0A&(1<<UDRE0))) {
      UDR0=txfifo.pop();
    }
  }

  // meant to be run from interrupt handler, with interrupts disabled
  void byte_received(uint8_t c)
  {
    rxfifo.push(c);
  }

};
