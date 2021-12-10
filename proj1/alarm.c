#include <stdlib.h>

#include "alarm.h"
#include "defines.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void answer(int signal) {
  if(signal != SIGALRM)
    return;

  printf("TIMEOUT with Alarm #: %d\n", tries + 1);
	cFlag = TRUE;
	tries++;
}

void setAlarm() {
	struct sigaction sa;
	sa.sa_handler = &answer;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGALRM, &sa, NULL);

	cFlag = 0;

	alarm(TIMEOUT); // creates alarm
}

void cancelAlarm() {

	struct sigaction action;
	action.sa_handler = NULL;
	sigaction(SIGALRM, &action, NULL);

	alarm(0); // cancels pending alarms
}