/*
 * Library for Wolfson WM8731 codec control interface
 *
 * NOTE: This library only controls the codec operation.  It doesn't have a data interface!  You need to do that yourself (e.g. with I2S).
 *
 * Requires the Wire library.
 *
 * 2013-01-14 @machinesalem,  (cc) https://creativecommons.org/licenses/by/3.0/
 */

#ifndef __WM8731_H__
#define __WM8731_H__
 
#include "Arduino.h"

/* The address of the codec is set by the state of the CSB pin */

#define WM8731_DEVICE_ADDRESS_CSB_LOW   0x1a    // 0011010
#define WM8731_DEVICE_ADDRESS_CSB_HIGH  0x1b    // 0011011

typedef enum {
      low = WM8731_DEVICE_ADDRESS_CSB_LOW,
      high = WM8731_DEVICE_ADDRESS_CSB_HIGH
    } WM8731_csb;

/* ----- Registers ----- */

#define WM8731_LLINEIN              ((unsigned char)0x00)
#define WM8731_RLINEIN              ((unsigned char)0x01)
#define WM8731_LHEADOUT             ((unsigned char)0x02)
#define WM8731_RHEADOUT             ((unsigned char)0x03)
#define WM8731_ANALOG               ((unsigned char)0x04)
#define WM8731_DIGITAL              ((unsigned char)0x0a)
#define WM8731_POWERDOWN            ((unsigned char)0x06)
#define WM8731_INTERFACE            ((unsigned char)0x07)
#define WM8731_SAMPLING             ((unsigned char)0x08)
#define WM8731_CONTROL              ((unsigned char)0x09)
#define WM8731_RESET                ((unsigned char)0x0F)      // writing 0 resets device

/* ----- Flags ----- */

#define WM8731_LLINEIN_LINVOL(n)    ((unsigned char)(n & 0x1f))       // Left channel line-input volume (0..31)
#define WM8731_LLINEIN_LINVOL_MASK  ((unsigned char)(0xe0))
#define WM8731_LLINEIN_LINMUTE      ((unsigned char)0x80)             // Left line input mute to ADC
#define WM8731_LLINEIN_LRINBOTH     ((unsigned short)0x100)           // Left to Right Mic Control Join

#define WM8731_RLINEIN_RINVOL(n)    ((unsigned char)(n & 0x1f))       // Right channel line-input volume (0..31)
#define WM8731_RLINEIN_RINVOL_MASK  ((unsigned char)(0xe0))
#define WM8731_RLINEIN_RINMUTE      ((unsigned char)0x80)             // Right line input mute to ADC
#define WM8731_RLINEIN_RLINBOTH     ((unsigned short)0x100)           // Right to Left Mic Control Join

#define WM8731_ANALOG_MICBOOST      ((unsigned char)0x01)             // Mic Input Level Boost
#define WM8731_ANALOG_MUTEMIC       ((unsigned char)0x02)             // Mic Input Mute to ADC
#define WM8731_ANALOG_INSEL         ((unsigned char)0x04)             // Mic Input Select to ADC (zero: select Line input to ADC)
#define WM8731_ANALOG_BYPASS        ((unsigned char)0x08)             // Bypass (line inputs summed to line out)
#define WM8731_ANALOG_DACSEL        ((unsigned char)0x10)             // DAC select (only send DAC output to line out)
#define WM8731_ANALOG_SIDETONE      ((unsigned char)0x20)             // Sidetone (mic inputs summed to line out)
#define WM8731_ANALOG_SIDEATT(n)    ((unsigned char)(n & 3)<<6)       // Sidetone Attenuation 0=-6dB, 1=-9dB, 2=-12dB, 3=-15dB

#define WM8731_DIGITAL_ADCHPD       ((unsigned char)0x01)             // ADC High Pass Filter Enable
#define WM8731_DIGITAL_HPOR         ((unsigned char)0x10)             // Store DC offset when High Pass filter disabled
#define WM8731_DIGITAL_DEEMP(n)     ((unsigned char)(n & 3)<<1)       // De-Emph: 0=disable, 1=32kHz, 2=44k1, 3=48k
#define WM8731_DIGITAL_DACMU        ((unsigned char)0x08)             // DAC Soft Mute (digital)

#define WM8731_LHEADOUT_LHPVOL(n)   ((unsigned char)(n & 0x3f))       // Left Headphone Output Volume (0..127)
#define WM8731_LHEADOUT_LHPVOL_MASK ((unsigned char)(0xc0))
#define WM8731_LHEADOUT_LZCEN       ((unsigned char)0x80)             // Left Channel Zero Cross Detect
#define WM8731_LHEADOUT_LRHPBOTH    ((unsigned short)0x100)           // Left to Right Headphone Control Join

#define WM8731_RHEADOUT_RHPVOL(n)   ((unsigned char)(n & 0x3f))       // Right Headphone Output Volume (0..127)
#define WM8731_RHEADOUT_RHPVOL_MASK ((unsigned char)(0xc0))
#define WM8731_RHEADOUT_RZCEN       ((unsigned char)0x80)             // Right Channel Zero Cross Detect
#define WM8731_RHEADOUT_RLHPBOTH    ((unsigned short)0x100)           // Right to Left Headphone Control Join

#define WM8731_INTERFACE_FORMAT(n)  ((unsigned char)(n & 3))          // Format: 0=MSB-first RJ, 1=MSB-first LJ, 2=I2S, 3=DSP
#define WM8731_INTERFACE_IWL(n)     ((unsigned char)(n & 3)<<2)       // Word Length: 0=16 1=20 2=24 3=32
#define WM8731_INTERFACE_LRP        ((unsigned char)(0x10)            // DACLRC phase control
#define WM8731_INTERFACE_LRSWAP     ((unsigned char)(0x20)            // DAC Left Right Clock Swap
#define WM8731_INTERFACE_MS         ((unsigned char)(0x40)            // Master/Slave Mode (1=master)
#define WM8731_INTERFACE_MCLKINV    ((unsigned char)(0x80)            // Bit Clock Invert

typedef enum {
      right_justified = 0,
      left_justified = 1,
      I2S = 2,
      DSP = 3
    } WM8731_interface_format;

#define WM8731_SAMPLING_USBMODE     ((unsigned char)0x01)             // USB Mode Select
#define WM8731_SAMPLING_BOSR        ((unsigned char)0x02)             // Base OverSampling Rate
#define WM8731_SAMPLING_SR(n)       ((unsigned char)(n & 0x0f)<<2)    // Sample Rate
#define WM8731_SAMPLING_CLKIDIV2    ((unsigned char)0x20)             // Core Clock Divider Select (0=MCLK, 1=MCLK/2)
#define WM8731_SAMPLING_CLKODIV2    ((unsigned char)0x40)             // CLKOUT Divider Select (0=MCLK, 1=MCLK/2)

#define WM8731_CONTROL_ACTIVE       ((unsigned char)1)


#define WM8731_DEBUG 1
#define WM8731_NREGISTERS 10
static unsigned short WM8731_registers[WM8731_NREGISTERS];

class WM8731_class
{
public:
    static void begin( WM8731_csb device_address, unsigned int sample_rate_hz, unsigned char word_length_bits, WM8731_interface_format interface_format );
    static void reset();
    static void setActive();
    static void setInactive();
    static void setInputVolume( unsigned char value ); /* 0 to 63 */
    static void setOutputVolume( unsigned char value ); /* 0 to 31 */
    static void set( unsigned char reg, unsigned short value );
    static inline unsigned short get( unsigned char reg ) { return WM8731_registers[reg]; };
};

extern WM8731_class WM8731;

#endif
