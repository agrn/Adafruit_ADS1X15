#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <string.h>

#include "Adafruit_ADS1015.h"

int main(int argc, char **argv) {
    int file;
    int16_t results;
    float multiplier = 3.0F; /* 3.0F for ADS1015, 0.1875F for ADS1115 */
    struct ads1x15 ads;
    char *filename = "/dev/i2c-1";

    if (argc == 1)
        printf("Warning: no I2C device provided. Using /dev/i2c-1.");
    else
        filename = argv[1];

    file = open(filename, O_RDWR);
    if (file < 0) {
        perror("Failed to open bus");
        return EXIT_FAILURE;
    }

    ads1015_init(&ads, file, 0x48);
    results = ads1x15_readADC_differential(ads, ADS1015_REG_CONFIG_MUX_DIFF_0_1);
    printf("Differential: %d (%d mV)", results, results * multiplier);

    close(file);

    return EXIT_SUCCESS;
}
