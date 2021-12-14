#include "ll.h"
#include "extras.h"
#include "defines.h"
#include "alarm.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <strings.h>
#include <stdint.h>
#include <stdlib.h>>
#include <string.h>


#define BAUDRATE B38400

struct termios oldtio, newtio;

struct applicationLayer {
int fileDescriptor; /*Descritor correspondente à porta série*/
int status; /*TRANSMITTER | RECEIVER*/
};

l_Control lControl;

int pCounter = 0;

static message info = (message){.msgNr = 0};

state_machine state_Machine;

static int rejs;
static int rrs;

extern int cFlag;
extern int tries;

char retrieveBcc(char * buffer, int data_bytes, int bytes) {
    char bcc = buffer[bytes];
    int i = bytes + 1;
    for(; i < bytes + data_bytes; ++i) {  bcc ^= buffer[i]; }
    return bcc;
}

int llopen(int porta, int status) {

  char port[256];
  int fd;

  lControl.N_s = 0;
  // sprintf(port, "/dev/ttyS%i", porta);
  // printf("port %s\n", port);
  
  if (status == 0) {
    fd = open("/dev/ttyS10", O_RDWR | O_NOCTTY);
  }
  else {
    fd = open("/dev/ttyS11", O_RDWR | O_NOCTTY); 
  }

  // fd = open(port, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror(port);
    return(-1);
  }
  

  if (tcgetattr(fd, &oldtio) == -1) 
  { /* save current port settings */
    perror("tcgetattr");
    return(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 0;  /* blocking read until 5 chars received */

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    return(-1);
  }


  if (status == 0) { //TRANSMITTER
    do
    {
      send_set(fd);
      setAlarm(TIMEOUT);
      cFlag = 0;
      receive_ua(fd);
      cancelAlarm();  
      printf("cflag is %i and tries is %i\n", cFlag, tries);
    } while (cFlag && tries <= MAX_TRIES);
    if (tries >= MAX_TRIES) return -1;
    else tries = 1; 
    cancelAlarm();


  }
  else { //RECEIVER
    do
    {
      receive_set(fd);
      setAlarm(TIMEOUT);
      cFlag = 0;
      send_ua(fd);
      cancelAlarm();  
    } while (cFlag && tries <= MAX_TRIES);
    if (tries >= MAX_TRIES) return -1;
    else tries = 1; 
    cancelAlarm();
  }

  printf("New termios structure set\n");

  return fd; // link layer identifier
}

void stateMachine(char byte) {
  switch (state_Machine.state) {
  case START:
    if (byte == FLAG) { state_Machine.state = FLAG_RCV; }
    break;
  case FLAG_RCV:
    if (byte == FLAG) { break;}
    else if ((byte == TRANSMITTER_A) || (byte == RECEIVER_A)) {
      state_Machine.state = A_RCV;
      state_Machine.addr = byte;
    }
    else { state_Machine.state = START; }
    break;
  case A_RCV:
    switch (byte) {
        case FLAG:
            state_Machine.state = FLAG_RCV;
            break;
        case DISC:
        case SET: 
        case UA:
        case C_RR0:
        case C_RR1:
        case C_REJ0:
        case C_REJ1:
            state_Machine.state = C_RCV;
            state_Machine.ctrl = byte;
            break;
        case C10: case C11:
            state_Machine.state = INFO_RCV;
            state_Machine.ctrl = byte;
            break;
        default:
            state_Machine.state = START;
            break;    
    }
    break;
  case C_RCV:
    if (byte == FLAG) { state_Machine.state = FLAG_RCV; }
    else if (byte == BCC(state_Machine.addr, state_Machine.ctrl)) { state_Machine.state = BCC_OK; }
    else { state_Machine.state = START; }
    break;
  case BCC_OK:
    if (byte == FLAG) { state_Machine.state = MSG_RCV; }
    else { state_Machine.state = START; }
    break;
  case INFO_RCV:
    if (byte == FLAG) { state_Machine.state = FLAG_RCV; }
    else if (byte == BCC(state_Machine.addr, state_Machine.ctrl)) { state_Machine.state = DATA_RCV;}
    else { state_Machine.state = START;}
    break;
  case MSG_RCV:
    return ;
  case DATA_RCV:
    if (byte == FLAG) {
      state_Machine.unstuffed_size = unstuffData(state_Machine.message, state_Machine.index, state_Machine.unstuffed_msg);
      if (state_Machine.unstuffed_size < 0) { 
        state_Machine.state = ERROR;
        return;
      }

      char bcc = retrieveBcc(state_Machine.unstuffed_msg, (state_Machine.unstuffed_size - 1), (int) 0);

      char rec_bcc = state_Machine.unstuffed_msg[state_Machine.unstuffed_size - 1];

      if (bcc == rec_bcc) { state_Machine.state = INFO_RCV; }
      else { state_Machine.state = ERROR; }

    }
    else {
      if(state_Machine.index < ((1024 * 2) + 2)) { state_Machine.message[state_Machine.index++] = byte; }
      else { state_Machine.state = ERROR; }
    }
    break;
  case ERROR:
    return;
  case INFO_MSG:
    return;
  case STOP:
    return;
  }
}

