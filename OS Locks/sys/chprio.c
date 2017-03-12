/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */

SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	
	pptr->pprio = newprio;
	
	
	//does this process belong to waiting queue of a lock
	if(pptr->pstate == PRLCKWAIT){

		int maxPriority = getMaxPrioFrmLockWaitQ(lockList[pptr->plock].lqtail, lockList[pptr->plock].lqhead);	
		rampUpPrio(&lockList[pptr->plock], maxPriority);
	
	}
	
	restore(ps);
	return(newprio);
}
