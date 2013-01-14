/*
 * Library for Wolfson WM8731 codec control interface
 *
 * NOTE: This library only controls the codec operation.  It doesn't have a data interface!  You need to do that yourself (e.g. with I2S).
 *
 * Requires the Wire library.
 *
 * Physical connections:
 *      WM8731 "SDIN" (Proto board "SDA") to    SDA -- Teensy 3.0 pin 18 (A4)
 *      WM8731 "SCLK" (Proto board "SCL") to    SCL -- Teensy 3.0 pin 19 (A5)
 * NOTE: Teensy3.0 requires pullup resistors on these two pins -- e.g. 10k each, from pin to 3.3V.
 *
 * 2013-01-14 @machinesalem,  (cc) https://creativecommons.org/licenses/by/3.0/
 */

#include "Wire.h"
#include "WM8731.h"

static unsigned char WM8731_initialized=0;
static unsigned char WM8731_device_address=WM8731_DEVICE_ADDRESS_CSB_LOW;

void WM8731_class::begin( WM8731_csb device_address, unsigned int sample_rate_hz, unsigned char word_length_bits, WM8731_interface_format interface_format )
{
    WM8731_device_address = device_address;
    if( !WM8731_initialized )
    {
        WM8731_initialized = 1;
        Wire.begin();
        delay(200);
    }
    
    // Reset the codec
    reset();
       
    // Power on
    set( WM8731_POWERDOWN, 0 );

    // Set the digital data format
    unsigned char iwl = 0;
    if( word_length_bits==20 ) iwl = 1;
    else if( word_length_bits==24 ) iwl = 2;
    else if( word_length_bits==32 ) iwl = 3;
    set( WM8731_INTERFACE, WM8731_INTERFACE_FORMAT(interface_format) | WM8731_INTERFACE_IWL(iwl) );
    
    // Default volumes are all off
    set( WM8731_LLINEIN,  WM8731_LLINEIN_LINVOL(0) );
    set( WM8731_RLINEIN,  WM8731_RLINEIN_RINVOL(0) );
    set( WM8731_LHEADOUT, WM8731_LHEADOUT_LHPVOL(0) );
    set( WM8731_RHEADOUT, WM8731_RHEADOUT_RHPVOL(0) );
    set( WM8731_ANALOG, WM8731_ANALOG_DACSEL );
    set( WM8731_DIGITAL, 0 );
    
    // Default rate is 48k.  Assume a 12MHz mclk.
    unsigned char rate = 0x00;
    if( sample_rate_hz==8000 )         rate = 0x03;
    else if( sample_rate_hz==32000 )  rate = 0x06;
    else if( sample_rate_hz==96000 )  rate = 0x07;
    set( WM8731_SAMPLING, WM8731_SAMPLING_SR(rate) );

    //set( 0x10, 0xa0 );
}

void WM8731_class::reset()
{
    set( WM8731_RESET, 0 );
}               

void WM8731_class::setActive()
{
    set( WM8731_CONTROL, WM8731_CONTROL_ACTIVE );
    //set( 0x12, 1 );
}

void WM8731_class::setInactive()
{
    set( WM8731_CONTROL, 0 );
}

void WM8731_class::setInputVolume( unsigned char value )
{
    unsigned char reg;
    reg = ( WM8731.get(WM8731_LLINEIN) & WM8731_LLINEIN_LINVOL_MASK ) | WM8731_LLINEIN_LINVOL(value);
    set( WM8731_LLINEIN, reg );
    reg = ( WM8731.get(WM8731_RLINEIN) & WM8731_RLINEIN_RINVOL_MASK ) | WM8731_RLINEIN_RINVOL(value);
    set( WM8731_RLINEIN, reg );
}

void WM8731_class::setOutputVolume( unsigned char value )
{
    unsigned char reg;
    reg = ( WM8731.get(WM8731_LHEADOUT) & WM8731_LHEADOUT_LHPVOL_MASK ) | WM8731_LHEADOUT_LHPVOL(value) /* | WM8731_LHEADOUT_LZCEN */;
    set( WM8731_LHEADOUT, reg );
    reg = ( WM8731.get(WM8731_RHEADOUT) & WM8731_RHEADOUT_RHPVOL_MASK ) | WM8731_RHEADOUT_RHPVOL(value) /* | WM8731_LHEADOUT_RZCEN */;
    set( WM8731_RHEADOUT, reg );
}

void WM8731_class::set( unsigned char reg, unsigned short value )
{
    if( reg < WM8731_NREGISTERS )
        WM8731_registers[reg] = value;
        
#ifdef WM8731_DEBUG
    Serial.print( reg, HEX );
    Serial.print( "=" );
    Serial.println( value, HEX );
#endif

    Wire.beginTransmission(WM8731_device_address);
    Wire.send( (unsigned char)((reg<<1) | ((value>>8) & 0x1)) );
    Wire.send( (unsigned char)(value & 0xFF) );
    Wire.endTransmission();
}

