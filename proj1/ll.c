#include "ll.h"
#include "extras.h"
#include "defines.h"
#include "alarm.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <strings.h>

#define BAUDRATE B38400

struct termios oldtio, newtio;

struct applicationLayer {
int fileDescriptor; /*Descritor correspondente à porta série*/
int status; /*TRANSMITTER | RECEIVER*/
};

l_Control lControl;

int llopen(int porta, int status) {

  char port[256];
  int fd;

  lControl.N_s = 0;

  sprintf(port, "/dev/ttyS%i", porta);
  fd = open(port, O_RDWR | O_NOCTTY);
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

  if (status) { //TRANSMITTER
    do
    {
      send_set(fd);
      setAlarm(TIMEOUT);
      cFlag = 0;
      receive_ua(fd);
      cancelAlarm();  
    } while (cFlag && tries <= MAX_TRIES);
    if (tries >= MAX_TRIES) return -1;
    else tries = 1; 
    cancelAlarm();


  }
  else { //RECEIVER
    receive_set(fd);
    send_ua(fd);
  }

  printf("New termios structure set\n");

  return fd; // link layer identifier
}

int llread(int fd, char * buffer) {

  return 0; //nº read chars
}

int llwrite(int fd, char * buffer, int length) {
  lControl.N_s = lControl.N_s ^ 1;

  int bytes;

  int cFlag = TRUE;
  unsigned int framePosition;
  unsigned int nTries = 0;
  unsigned char frame[FRAME_SIZE(length)];

  framePosition = 4;
  frame[0] = FLAG;
  frame[1] = TRANSMITTER_A;
  if(lControl.N_s == 0) { frame[2] = C10; }
  else { frame[2] = C11; }
  frame[3] = BCC(TRANSMITTER_A, frame[2]);

  unsigned int packetPosition = 0;
  unsigned char currentByte;
  while (packetPosition < length) {
    currentByte = buffer[packetPosition++];
    if(currentByte == FLAG || currentByte == ESC) {
      frame[framePosition++] = ESC;
      frame[framePosition++] = currentByte ^ STUFF;
    }
    else { frame[framePosition++] = currentByte; }
  }
  
  unsigned char bcc2 = 0;
  for (int i = 0; i < length; i++) { bcc2 ^= buffer[i]; }

  if (bcc2 == FLAG || bcc2 == ESC) {
    frame[framePosition++] = ESC;
    frame[framePosition++] = bcc2 ^ STUFF;
  }
  else { frame [framePosition++] = bcc2;}
  frame[framePosition++] = FLAG;

  // do 
  // {
  //   bytes = write(fd, frame, framePosition);

  //   setAlarm(TIMEOUT);
  //   cFlag = 0;

  //   if ()
  //   {
  //     /* code */
  //   }
    
    
  // } while (/* condition */);
  
  

  return bytes; //nº written chars
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