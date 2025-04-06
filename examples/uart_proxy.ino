#include <avrserial.hpp>

#define MODEM_TX 6
#define MODEM_RX 5

UART uart;

// create inherited class to implement I/O on required GPIO pins
//
class : public SoftwareUART
{
   void writebit(uint8_t b) { digitalWrite(MODEM_TX,b); }
   uint8_t readbit() { return digitalRead(MODEM_RX); }
} modem;

// timer1 interrupts at 9600Hz, 8 times the 1200bps that software uart is running on
//
ISR(TIMER1_OVF_vect)
{
  TCNT1=0xffff-1600;
  modem.run();
  uart.run();
}

ISR(USART_RX_vect)
{
  uart.byte_received(UDR0);
}

void setup()
{
  pinMode(MODEM_TX,OUTPUT);
  pinMode(MODEM_RX,INPUT);

  // configure timer1 to interrupt at 1200*8=9600Hz for
  // software serial port
  // 16000000 / 9600 = 1667, with interrupt handler overhead
  // manual tuning resulted in 1600 as correct value
  TCCR1A=0x00;
  TCCR1B=0x01; // no prescaling
  TCNT1=0xffff-1600;
  TIMSK1=1;

  // configure hardware UART for 9600 bps
  UBRR0H=((F_CPU/(16UL*9600))-1)>>8;
  UBRR0L=((F_CPU/(16UL*9600))-1)&0xff;
  UCSR0B=(1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); // enable rx,tx, enable rx int

  sei();
}

// with hardware UART running at 9600 bps and sofware UART running at 1200
// bps, we only have to worry about not overrunning sofware one
//
void loop() {
    while (!modem.txfull() && uart.rxcount())
      modem.send(uart.receive());
    while (modem.rxcount())
      uart.send(modem.receive());
}
