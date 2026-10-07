
#include "Wire.h"

#define DESIGN_VERSION 1

#define GPIO_I2C_ADDR2    D3
#define GPIO_I2C_ADDR1    D4

// i2c address of the MCP23008 IO-portexpander chip
// All 3 ADR = 0 => i2c address is = 0b0100aaaR (aaa are A2, A1, A0 pins, R is 0 for write, 1 for read)
#define I2C_ADDR (0x40>>1) // shift one right to get 7bit i2c address

// The MCP23008 commands on the i2c bus consist of
// - 1 byte register address
// - 1 byte register value
// and write the value to the MCP23008 internal register storage
// This program uses two regsisters of the MCP23008:
// - register address 0x00: contains one bit for each of the 8 GPIOs. It defines whether the GPIO is input(1) or output(0).
// - register address 0x09: contains one bit for each of the 8 GPIOs. It defines the voltage level on the GPIO pin. 0=low, 1=high

// The following diagram shows the 7 segment display with its 8 led areas (the 8th led is the decimal point)
// Each led is marked with the number of the GPIO pin of the MCP23008 connected to it. To illuminate one of the LEDs,
// the corresponding bit in the GPIO register of the MCP23008 must be set to 0.
//     5
//  4     6
//     3
//  2     8
//     1     7    

// acDigit contains the GPIO code to display the numbers 0...6
const char acDigit[] = {
  0x44,         // binary: 0100 0100    digit 0
  0x5f,         // binary: 0101 1111    digit 1
  0xc8,         // binary: 1100 1000    digit 2
  0x4a,         // binary: 0100 1010    digit 3
  0x53,         // binary: 0101 0011    digit 4
  0x62,         // binary: 0110 0010    digit 5
  0x60,         // binary: 0110 0000    digit 6
  0x47,         // binary: 0100 1111    digit 7
  0x40,         // binary: 0100 0000    digit 8
  0x42,         // binary: 0100 0010    digit 9
};


void startDice();
void animateDice();
void displayDigit();
void MCP23008_setOutput();
void MCP23008_writeValue( int value );

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);

  pinMode( GPIO_I2C_ADDR1, OUTPUT );
  pinMode( GPIO_I2C_ADDR2, OUTPUT );
  #if DESIGN_VERSION == 2
  pinMode( D6, INPUT ); // This is our digital input for the push button for the second design version
  #endif

  digitalWrite( GPIO_I2C_ADDR1, LOW );
  digitalWrite( GPIO_I2C_ADDR2, LOW );

  // Initialize the i2c bus protocol (for access to the MCP23008 port expander chip)
  Wire.begin();

  // Set all GPIO values to HIGH (which means in the dice circuit: all LEDs OFF)
  MCP23008_writeValue( 0xff );

  // Set all the GPIO to output
  MCP23008_setOutput();
 }



void loop()
{
#if DESIGN_VERSION == 2
  int nStartDice = digitalRead( D6 );
  if( nStartDice == 1 )
#else
  int nStartDice = analogRead( A0 );  // Returns voltage level on A0 pin (0=0Volt, 1024=3.3Volt)
  if( nStartDice > 500 )
#endif
  {
    Serial.printf( "nStartDice=%d\n", nStartDice );

    startDice();
  }
}


void startDice()
{
  // Refresh the GPIO settings (not strictly necessary, just to be sure)
  MCP23008_setOutput();

  // Determine a random outcome of the dice throw
  randomSeed( micros() );
  int diceValue = (int) random(1,7); // Generates a random number from 1 to 6

  animateDice();

  // Display the dice value
  displayDigit( diceValue );
}

void animateDice()
{
  for( int i = 0; i<=20; i++ )
  {
    int index = i % 10; // Calculate the digit to be displayed which runs from 0 to 9
    displayDigit( index );

    delay(20);
  }
}

// value: digit to be displayed, must be from 0 to 9
void displayDigit( int digit )
{
  MCP23008_writeValue( acDigit[digit] );
}

// Set all GPIOs of the MCP23008 as output
void MCP23008_setOutput()
{
  // Set all the GPIO to output
  Wire.beginTransmission( I2C_ADDR );
  Wire.write( 0x00 ); // Register address, 0x00 ia the direction register (input/output)
  Wire.write( 0x00 ); // Value, 0x00 means: set all GPIO pins as output
  int error = Wire.endTransmission(true);
  if( error != 0 )
    Serial.printf( "i2c write value returned %d\n", error );
}

// Write value to the GPIO register of the MCP23008
void MCP23008_writeValue( int value )
{
  Wire.beginTransmission( I2C_ADDR );
  Wire.write( 0x09 );
  Wire.write( value );
  int error = Wire.endTransmission(true);
  if( error != 0 )
    Serial.printf( "i2c write value returned %d\n", error );
}

