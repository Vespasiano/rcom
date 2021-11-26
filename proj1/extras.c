//F A C BCC1 F

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



int send_set(int fd){
    char buf[5];
    int bytes;
    buf[0] = FLAG;
    buf[1] = TRANSMITTER_A;
    buf[2] = SET;
    buf[3] = BCC(TRANSMITTER_A, SET);
    buf[4] = FLAG;

    bytes = write(fd, buf, 5);
    printf("%d bytes written\n", bytes);
    return bytes;
}

int send_ua(int fd){
    char buf[5];
    int bytes;
    buf[0] = FLAG;
    buf[1] = RECEIVER_A;
    buf[2] = UA;
    buf[3] = BCC(RECEIVER_A, UA);
    buf[4] = FLAG;

    bytes = write(fd, buf, 5);
    printf("%d bytes written\n", bytes);
    return bytes;
}

int receive_set(int fd){
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
            if (buf[1] == RECEIVER_A) { state = A_RCV; }
            else if (buf[1] == FLAG) { state = FLAG_RCV; }
            else { state = START; }
            break;
        case A_RCV:
            if (buf[1] == SET) { state = C_RCV; }
            else if (buf[1] == FLAG) { state = FLAG_RCV; }
            else { state = START; }
            break;
        case C_RCV:
            if (buf[1] == BCC(RECEIVER_A, SET)) { state = BCC_OK; }
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