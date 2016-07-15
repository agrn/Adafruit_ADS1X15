#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <string.h>

#include "Adafruit_ADS1015.h"

int main(int argc, char **argv) {
    int file, i;
    struct ads1x15 ads;
    char *filename = "/dev/i2c-1";

    if (argc == 1)
        printf("Warning: no I2C device provided. Using /dev/i2c-1.\n");
    else
        filename = argv[1];

    file = open(filename, O_RDWR);
    if (file < 0) {
        perror("Failed to open bus");
        if (getuid() != 0)
            printf("Try running this program as root.\n");

        return EXIT_FAILURE;
    }

    //ads1115_init(&ads, file, 0x48); // Use this for the ADS1115
    ads1015_init(&ads, file, 0x48); // Use this for the ADS1015

    // The ADC input range (or gain) can be changed via the following
    // functions, but be careful never to exceed VDD +0.3V max, or to
    // exceed the upper and lower limits if you adjust the input range!
    // Setting these values incorrectly may destroy your ADC!
    //                                                              ADS1015  ADS1115
    //                                                              -------  -------
    // ads.gain = GAIN_TWOTHIRDS;  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // ads.gain = GAIN_ONE;        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    // ads.gain = GAIN_TWO;        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
    // ads.gain = GAIN_FOUR;       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
    // ads.gain = GAIN_EIGHT;      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
    // ads.gain = GAIN_SIXTEEN;    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

    for (i = 0; i < 4; i++)
        printf("A%d: %d\n", i, ads1x15_readADC_singleEnded(ads, i));

    close(file);

    return EXIT_SUCCESS;
}
