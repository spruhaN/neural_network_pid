/*
 * snake:  This is a demonstration program to investigate the viability
 *         of a curses-based assignment.
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
 *         $Log: hungrysnakes.c,v $
 *         Revision 1.7  2023-01-28 14:36:33-08  pnico
 *         Summary: lwp_create() no longer takes a size
 *
 *         Revision 1.6  2023-01-28 14:27:44-08  pnico
 *         checkpointing as launched
 *
 *         Revision 1.5  2013-04-24 11:45:28-07  pnico
 *         pesky longlines.
 *
 *         Revision 1.4  2013-04-13 06:38:58-07  pnico
 *         doubled initial stack size
 *
 *         Revision 1.3  2013-04-10 11:22:27-07  pnico
 *         turned off the gdb sleep
 *
 *         Revision 1.2  2013-04-07 12:13:43-07  pnico
 *         Changed new_lwp() to lwp_create()
 *
 *         Revision 1.1  2013-04-02 16:39:24-07  pnico
 *         Initial revision
 *
 *         Revision 1.2  2004-04-13 12:31:50-07  pnico
 *         checkpointing with listener
 *
 *         Revision 1.1  2004-04-13 09:53:55-07  pnico
 *         Initial revision
 *
 *         Revision 1.1  2004-04-13 09:52:46-07  pnico
 *         Initial revision
 *
 */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/time.h>
#include "snakes.h"
#include "lwp.h"
#include "util.h"

#define MAXSNAKES  100

int main(int argc, char *argv[]){
  int i,cnt,err;
  snake s[MAXSNAKES];

  err = 0;
  for (i=1;i<argc;i++) {                /* check options */
    fprintf(stderr,"%s: unknown option\n",argv[i]);
    err++;
  }
  if ( err ) {
    fprintf(stderr,"usage: %s\n",argv[0]);
    fprintf(stderr," SIGINT(^C) handler calls kill_snake().\n");
    fprintf(stderr," SIGQUIT(^\\) handler calls lwp_stop().\n");
    exit(err);
  }

  install_handler(SIGINT, SIGINT_handler);   /* SIGINT will kill a snake */
  install_handler(SIGQUIT,SIGQUIT_handler);  /* SIGQUIT will end lwp     */


  /* wait to gdb
  fprintf(stderr,"%d\n",getpid());
  sleep(5);
  */

  #ifdef FAST_SNAKES
  /*PLN*/set_snake_delay(1);
  #endif

  start_windowing();            /* start up curses windowing */

  /* Initialize Snakes */
  cnt = 0;
  /* snake new_snake(int y, int x, int len, int dir, int color) ;*/
  s[cnt++] = new_snake( 8,30,10, E,1);/* all start at color one */
  s[cnt++] = new_snake(10,30,10, E,1);
  s[cnt++] = new_snake(12,30,10, E,1);
  s[cnt++] = new_snake( 8,50,10, W,1);
  s[cnt++] = new_snake(10,50,10, W,1);
  s[cnt++] = new_snake(12,50,10, W,1);
  s[cnt++] = new_snake( 4,40,10, N,1);

  /* Draw each snake */
  draw_all_snakes();

  /* turn each snake loose as an individual LWP */
  for(i=0;i<cnt;i++) {
    s[i]->lw_pid = lwp_create((lwpfun)run_hungry_snake,(void*)(s+i));
  }

  lwp_start();                    

  for(i=0;i<cnt;i++)
    lwp_wait(NULL);

  end_windowing();              /* close down curses windowing */

  printf("Goodbye.\n");         /* Say goodbye, Gracie */
  return err;
}

