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
        printf("Warning: no I2C device provided. Using /dev/i2c-1.");
    else
        filename = argv[1];

    file = open(filename, O_RDWR);
    if (file < 0) {
        perror("Failed to open bus");
        return EXIT_FAILURE;
    }

    ads1015_init(&ads, file, 0x48);

    for (i = 0; i < 4; i++)
        printf("A%d: %d\n", i, ads1x15_readADC_singleEnded(ads, i));

    close(file);

    return EXIT_SUCCESS;
}
