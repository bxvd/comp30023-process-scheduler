#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "sys.h"

#define EPOCH 60

#define ceil(x) (x > (float)((int)x) ? (int)x + 1 : (int)x)

typedef enum notification { RUN, FINISH, EVICT } Notification;

void notify(Notification n, System sys, int var, ...);

#endif
