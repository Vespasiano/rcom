#include <stdio.h>


typedef enum
{



} state;

int main(int argc, char **argv[]) {
    int porta, status;
    if (!(argc == 3 && strcmp(argv[1], "TRANSMITTER") == 0) &&
        !(argc == 4 && strcmp(argv[1], "RECEIVER") == 0)) {
        fprintf(stderr, "Usage: [TRANSMITTER | RECEIVER ] SerialPort Filename\n");
        exit(1);
    }

    porta = atoi(argv[2]);
    status = strcmp(argv[1], "TRANSMITTER"); //Sets status to 0 if it's TRANSMITTER, 1 if it's RECEIVER
    
    llopen(porta, status);
    
}