int unstuffData(char *buffer, int length, char * unstuffed) {
  int index = 0;
  int unstuffed_index = 0;

  while(index < length) {
    if (buffer[index] == ESC) {
        index++;

        if (buffer[index] == STUFF_FLAG) { unstuffed[unstuffed_index++] = FLAG; } 
        else if (buffer[index] == STUFF_ESC) { unstuffed[unstuffed_index++] = ESC; } 
        else { return -1; }
    } 
    else { unstuffed[unstuffed_index++] = buffer[index]; }

    index++;
  }

  return unstuffed_index;
}

int receiveRead(int fd, char * buffer) {
  state_Machine.state = START;
  state_Machine.index = 0;
  char byte;
  int ret;
  int tries = 0;
  int read_buf_size = 0;
  int msgNr = 0;
  char * read_buf;

  while(1) {
    ret = read(fd, &byte, 1);

    if (ret <= 0) {
      if(tries < 3) {
        state_Machine.state = START;
        state_Machine.index = 0;
        tries++;
        continue;
      }
      else { return -1; }
    }
    else { tries = 0; }

    stateMachine(byte);

    if (state_Machine.state == MSG_RCV) {
      if (state_Machine.ctrl == DISC) {
        return RCV_DISC;
      }
    }
    else if (state_Machine.state == INFO_RCV) {
      if (state_Machine.ctrl == C_S(msgNr)) {
          read_buf = state_Machine.unstuffed_msg;
          read_buf_size = state_Machine.unstuffed_size - 1;

          msgNr++;

      }

      state_Machine.state = START;
      state_Machine.index = 0;

      writeRetry(fd, TRANSMITTER_A, RR(msgNr));
      rrs++;

    }
    else if (state_Machine.state == ERROR) {
      state_Machine.state = START;
      state_Machine.index = 0;

      writeRetry(fd, TRANSMITTER_A, REJ(msgNr));
      rejs++;
    }
  }
}

int writeRetry(int fd, int a, int rr) {
  int tries = 0;
  int ret = 0;

  for (; tries < 3; tries++) {
    ret = send_packet(fd, a, rr);

    if (ret != 5 && tries < 2) {
      sleep(3);
      continue;
    }
    else { return 0; }
  }

  return -1;
}

int llread(int fd, char * buffer) {
  int tries = 0;
  int res = 0;

  res = receiveRead(fd, buffer);

  if (res != RCV_DISC) { return -1; }

  for (; tries < 3; tries++) {
    res = writeRetry(fd, RECEIVER_A, DISC);

    if(res != 0) { continue; }
    res = readS(fd);

    if (res != 5 || state_Machine.ctrl != UA) { continue; }
    else { return 0; }
  }

  return res; //nº read chars
}

int retrievePacketNumber(int length){
  if (MAX_PACKET_SIZE > length) {
    return MAX_PACKET_SIZE;
  }
  else {
    return length;
  }
}



