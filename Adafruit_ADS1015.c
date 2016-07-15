/**************************************************************************/
/*!
  @file     Adafruit_ADS1015.cpp
  @author   K.Townsend (Adafruit Industries)
  @license  BSD (see license.txt)

  Driver for the ADS1015/ADS1115 ADC

  This is a library for the Adafruit MPL115A2 breakout
  ----> https://www.adafruit.com/products/???

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  @section  HISTORY

  v1.0 - First release
*/
/**************************************************************************/

#include <linux/i2c-dev.h>

#include "Adafruit_ADS1015.h"

/**************************************************************************/
/*!
  @brief  Writes 16-bits to the specified destination register
*/
/**************************************************************************/
static int writeRegister(int fd, uint8_t i2cAddress, uint8_t reg, uint16_t value) {
    uint8_t buf[3];
    if (ioctl(fd, I2C_SLAVE, i2cAddress) < 0)
        return -1;

    buf[0] = reg;
    buf[1] = ((uint8_t) (value >> 8));
    buf[2] = ((uint8_t) (value & 0xFF));

    if (write(fd, buf, 3) != 3)
        return -1;
    return 0;
}

/**************************************************************************/
/*!
  @brief  Writes 16-bits to the specified destination register
*/
/**************************************************************************/
static uint16_t readRegister(int fd, uint8_t i2cAddress, uint8_t reg) {
    char buf[2];
    if (ioctl(fd, I2C_SLAVE, i2cAddress) < 0)
        return 0;

    if (write(fd, &reg, 1) != 1)
        return 0;

    if (read(fd, buf, 2) != 2)
        return 0;

    return ((buf[0] << 8) | buf[1]);
}

/**************************************************************************/
/*!
  @brief  Instantiates a new ADS1015 class w/appropriate properties
*/
/**************************************************************************/
void ads1015_init(struct ads1x15 *ads1015, int fd, uint8_t i2cAddress) {
    ads1015->fd = fd;
    ads1015->i2cAddress = i2cAddress;
    ads1015->conversionDelay = ADS1015_CONVERSIONDELAY;
    ads1015->bitShift = 4;
    ads1015->gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}

/**************************************************************************/
/*!
  @brief  Instantiates a new ADS1115 class w/appropriate properties
*/
/**************************************************************************/
void ads1115_init(struct ads1x15 *ads1115, int fd, uint8_t i2cAddress) {
    ads1015_init(ads1115, fd, i2cAddress);

    ads1115->conversionDelay = ADS1115_CONVERSIONDELAY;
    ads1115->bitShift = 0;
}

/**************************************************************************/
/*!
  @brief  Gets a single-ended ADC reading from the specified channel
*/
/**************************************************************************/
uint16_t ads1x15_readADC_singleEnded(struct ads1x15 ads1x15, uint8_t channel) {
    uint16_t config;
    if (channel > 3)
        return 0;

    // Start with default values
    config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
             ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
             ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
             ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
             ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
             ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= ads1x15.gain;

    // Set single-ended input channel
    if (channel == 0)
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
    else if (channel == 1)
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
    else if (channel == 2)
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
    else
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(ads1x15.fd, ads1x15.i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    usleep(ads1x15.conversionDelay * 1000);

    // Read the conversion results
    // Shift 12-bit results right 4 bits for the ADS1015
    return readRegister(ads1x15.fd, ads1x15.i2cAddress, ADS1015_REG_POINTER_CONVERT) >> ads1x15.bitShift;
}

/**************************************************************************/
/*! 
  @brief  Reads the conversion results, measuring the voltage
  difference between the P and N input.  Generates
  a signed value since the difference can be either
  positive or negative.
*/
/**************************************************************************/
int16_t ads1x15_readADC_differential(struct ads1x15 ads1x15, uint8_t channels) {
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    uint16_t res;

    // Set PGA/voltage range
    config |= ads1x15.gain;

    // Set channels
    config |= channels;

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(ads1x15.fd, ads1x15.i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    usleep(ads1x15.conversionDelay * 1000);

    // Read the conversion results
    res = readRegister(ads1x15.fd, ads1x15.i2cAddress, ADS1015_REG_POINTER_CONVERT) >> ads1x15.bitShift;
    if (ads1x15.bitShift != 0 && res > 0x07FF)
        res |= 0xF000;

    return (int16_t) res;
}

/**************************************************************************/
/*!
  @brief  Sets up the comparator to operate in basic mode, causing the
  ALERT/RDY pin to assert (go from high to low) when the ADC
  value exceeds the specified threshold.

  This will also set the ADC in continuous conversion mode.
*/
/**************************************************************************/
void ads1x15_startComparator_singleEnded(struct ads1x15 ads1x15, uint8_t channel, int16_t threshold) {
    uint16_t config;
    if (channel > 3)
        return;

    // Start with default values
    config = ADS1015_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
             ADS1015_REG_CONFIG_CLAT_LATCH   | // Latching mode
             ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
             ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
             ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
             ADS1015_REG_CONFIG_MODE_CONTIN  | // Continuous conversion mode
             ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

    // Set PGA/voltage range
    config |= ads1x15.gain;

    // Set single-ended input channel
    if (channel == 0)
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
    else if (channel == 1)
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
    else if (channel == 2)
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
    else
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;

    // Set the high threshold register
    // Shift 12-bit results left 4 bits for the ADS1015
    writeRegister(ads1x15.fd, ads1x15.i2cAddress, ADS1015_REG_POINTER_HITHRESH, threshold << ads1x15.bitShift);

    // Write config register to the ADC
    writeRegister(ads1x15.fd, ads1x15.i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
}

/**************************************************************************/
/*!
  @brief  In order to clear the comparator, we need to read the
  conversion results.  This function reads the last conversion
  results without changing the config value.
*/
/**************************************************************************/
int16_t ads1x15_getLastConversionResults(struct ads1x15 ads1x15) {
    uint16_t res;

    // Wait for the conversion to complete
    usleep(ads1x15.conversionDelay * 1000);

    // Read the conversion results
    res = readRegister(ads1x15.fd, ads1x15.i2cAddress, ADS1015_REG_POINTER_CONVERT) >> ads1x15.bitShift;
    if (ads1x15.bitShift != 0 && res > 0x07FF)
        res |= 0xF000;

    return (int16_t) res;
}
