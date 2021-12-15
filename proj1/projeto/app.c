#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "app.h"
#include "macros.h"

unsigned char* create_information_plot(char ctrl, unsigned char * buffer, int length) {
    unsigned char* information_plot = (unsigned char*)malloc(sizeof(unsigned char) * (length + 6));
    
    information_plot[0] = FLAG;
    information_plot[1] = TRANSMITTER_A;
    information_plot[2] = ctrl;
    information_plot[3] = BCC(TRANSMITTER_A, ctrl);
    
    memcpy(&information_plot[4], buffer, length);

    unsigned char BCC2 = 0;

    for (int i = 0; i < length; i++) { BCC2 ^= buffer[i]; }

    printf("bcc2 is %u\n", BCC2);
    

    information_plot[4 + length] = BCC2;
    information_plot[5 + length] = FLAG; 

    return information_plot;
}