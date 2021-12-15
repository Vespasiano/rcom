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

    char fileName[256] = "penguin.jpg";
    int fileSize, fileFd;
    struct stat stats;

    fileFd = open(fileName, O_RDONLY);
    stat(fileName, &stats);
    fileSize = stats.st_size;

    if (status == 0) {
        // char fileName[256];
        // fileName = argv[3];
        // int fileSize, fileFd;
        // struct stat stats;

        // fileFd = open(fileName, O_RDONLY);
        // stat(fileName, &stats);
        // fileSize = stats.st_size;

        unsigned char fileData [fileSize];

        read(fileFd, &fileData, fileSize);

        for (int i = 0; i < 30; i++) {
            printf("%u", fileData[i]);
        }
        

        write(fd, fileData, fileSize);

        // unsigned char* information_plot = create_information_plot(C_S(plot_counter), fileData, fileSize);

        // write(fd, information_plot, fileSize + 6);
    }
    else {
        sleep(20);

        unsigned char fileData [fileSize];

        read(fd, &fileData, fileSize);

        int newFileFd = open("new_penguin.jpg",O_WRONLY |O_APPEND|O_CREAT|O_TRUNC,0750);
        
        for (int i = 0; i < 30; i++) {
            printf("%u", fileData[i]);
        }

        write(newFileFd, fileData, fileSize);
    
    }

    close(fileFd);

    printf("Closing serial port.\n");

    close(fd);

    return 0;
}