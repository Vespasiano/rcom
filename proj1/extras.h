#ifndef _EXTRAS_
#define _EXTRAS_

int send_packet(int fd, int A, int C);

int receive_packet(int fd, int A, int C);

int send_set(int fd);

int send_ua(int fd);

int receive_set(int fd);

int receive_ua(int fd);


#endif // _EXTRAS_