#include "extras.h"   


int send_packet(int fd, int A, int C){
    char buf[5];
    int bytes;
    buf[0] = FLAG;
    buf[1] = A;
    buf[2] = C;
    buf[3] = BCC(A, C);
    buf[4] = FLAG;

    bytes = write(fd, buf, 5);
    // printf("%d bytes written\n", bytes);
    return bytes;
}

int receive_packet(int fd, int A, int C){
    message_state state = START;
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

information_state trama_check(information_state state, char byte, int C) {
    switch (state) {
    case START_I:
        if (byte == FLAG_RCV_I) { state = FLAG_RCV; }
        break;
    case FLAG_RCV_I:
        if (byte == TRANSMITTER_A) { state = A_RCV_I; }
        else if (byte == FLAG) { state = FLAG_RCV_I; }
        else { state = START_I; }
        break;
    case A_RCV_I:
        if (byte == C_S(0)) { state = C_RCV_I; }
        else if (byte == FLAG) { state = FLAG_RCV_I; }
        else { state = START_I; }
        break;
    case BCC1_OK:
        break;
    case DATA:
        break;
    case STOP_I:
        break;
    default:
        exit(1);
        break;
    }
    return state;
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
