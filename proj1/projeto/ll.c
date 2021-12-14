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
    printf("Before send SET \n");
    send_packet(fd, TRANSMITTER_A, SET);
    printf("After send SET \n");
    receive_packet(fd);
    printf("After receive UA \n");
  }
  else {
    printf("Before receive SET \n");
    receive_packet(fd);
    printf("After receive SET \n");
    send_packet(fd, RECEIVER_A, UA);
    printf("After send UA \n");
  }
    


//   if (status == 0) { //TRANSMITTER
//     do
//     {
//       send_set(fd);
//       setAlarm(TIMEOUT);
//       cFlag = 0;
//       receive_ua(fd);
//       cancelAlarm();  
//       printf("cflag is %i and tries is %i\n", cFlag, tries);
//     } while (cFlag && tries <= MAX_TRIES);
//     if (tries >= MAX_TRIES) return -1;
//     else tries = 1; 
//     cancelAlarm();


//   }
//   else { //RECEIVER
//     do
//     {
//       receive_set(fd);
//       setAlarm(TIMEOUT);
//       cFlag = 0;
//       send_ua(fd);
//       cancelAlarm();  
//     } while (cFlag && tries <= MAX_TRIES);
//     if (tries >= MAX_TRIES) return -1;
//     else tries = 1; 
//     cancelAlarm();
//   }

  printf("New termios structure set\n");

  return fd; // link layer identifier
}