#include "extras.h"   

#include <string.h>

int send_packet(int fd, int A, int C){
    printf("sending packet\n");
    char buf[5];
    int bytes;
    buf[0] = FLAG;
    buf[1] = A;
    buf[2] = C;
    buf[3] = BCC(A, C);
    buf[4] = FLAG;

    bytes = write(fd, buf, 5);
    printf("%d bytes written\n", bytes);
    return bytes;
}

int receive_packet(int fd, int A, int C){
    information_state state = START;
    int bytes;
    while (state != STOP) {
        char buf[1];
        int res;
        res = read(fd, buf, 1);
        bytes++;

        switch (state) {
        case START:
            if (buf[1] == FLAG) { state = FLAG_RCV; }
            break;
        case FLAG_RCV:
            if (buf[1] == A) { state = A_RCV; }
            else if (buf[1] == FLAG) { state = FLAG_RCV; }
            else { state = START; }
            break;
        case A_RCV:
            if (buf[1] == C) { state = C_RCV; }
            else if (buf[1] == FLAG) { state = FLAG_RCV; }
            else { state = START; }
            break;
        case C_RCV:
            if (buf[1] == BCC(A, C)) { state = BCC_OK; }
            else if (buf[1] == FLAG) { state = FLAG_RCV; }
            else { state = START; }
            break;
        case BCC_OK:
            if (buf[1] == FLAG) { state = STOP; } 
            else { state = START; }
            break;
        default:
            printf("Error in message state \n");
            exit(-1);
        }

        buf[res] = 0;
    }
    printf("%d bytes read\n", bytes);
    return bytes;
}


int send_set(int fd) {
    return send_packet(fd, TRANSMITTER_A, SET);
}

int send_ua(int fd) {
    return send_packet(fd, RECEIVER_A, UA);
}

int send_disc(int fd) {
    return send_packet(fd, RECEIVER_A, DISC);
}

int send_rr0(int fd) {
    return send_packet(fd, RECEIVER_A, C_RR0);
}

int send_rr1(int fd) {
    return send_packet(fd, RECEIVER_A, C_RR1);
}

int send_rej0(int fd) {
    return send_packet(fd, RECEIVER_A, C_REJ0);
}

int send_rej1(int fd) {
    return send_packet(fd, RECEIVER_A, C_REJ1);
}

int receive_set(int fd){
    return receive_packet(fd, TRANSMITTER_A, SET);
}

int receive_ua(int fd){
    return receive_packet(fd, RECEIVER_A, UA);
}

int receive_disc(int fd){
    return receive_packet(fd, RECEIVER_A, DISC);
}

int receive_rr0(int fd){
    return receive_packet(fd, RECEIVER_A, C_RR0);
}

int receive_rr1(int fd){
    return receive_packet(fd, RECEIVER_A, C_RR1);
}

int receive_rej0(int fd){
    return receive_packet(fd, RECEIVER_A, C_REJ0);
}

int receive_rej1(int fd){
    return receive_packet(fd, RECEIVER_A, C_REJ1);
}

int byte_stuffing(unsigned char* packet, int size, unsigned char** frame){
  *frame = (unsigned char*) malloc(size * 2);
  int new_size = 0;
  for(int i = 0, j = 0; i < size; i++,j++)
  {
    if(packet[i] == FLAG){
      (*frame)[j] = ESC;
      j++;
      (*frame)[j] = 0x5E;
    }
    else if (packet[i] == ESC){
      (*frame)[j] = ESC;
      j++;
      (*frame)[j] = 0x5D;
    }
    else{
      (*frame)[j] = packet[i];
    }
    new_size++;
  }
  return new_size;
}