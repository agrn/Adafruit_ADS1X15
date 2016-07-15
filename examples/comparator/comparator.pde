#include <Wire.h>
#include <Adafruit_ADS1015.h>

struct ads1x15 ads;

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Single-ended readings from AIN0 with >3.0V comparator");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  Serial.println("Comparator Threshold: 1000 (3.000V)");

  // ads1115_init(&ads); // Use this for the ADS1115 (16-bit)
  ads1015_init(&ads); // Use this for the ADS1015 (12-bit)

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads->gain = GAIN_TWOTHIRDS;  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads->gain = GAIN_ONE;        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads->gain = GAIN_TWO;        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads->gain = GAIN_FOUR;       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads->gain = GAIN_EIGHT;      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads->gain = GAIN_SIXTEEN;    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  ads1x15_begin();

  // Setup 3V comparator on channel 0
  ads1x15_startComparator_singleEnded(&ads, 0, 1000);
}

void loop(void) 
{
  int16_t adc0;

  // Comparator will only de-assert after a read
  adc0 = ads1x15_getLastConversionResults(&ads);
  Serial.print("AIN0: "); Serial.println(adc0);
  
  delay(100);
}
