/*
 * util:   Some utility routines (schedulers and signal handlers)
 *         for the snake demos
 *
 * Author: Dr. Phillip Nico
 *         Department of Computer Science
 *         California Polytechnic State University
 *         One Grand Avenue.
 *         San Luis Obispo, CA  93407  USA
 *
 * Email:  pnico@csc.calpoly.edu
 *
 * Revision History:
 *         $Log: util.c,v $
 *         Revision 1.2  2023-01-28 14:27:44-08  pnico
 *         checkpointing as launched
 *
 *         Revision 1.1  2013-04-02 16:39:24-07  pnico
 *         Initial revision
 *
 *
 */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "snakes.h"
#include "lwp.h"
#include "util.h"


#ifdef OLDSCHEDULERS
/********************************************************
 * This section contains potential scheduling algorithms
 ********************************************************/
int OLD_AlwaysZero() {
  /* always run the first one */
  return 0;
}

int SIGTSTPcounter = 0;
int OLD_ChangeOnSIGTSTP() {
  /* Move to the next one, counting TSTPs*/
  return SIGTSTPcounter%lwp_procs;
}

int OLD_ChooseLowestColor(){
  /* choose the lowest available color.  Round-robin within
   * colors.  (I.e. Always advance)
   * We'll do the round-robin within the snake array
   */
  int i,color,next;
  snake s;

  color = MAX_VISIBLE_SNAKE+1;
  for(i=0;i<lwp_procs;i++) {
    s = snakeFromLWpid(lwp_ptable[i].pid);
    if ( s && s->color < color )
      color = s->color;
  }
  next = -1;
  if ( color ){                 /* find the next one of the given color */
    next = (lwp_running+1)%lwp_procs;
    s = snakeFromLWpid(lwp_ptable[next].pid);
    while(s && (s->color != color) ) {
      next = (next+1)%lwp_procs;
      s = snakeFromLWpid(lwp_ptable[next].pid);
    }
  }

  return next;
}

int OLD_ChooseHighestColor(){
  /* choose the highest available color.  Round-robin within
   * colors.  (I.e. Always advance)
   * We'll do the round-robin within the snake array
   */
  int i,color,next;
  snake s;

  color = 0;
  for(i=0;i<lwp_procs;i++) {
    s = snakeFromLWpid(lwp_ptable[i].pid);
    if ( s && s->color > color )
      color = s->color;
  }
  next = -1;
  if ( color ){                 /* find the next one of the given color */
    next = (lwp_running+1)%lwp_procs;
    s = snakeFromLWpid(lwp_ptable[next].pid);
    while(s && (s->color != color) ) {
      next = (next+1)%lwp_procs;
      s = snakeFromLWpid(lwp_ptable[next].pid);
    }
  }

  return next;
}


/********************************************************
 * End scheduling algorithms
 * Now the signal handling material
 ********************************************************/

void SIGTSTP_handler(int num){
  SIGTSTPcounter++;              /* increment the counter */
}
#endif

void SIGINT_handler(int num){
  kill_snake();                 /* mark a snake for death */
}

void SIGQUIT_handler(int num){
}

void install_handler(int sig, sigfun fun){
  /* use sigaction to install a signal handler */
  struct sigaction sa;

  sa.sa_handler = fun;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;


  if ( sigaction(sig,&sa,NULL) < 0 ) {
    perror("sigaction");
    exit(-1);
  }
}