int readS(int fd) {
  state_Machine.state = START;
  state_Machine.index = 0;

  char byte;
  int ret;

  while(state_Machine.state != MSG_RCV) {
    if (state_Machine.state == INFO_RCV || state_Machine.state == ERROR ) {
      state_Machine.state = START;
      state_Machine.index = 0;
    }

    ret = read(fd, &byte, 1);

    if (ret <= 0) { return ret; }

    stateMachine(byte);
    
  }

  return 5;
}

char readResponse(int fd, char msgNrS) {
    int ret = readS(fd);

    if (ret != 5) {
        return msgNrS;
    } else {
        if (state_Machine.ctrl == C_RR0 || state_Machine.ctrl == C_RR1) {
            rrs++;
            return ((state_Machine.ctrl) >> 7);
        } else if (state_Machine.ctrl == C_REJ0 || state_Machine.ctrl == C_REJ1) {
            rejs++;
            return REC_REJ;
        } else {
            return msgNrS;
        }
    }
}

int writeMessage(int fd, message info, char * stuffed, int stuffed_bytes) {
  unsigned int stuffedBcc;
  if(stuffed_bytes == 0) {
      return -1;
  }

  char buffer[(stuffed_bytes + 7) * sizeof(char)];

  buffer[0] = FLAG;
  buffer[1] = info.addr;
  buffer[2] = C_S(info.msgNr);
  buffer[3] = BCC(info.addr, info.msgNr);

  memcpy(buffer + 4, stuffed, stuffed_bytes);

  if (info.bcc == FLAG) {
      stuffedBcc = (ESC << 8) | STUFF_ESC;
  } else if (info.bcc == ESC) {
      stuffedBcc = (ESC << 8) | STUFF_ESC;
  } else {
      stuffedBcc = info.bcc;
  }

  if ((((stuffedBcc) & 0xFF00) >> 8) == ESC) {
      buffer[stuffed_bytes + 4] = (((stuffedBcc) & 0xFF00) >> 8);
      buffer[stuffed_bytes + 5] = (stuffedBcc) & 0xFF00;
      buffer[stuffed_bytes + 6] = FLAG;
  } else {
      buffer[stuffed_bytes + 4] = (stuffedBcc) & 0xFF00;
      buffer[stuffed_bytes + 5] = FLAG;
  }

  int res = write(fd, buffer, stuffed_bytes + 7);
  
  return res != (stuffed_bytes + 7);
}

int tryWrite(int fd, char * stuffed, int stuffed_bytes){
  int tries = 0;
  int answer = 0;
  int bytes = 0;
  char c_s = C_S(info.msgNr);

  do {
    tries++;
    bytes = writeMessage(fd, info, stuffed, stuffed_bytes);

    if(bytes != 0) { return 0; }
    
    answer = readResponse(fd, c_s);

    if(answer == c_s) { continue; } 

    else if(answer == REC_REJ) {
        tries = 0;
        continue;
    }
    else { return 0; }

  } while(tries < 3);

  return -1;
}

int llwrite(int fd, char* buffer, int length) {
  int bytes = 0;
  int data_bytes, stuffed_bytes;
  info.addr = TRANSMITTER_A;
  char stuffed[STUFFED_MAX_SIZE];

  stuffed_bytes = byte_stuffing(buffer, length, stuffed);

  info.bcc = retrieveBcc(buffer, data_bytes, bytes);

  bytes += data_bytes;

  if (tryWrite(fd, stuffed, stuffed_bytes) == -1) { return -1; }

  info.msgNr++;

  return bytes;
}

int llclose(int fd, int status) {
  if (!(status)) { //Transmitter
    printf("Closing connection.\n");
    do {
      send_disc(fd);
      setAlarm(TIMEOUT);
      cFlag = 0;
      receive_ua(fd);
      cancelAlarm();
    } while (cFlag && tries <= MAX_TRIES);
    if (tries >= MAX_TRIES) { return -1;}
    else tries = 1;
    cancelAlarm();
    printf("UA received.\n");
    
  }
  else { //Receiver
    setAlarm(TIMEOUT);
    receive_disc(fd);
    cancelAlarm();
    printf("DISC received.\n");
    printf("Sending UA.\n");
    send_ua(fd);
    sleep(2);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      return FALSE;
    }
    close(fd); 
  }
  return 0; //sucess
}