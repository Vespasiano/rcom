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
        unsigned char buffer[5] = "bruh";
        unsigned char* information_plot = create_information_plot(C_S(plot_counter), buffer, 5);
        write(fd, "nigga", 5);
        printf("information plot is %s\n", information_plot);
        for (int i = 0; i < strlen( (char *) information_plot); i++) {
            printf("char number %i is %u\n", i, information_plot[i]);
        }
        
        // write(fd, information_plot, 11);
    }
    else {
        
        unsigned char buffer2[5];
        // read(fd, buffer2, 11);
        read(fd, buffer2, 5);
        printf("buffer2 is %s\n", buffer2);
    }

    printf("Closing serial port.\n");

    close(fd);

    return 0;
}