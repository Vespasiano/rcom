#ifndef _LL_
#define _LL_

int llopen(int porta, int status);
int llwrite(int fd, char * buffer, int length);
int llread(int fd, char * buffer);
int llclose(int fd);


#endif // _LL_