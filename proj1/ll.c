#include "ll.h"
#include <fcntl.h>
#include <unistd.h>

struct applicationLayer {
int fileDescriptor; /*Descritor correspondente à porta série*/
int status; /*TRANSMITTER | RECEIVER*/
};

int llopen(int porta, int status) {

  char port[256];
  int fd;
  sprintf(port, "/dev/ttyS%s", porta);
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

  if (status) {
    send_set(fd);
    receive_ua(fd);
  }
  else {
    receive_set(fd);
    send_ua(fd);
  }
  



  printf("New termios structure set\n");

  if (/* condition */)
  {
    /* code */
  }
  

  return fd; // link layer identifier
}

int llwrite(int fd, char * buffer, int length) {

  return 0; //nº written chars
}

int llread(int fd, char * buffer) {

  return 0; //nº read chars
}


int llclose(int fd) {

  return 0; //sucess
}