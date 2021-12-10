#ifndef _EXTRAS_
#define _EXTRAS_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "defines.h"

int send_packet(int fd, int A, int C);

int receive_packet(int fd, int A, int C);

int send_set(int fd);

int send_ua(int fd);

int send_disc(int fd);

int send_rr0(int fd);

int send_rr1(int fd);

int send_rej0(int fd);

int send_rej1(int fd);

int receive_set(int fd);

int receive_ua(int fd);

int receive_disc(int fd);

int receive_rr0(int fd);

int receive_rr1(int fd);

int receive_rej0(int fd);

int receive_rej1(int fd);

information_state trama_check(information_state state, char byte, int C);

#endif // _EXTRAS_