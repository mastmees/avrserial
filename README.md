# avrserial

Software and hardware serial port library for AVR based Arduinos.

The main focus of this library is software serial port implementation that
requires subclassing the SoftwareUART class to implement simple bit read and
write methods for I/O, and calling one function at 1/8 of a bit time
interval. Multiple ports can be created this way. You must implement
bit transmit function, receive functionality is optional. the run() method
is designed to be called from timer interrupt handler.

Hardware UART class is provided for completeness, with same basic interface,
except that it provides a method for buffering bytes from rx interrupt
handler.

Both classes rely on generic FIFO class template, and by default provide 16
byte transmit and receive buffers.
