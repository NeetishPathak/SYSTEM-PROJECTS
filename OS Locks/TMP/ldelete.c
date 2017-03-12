/* ldelete.c - ldelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 *  * ldelete  --  delete a lock by releasing its table entry
 *   *------------------------------------------------------------------------
 *    */
SYSCALL ldelete(int lockdescriptor)
{
	STATWORD ps;    
	int	pid;
	struct	lentry	*lptr;
	struct  pentry *pptr;

	disable(ps);
	
	lockdescriptor %= NLOCKS;
		
	if ( (isbadlock(lockdescriptor)) || (lockList[lockdescriptor].lstate==LFREE) ) {
		restore(ps);
		return(SYSERR);
	}

	
	lptr = &lockList[lockdescriptor];
	
	lptr->lstate = LFREE;
	
	//Change the bitmask of all the processes holding this lock
	int i=0;
	
	for(i =0; i<NPROC; ++i){

		pptr = &proctab[i];
	
		pptr->plockbitmask &= ~(LL_ONE << lockdescriptor); 
	}
	
	//make the bit mask for this lock = 0
	lptr->lprocbitmask = 0;
	
	//if there are processes in the waiting queue of the lock, make them ready
	if (nonempty(lptr->lqhead)) {
		
		while( (pid=getfirst(lptr->lqhead)) != EMPTY)
		  {
		    proctab[pid].plockwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		
		resched();
	}

	restore(ps);
	return(OK);
}

