#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "thread.h"
#include "semaphor.h"
#include "event.h"

//Users main function
int userMain(int argc, char *argv[]);

//tick() function
void tick();

//Thread safe print
int syncPrintf(const char *format, ...);

//Memory error handler
void memError(const char *msg);

//General error handler
void genError(const char *msg);

#endif
