#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


#include "ll.h"
#include "extras.h"
#include "defines.h"
#include "alarm.h"

program_state state;

int cFlag = FALSE;
int tries = 0;

int main(int argc, char **argv) {
    int porta, status, fd;
    struct stat st;

    if (!(argc == 3 && (strcmp(argv[1], "TRANSMITTER") == 0)) &&
        !(argc == 4 && (strcmp(argv[1], "RECEIVER") == 0))) {
        fprintf(stderr, "Usage: [TRANSMITTER | RECEIVER ] SerialPort Filename (only if you're TRANSMITTER)\n");
        exit(1);
    }


    status = strcmp(argv[1], "TRANSMITTER"); //Sets status to 0 if it's TRANSMITTER, 1 if it's RECEIVER
    porta = atoi(argv[2]);

    state = OPENING_CONNECTION;
    fd = llopen(porta, status);
    
    printf("Created connection. \n");

    if (status) { //TRANSMITTER
        int fileFd, fileSize;
        char fileName[256];
        struct stat fileData;

        sprintf(fileName, "%s", argv[3]);
        stat(fileName, &st);
    

        fileFd = open(fileName, O_RDONLY);
        if(fileFd == -1) {
            perror("Error opening file.\n");
            return -1;
        }

        if(fstat(fileFd, &fileData) == -1) {
            perror("Error in fstat.\n");
            return -1;
        }

        fileSize = fileData.st_size;

        state = SENDING_DATA;
        lControl.N_s = 1;



    }
    else { //RECEIVER

    }

    return 0;   
}