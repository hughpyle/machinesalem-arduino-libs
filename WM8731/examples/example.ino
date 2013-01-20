#include <WM8731.h>

/* WM8731 audio codec */
/* Testing with the proto board from Mikro (which unfortunately doesn't break out Line-In & Line-Out to pins) */
/* This doesn't test data at all - it only sends control signals. */

#include <Wire.h>
#include <WM8731.h>

void setup()
{
  Serial.println( "Starting" );
  delay(2000);
  Serial.println( "Initializing" );
  delay(2000);
  
  // Define the data interface format
  unsigned char interface = WM8731_INTERFACE_FORMAT(I2S) | WM8731_INTERFACE_WORDLEN(bits16);
  // Is the codec behaving as "master" (driving the clocks)?  Usually that would be the best thing.
  interface |= WM8731_INTERFACE_MASTER;
  // Initialize the codec at 48kHz
  WM8731.begin( low, WM8731_SAMPLING_RATE(hz48000), interface );

  Serial.println( "Initialized" );
}


void loop()
{
  delay(2000);
  WM8731.setActive();
  Serial.println( "Active" );

  delay(2000);
  // Microphone inputs to DAC & line output
  Serial.println( "Loop-Mic" );
  WM8731.setInputVolume(31);
  WM8731.setOutputVolume(0xFF);
  WM8731.set( WM8731_ANALOG, WM8731_ANALOG_INSEL | WM8731_ANALOG_MICBOOST | WM8731_ANALOG_SIDETONE | WM8731_ANALOG_SIDEATT(0) );

  delay(5000);
  // Line inputs to line output
  Serial.println( "Loop-Bypass" );
  WM8731.set( WM8731_ANALOG, WM8731_ANALOG_BYPASS );

  delay(2000);
  // DAC to line output
  Serial.println( "Loop-DAC" );
  WM8731.set( WM8731_ANALOG, WM8731_ANALOG_DACSEL );
  
  delay(2000);
  Serial.println( "Loop-Off" );
  WM8731.setInputVolume(0);
  WM8731.setOutputVolume(0);
  WM8731.set( WM8731_ANALOG, 0 );

  delay(2000);
  WM8731.setInactive();
  Serial.println( "Inactive" );
}


