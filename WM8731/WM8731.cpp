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


/*
 * Pins for the Mikro proto board
 *  SCK  = bit-clock (out when master)
 *  DACL = word-clock for DAC (out when master)
 *  ADCL = word-clock for ADC (out when master)
 *  MOSI = data for DAC
 *  MISO = data for ADC
 *  SDA  = I2C control data
 *  SCK  = I2C control clock
 */

#include "Wire.h"
#include "WM8731.h"

static unsigned char WM8731_initialized=0;
static unsigned char WM8731_device_address=WM8731_DEVICE_ADDRESS_CSB_LOW;

/*
 * @brief Initialize the WM8731 codec.
 * @param[in]   device_address    Either "low" (hex 1A) or "high" (hex 1B), depending whether the CSB pin is wired low or high.
 * @param[in]   sampling_flags    Combination of flags to define the sample rate etc.  For example WM8731_SAMPLING_RATE(hz48000)
 * @param[in]   interface_flags   Combination of flags to define the data interface.  For example WM8731_INTERFACE_FORMAT(I2S) | WM8731_INTERFACE_MASTER
 * @return none.
 */
void WM8731_class::begin( WM8731_csb device_address, unsigned char sampling_flags, unsigned char interface_flags )
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
       
    // Power on all modules
    set( WM8731_POWERDOWN, 0 );

    // Set the digital data format
    set( WM8731_INTERFACE, interface_flags );
    
    // Default volumes are all off
    set( WM8731_LLINEIN,  WM8731_LLINEIN_LINVOL(0) );
    set( WM8731_RLINEIN,  WM8731_RLINEIN_RINVOL(0) );
    set( WM8731_LHEADOUT, WM8731_LHEADOUT_LHPVOL(0) );
    set( WM8731_RHEADOUT, WM8731_RHEADOUT_RHPVOL(0) );
    set( WM8731_ANALOG,   WM8731_ANALOG_DACSEL );
    set( WM8731_DIGITAL, 0 );
    
    set( WM8731_SAMPLING, sampling_flags );

    //set( 0x10, 0xa0 );
}

/*
 * @brief Reset the codec.  (This is done automatically on 'begin')
 * @return none.
 */
void WM8731_class::reset()
{
    set( WM8731_RESET, 0 );
}               

/*
 * @brief Makes the codec active.  (This is NOT done automatically on 'begin').
 * @return none.
 */
void WM8731_class::setActive()
{
    set( WM8731_CONTROL, WM8731_CONTROL_ACTIVE );
    //set( 0x12, 1 );
}

/*
 * @brief Makes the codec inactive.  (Its functions are powered on still).
 * @return none.
 */
void WM8731_class::setInactive()
{
    set( WM8731_CONTROL, 0 );
}

/*
 * @brief Sets the input gain on both line-input channels.
 * @param[in]   value       Volume, 0 to 31
 * @return none.
 */
void WM8731_class::setInputVolume( unsigned char value )
{
    unsigned char reg;
    reg = ( WM8731.get(WM8731_LLINEIN) & WM8731_LLINEIN_LINVOL_MASK ) | WM8731_LLINEIN_LINVOL(value);
    set( WM8731_LLINEIN, reg );
    reg = ( WM8731.get(WM8731_RLINEIN) & WM8731_RLINEIN_RINVOL_MASK ) | WM8731_RLINEIN_RINVOL(value);
    set( WM8731_RLINEIN, reg );
}

/*
 * @brief Sets the output volume both channels.
 * @param[in]   value       Volume, 0 to 127
 * @return none.
 */
void WM8731_class::setOutputVolume( unsigned char value )
{
    unsigned char reg;
    reg = ( WM8731.get(WM8731_LHEADOUT) & WM8731_LHEADOUT_LHPVOL_MASK ) | WM8731_LHEADOUT_LHPVOL(value) /* | WM8731_LHEADOUT_LZCEN */;
    set( WM8731_LHEADOUT, reg );
    reg = ( WM8731.get(WM8731_RHEADOUT) & WM8731_RHEADOUT_RHPVOL_MASK ) | WM8731_RHEADOUT_RHPVOL(value) /* | WM8731_LHEADOUT_RZCEN */;
    set( WM8731_RHEADOUT, reg );
}

/*
 * @brief Sets any parameter.
 * @param[in]   reg         The register
 * @param[in]   value       The value to write into the register
 * @return none.
 */
void WM8731_class::set( unsigned char reg, unsigned short value )
{
    if( reg < WM8731_NREGISTERS )
        WM8731_registers[reg] = value;
        
#ifdef WM8731_DEBUG
    Serial.print( "WM8731 register 0x" );
    Serial.print( reg, HEX );
    Serial.print( " = 0x" );
    Serial.println( value, HEX );
#endif

    Wire.beginTransmission(WM8731_device_address);
    Wire.send( (unsigned char)((reg<<1) | ((value>>8) & 0x1)) );
    Wire.send( (unsigned char)(value & 0xFF) );
    Wire.endTransmission();
}

