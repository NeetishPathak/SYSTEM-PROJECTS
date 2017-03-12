/* lcreate.c - lcreate, newlock */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 *  * screate  --  create and initialize a lock, returning its id
 *   *------------------------------------------------------------------------
 *    */
SYSCALL lcreate(void)
{
	STATWORD ps;    
	int	lock;

	disable(ps);

	if ( (lock=newlock())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}

	/* sqhead and sqtail were initialized at system startup */
	lockList[lock].lprocbitmask = 0;
	lockList[lock].counter += NLOCKS;
	restore(ps);
	return(lockList[lock].counter + lock);
}

/*------------------------------------------------------------------------
 *  * newlock  --  allocate an unused lock  and return its index
 *   *------------------------------------------------------------------------
 *    */
LOCAL int newlock()
{
	int	lock;
	int	i;

	for (i=0 ; i<NLOCKS ; ++i) {
		
		lock = NLOCKS - 1 - i;

		/*lock = nextlock--;
		
		if (newlock < 0){
			nextlock = NLOCKS-1;
			
		}
		*/
		if (lockList[lock].lstate == LFREE) {
			lockList[lock].lstate = LUSED;
			return(lock);
		}
	}
	return(SYSERR);
}

