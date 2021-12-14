#ifndef _LL_
#define _LL_

#include "defines.h"

typedef struct {
  unsigned char addr;
  unsigned char msgNr;
  char bcc;
} message;

typedef struct {
	unsigned int N_s;
	unsigned int framesSent;
	unsigned int framesReceived;
	unsigned int RRsent;
	unsigned int RJsent;
	unsigned int RRreceived;
	unsigned int RJreceived;
}l_Control;
extern l_Control lControl;



extern int cFlag;
extern int tries;

int llopen(int porta, int status);

int llwrite(int fd, char * buffer, int length);

int llread(int fd, char * buffer);

int llclose(int fd, int status);

#endif // _LL_