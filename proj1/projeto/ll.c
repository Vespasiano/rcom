#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <strings.h>

#include "ll.h"
#include "utils.h"

struct termios oldtio, newtio;

int llopen(int porta, int status) {

  char port[256];
  int fd;

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

  if (status == 0) {
    send_packet(fd, TRANSMITTER_A, SET);
    receive_packet(fd);
  }
  else {
    receive_packet(fd);
    send_packet(fd, RECEIVER_A, UA);
  }

  printf("New termios structure set\n");

  return fd; // link layer identifier
}

// int llclose(int fd, int status) {
//   if (!(status)) { //Transmitter
//     printf("Closing connection.\n");
//     send_disc(fd);
//     receive_ua(fd);
//   }
//   else { //Receiver
//     receive_disc(fd);
//     send_ua(fd);
//     sleep(2);
//     if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
//       perror("tcsetattr");
//       return FALSE;
//     }
//   }
//   close(fd); 
//   return 0; //sucess
// }