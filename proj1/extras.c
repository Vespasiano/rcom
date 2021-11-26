#define FLAG 0x7E
#define TRANSMITTER_A 0x03 //Command sent by Transmitter
#define RECEIVER_A 0X01    //Command sent by Receiver
#define BCC(A, C) (A ^ C) 
#define SET 0x3 
#define DISC 0xB
#define UA 0x7

#define TRUE 0
#define FALSE 1

typedef enum
{
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP,
} message_state;     

int send_packet(int fd, int A, int C){
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
    message_state state = START;
    while (state != STOP) {
        char buf[1];
        int res;
        res = read(fd, buf, 1);

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
            break;
        default:
            printf("Error in message state \n");
            exit(-1);
        }
    }
    
}

int send_set(int fd) {
    return send_packet(fd, TRANSMITTER_A, SET);
}

int send_ua(int fd) {
    return send_packet(fd, RECEIVER_A, UA);
}



int receive_set(int fd){
    return receive_packet(fd, TRANSMITTER_A, SET);
}

int receive_ua(int fd){
    return receive_packet(fd, RECEIVER_A, UA);
}
