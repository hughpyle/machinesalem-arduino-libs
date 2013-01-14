/*
  TLV5618.h
  
  Arduino library for Texas Instruments TLV5618 2-channel 12-bit SPI DAC
  
  External voltage reference; full-scale is 2*vref
  Values here are uint16, valid from 0 to 4096.
  
  To use:
    include <SPI.h> in your sketch before this library.
    Construct,
    call begin() in your setup(),
    call write().
  
  Wiring:
    Use any digital pin for !CS (chip select).
    Data and clock use hardware SPI connections.
      For "classic" Arduinos (Uno, Duemilanove, etc.), data = pin 11, clock = pin 13
      For Teensy 2.0, data = B2 (#2), clock = B1 (#1)
      For Teensy 3.0, data = 11 (DOUT), clock = 13 (SCK)
    
  2012-09-29 @machinesalem,  (cc) https://creativecommons.org/licenses/by/3.0/
*/

#ifndef __TLV5618_H__
#define __TLV5618_H__

#include "Arduino.h"

/* Optionally you can pass these control modes to the constructor.  Default is "slow" (10uS settling) */
#define TLV5618_SPEED_SLOW  0x00
#define TLV5618_SPEED_FAST  0x40
#define TLV5618_POWER_NORM  0x00
#define TLV5618_POWER_DOWN  0x20

/* Raw commands */
/* Write the value to channel B and to the buffer */
#define TLV5618_CMD_WRITE_B_AND_BUFFER 0x00
/* Write the value to the buffer only */
#define TLV5618_CMD_WRITE_BUFFER       0x10
/* Write the value to channel A, and update channel B from the buffer */
#define TLV5618_CMD_WRITE_A_UPDATE_B   0x80


class TLV5618
{
  private:
    uint8_t _cs_pin;
    uint8_t _control;
    
  public:
    TLV5618( uint8_t cs_pin );
    TLV5618( uint8_t cs_pin, uint8_t control );

    void begin();
    
    // Direct write methods
    void select( int b );
    inline void write_data_no_cs( uint8_t cmd, uint16_t value ) { SPI.transfer( ((value & 0xF00)>>8) | cmd | _control ); SPI.transfer(   value & 0xFF ); };
    void write_data( uint8_t cmd, uint16_t value );
    void write_fast( uint8_t cmd, uint16_t value );
    
    // Convenience write both channels
    void write( uint16_t valueA, uint16_t valueB );
};

#endif
