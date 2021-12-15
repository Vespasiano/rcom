#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ll.h"
#include "macros.h"
#include "app.h"

static int plot_counter = 0;

int main(int argc, char **argv) {
    int porta, status, fd;

    if (!(argc == 4 && (strcmp(argv[1], "TRANSMITTER") == 0)) &&
        !(argc == 3 && (strcmp(argv[1], "RECEIVER") == 0))) {
        fprintf(stderr, "Usage: [TRANSMITTER | RECEIVER ] SerialPort Filename (only if you're TRANSMITTER)\n");
        exit(1);
    }

    if ((strcmp(argv[1], "TRANSMITTER")) == 0) { //Sets status to 0 if it's TRANSMITTER, 1 if it's RECEIVER
        status = 0;
    }
    else { status = 1; }
    
    porta = atoi(argv[2]);

    fd = llopen(porta, status);

    if (status == 0) {
        unsigned char buffer[5] = "nigga";
        unsigned char* information_plot = create_information_plot(C_S(plot_counter), buffer, 5);
        write(fd, information_plot, 11);
        // for (int i = 0; i < 11; i++) {
        //     printf("byte %i is %u\n", i, information_plot[i]);
        // }
    }
    else {
        sleep(1);
        unsigned char buffer[21];
        int res = 0;
        res = read(fd, buffer, 21);
        buffer[res] = 0;
        // read(fd, buffer2, 11);
        

        // for (int i = 0; i < 21; i++) {
        //     printf("byte %i is %u\n", i, buffer[i]);
        // }
    }

    printf("Closing serial port.\n");

    close(fd);

    return 0;
}