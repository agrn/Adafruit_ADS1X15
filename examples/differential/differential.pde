#include <Wire.h>
#include <Adafruit_ADS1015.h>

ads1x15 ads;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  
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
}

void loop(void)
{
  int16_t results;
  
  /* Be sure to update this value based on the IC and the gain settings! */
  float   multiplier = 3.0F;    /* ADS1015 @ +/- 6.144V gain (12-bit results) */
  //float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

  results = ads1x15_readADC_differential(&ads, ADS1015_REG_CONFIG_MUX_DIFF_0_1);
    
  Serial.print("Differential: "); Serial.print(results); Serial.print("("); Serial.print(results * multiplier); Serial.println("mV)");

  delay(1000);
}
