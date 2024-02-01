#ifndef UTILH
#define UTILH

typedef void (*sigfun)(int signum);

extern void SIGINT_handler(int num);
extern void SIGQUIT_handler(int num);
extern void SIGTSTP_handler(int num);
extern void install_handler(int sig, sigfun fun);

#ifdef OLDSCHEDULERS
extern int AlwaysZero();
extern int ChangeOnSIGTSTP();
extern int ChooseLowestColor();
extern int ChooseHighestColor();
#else
#include "schedulers.h"
#endif

#endif
