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

extern int pCounter;

message info;

int create_control_packet(int status,int fileSize, char* fileName,unsigned char** packet) {
    int L1 = fileSize % 256; //file -> file_size % 256
    int L2 = strlen(fileName);
    int packet_size = L1 + L2 + 5;
    *packet = (unsigned char*)malloc(packet_size);
    (*packet)[0] = status;
    (*packet)[1] = 0;
    (*packet)[2] = L1;
    int i;
    for(i = 3;i < (L1 + 3);i++)
    {
        (*packet)[i] = ((fileSize) >> 8 * (i - 3)) & (0xFF);
    }
    i++;
    (*packet)[i] = 1;
    i++;
    (*packet)[i] = L2;
    memcpy(&packet[i],fileName,fileSize);
    return packet_size;
}

int create_data_packet(unsigned char* data, int size, unsigned char** data_packet)
{
    int L1 = size / 256;
    int L2 = size % 256;
    int packet_size = 4 + size;
    *data_packet = (unsigned char*)malloc(packet_size);
    (*data_packet)[0] = 1;
    (*data_packet)[1] = pCounter % 256;
    (*data_packet)[2] = L2;
    (*data_packet)[3] = L1;
    memcpy(*data_packet + 4,data,size);
    return packet_size;
}

int read_file(int fileFd,char* buffer, int size)
{
    int r = 0;
    if((r = read(fileFd,buffer,size)) < 0) return -1;
    return r;
}

int main(int argc, char **argv) {
    int porta, status, fd;
    struct stat st;

    if (!(argc == 4 && (strcmp(argv[1], "TRANSMITTER") == 0)) &&
        !(argc == 3 && (strcmp(argv[1], "RECEIVER") == 0))) {
        fprintf(stderr, "Usage: [TRANSMITTER | RECEIVER ] SerialPort Filename (only if you're TRANSMITTER)\n");
        exit(1);
    }


    status = strcmp(argv[1], "TRANSMITTER"); //Sets status to 0 if it's TRANSMITTER, 1 if it's RECEIVER
    porta = atoi(argv[2]);

    printf("porta is %i \n", porta);

    state = OPENING_CONNECTION;

    printf("opening connection \n");
    fd = llopen(porta, status);

    printf("Created connection. \n");

    if (status) { //TRANSMITTER

        printf("welcome transmitter");
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


        int control_packet_size = 5 * sizeof(char) + strlen(fileName);
        
        char *control_packet = (char *)malloc(control_packet_size);

        create_control_packet(2, fileSize, fileName, &control_packet);

        if (llwrite(fd, control_packet, control_packet_size) < 0) {
            return -1;
        }
        
        free(control_packet);

        int size = 1024;
        int bytes_read = 0;
        int packet_size = 0;
        char *buffer = (char *) malloc(size * sizeof(char) + 1);
        char *packet;

        


        while (1) {
            memset(buffer, '\0', size);

            bytes_read = read_file(fileFd, buffer, size);
            
            if (bytes_read < 0) { return -1; }
            else if (bytes_read == 0) { break;}

            printf("Read %i bytes", bytes_read);

            packet_size = 4 + bytes_read;

            packet = (char *) malloc(packet_size);

            if (create_data_packet(buffer, bytes_read, &packet)) {
                return -1;
            }

            if (llwrite(fd, packet, packet_size) < 0) {
                return -1;
            }

            free(packet);
        }

        free(buffer);
        
        char *end_control_packet = (char *)malloc(control_packet_size);

        create_control_packet(3, fileSize, fileName, &end_control_packet);

        if (llwrite(fd, end_control_packet, control_packet_size) < 0) {
            return -1;
        }

        free(end_control_packet);

        close(fileFd);

        llclose(fd, 0);

    }
    else { //RECEIVER
        char* file;
        int fileSize = 0;
        int fileIndex = 0;

        int l1index, l2index, l1, l2;

        int reading = 1;

        while (reading) {
            char * buffer;
            int bytes_read = llread(fd, buffer);

            
            switch(buffer[0]) {
                case 1:
                    l1index = 3;
                    l2index = 2;
                    l1 = buffer[l1index];
                    l2 = buffer[l2index];

                    int k = 256 * l2 + l1;

                    int index = 1;
                    int Ns = buffer[index];

                    if (Ns > info.msgNr) break;

                    info.msgNr++;
                    info.msgNr%= 255;

                    memcpy(file + fileIndex, buffer + 4, k);

                    fileIndex += 1024;
                    break;
                    
                case 2:
                    l1index = 2;
                    l1 = buffer[l1index];

                    l2index = 4 + l1;
                    l2 = buffer[l2index];

                    int fileSizeIndex = l1index + 1;
                    for (int i = 0; i < l1; i++) { fileSize += buffer[fileSizeIndex + i] * 256; }

                    int fileNameIndex = l2index + 1;
                    char * fileName = (char *) malloc(l2);
                    memcpy(fileName, buffer + fileNameIndex, l2);

                    int fileFd = fopen(fileName,"w");

                    file = (char *)malloc(fileSize);

                    free(fileName);

                    break;
                case 3:
                    fwrite(file, 1, fileSize, fileFd);
                    reading = 0;
                    free(file);
                    break;
            }
        }

        llclose(fd, 1);
    }

    return 0;   
}

