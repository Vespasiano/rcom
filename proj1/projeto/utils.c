#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


#include "utils.h"
#include "macros.h"

state_machine stateM;

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

int receive_packet(int fd) {
    int ret;
    stateM.state = START;
    while(stateM.state != S_MSG_RCV) {
        unsigned char byte[1];
        ret = read(fd, &(byte[0]), 1);
        if (ret == -1) { 
          printf("Failed read");
          break; 
        }

        stateMachine(byte[0]);
    }

    return 0;
}

void stateMachine(unsigned char byte) {
  switch (stateM.state) {
  case START:
    if (byte == FLAG) { stateM.state = FLAG_RCV; }
    break;
  case FLAG_RCV:
    if (byte == FLAG) { break;}
    else if ((byte == TRANSMITTER_A) || (byte == RECEIVER_A)) {
      stateM.state = A_RCV;
      stateM.addr = byte;
    }
    else { stateM.state = START; }
    break;
  case A_RCV:
    switch (byte) {
        case FLAG:
            stateM.state = FLAG_RCV;
            break;
        case DISC:
        case SET: 
        case UA:
        case C_RR0:
        case C_RR1:
        case C_REJ0:
        case C_REJ1:
            stateM.state = C_RCV;
            stateM.ctrl = byte;
            break;
        case C10: case C11:
            stateM.state = I_RCV;
            stateM.ctrl = byte;
            break;
        default:
            stateM.state = START;
            break;    
    }
    break;
  case C_RCV:
    if (byte == FLAG) { stateM.state = FLAG_RCV; }
    else if (byte == BCC(stateM.addr, stateM.ctrl)) { stateM.state = BCC_OK; }
    else { stateM.state = START; }
    break;
  case BCC_OK:
    if (byte == FLAG) { stateM.state = S_MSG_RCV; }
    else { stateM.state = START; }
    break;
  case I_RCV:
    if (byte == FLAG) { stateM.state = FLAG_RCV; }
    else if (byte == BCC(stateM.addr, stateM.ctrl)) { stateM.state = DATA_RCV;}
    else { stateM.state = START;}
    break;
  case S_MSG_RCV:
    return ;
  case DATA_RCV:
    return ;
//   case DATA_RCV:
//     if (byte == FLAG) {
//       stateM.unstuffed_size = unstuffData(stateM.message, stateM.index, stateM.unstuffed_msg);
//       if (stateM.unstuffed_size < 0) { 
//         stateM.state = ERROR;
//         return;
//       }

//       char bcc = retrieveBcc(stateM.unstuffed_msg, (stateM.unstuffed_size - 1), (int) 0);

//       char rec_bcc = stateM.unstuffed_msg[stateM.unstuffed_size - 1];

//       if (bcc == rec_bcc) { stateM.state = INFO_RCV; }
//       else { stateM.state = ERROR; }

//     }
//     else {
//       if(stateM.index < ((1024 * 2) + 2)) { stateM.message[stateM.index++] = byte; }
//       else { stateM.state = ERROR; }
//     }
//     break;
  case ERROR:
    return;
  case INFO_MSG:
    return;
  case STOP:
    return;
  }
}


