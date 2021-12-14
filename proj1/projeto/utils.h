#ifndef _utils_
#define _utils_

#include "macros.h"

typedef struct
{
  state_info state;
  char addr;
  char ctrl;
} state_machine;

int send_packet(int fd, int A, int C);

int receive_packet(int fd);

void stateMachine(unsigned char byte);

#endif // _utils_