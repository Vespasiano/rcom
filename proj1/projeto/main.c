#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

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
        unsigned char* fileData = "aaaaa";

        unsigned char* information_plot = create_information_plot(C_S(plot_counter), fileData, 5);
        for(int i = 0; i < 11; i++)
            printf("%u\n",information_plot[i]);

        write(fd, information_plot, 5 + 6);
    }
    else {

        char buff[1];
        
        for (int i = 0; i < 11; i++) {
            read(fd,&buff[0],1);
            printf("%u\n", buff[0]);
        }
    
    }

    printf("Closing serial port.\n");

    llclose(fd,status);

    return 0;
}