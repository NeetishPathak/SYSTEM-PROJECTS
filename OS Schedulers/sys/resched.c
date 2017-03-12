/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <sched.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int sched_type = DEFAULT;
int epochStarted = FALSE;
int epochCounter = 0;
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */


void setEpochQuanta(){
	int procId = NULLPROC;
	if(TRUE == epochStarted){
		for(procId = NULLPROC; procId < NPROC; ++procId){
			if(proctab[procId].counter != proctab[procId].quantum){
				proctab[procId].quantum = proctab[procId].pprio + (proctab[procId].counter)/2;
			}else{
				proctab[procId].quantum = proctab[procId].pprio;
			}
			proctab[procId].counter = proctab[procId].quantum;
			epochCounter += proctab[procId].counter;
			
			//only processes in the current and ready state can be a part of epoch
			if(proctab[procId].pstate == PRREADY || proctab[procId].pstate == PRCURR){
				proctab[procId].activeInEpoch = TRUE;	//this process is active in current epoch  
			}else{
				proctab[procId].activeInEpoch =	FALSE;	//this process in inactive in current epoch
			}
		}
	}
}

/*************************************
This function initializes the epoch
*************************************/
void initializeEpoch(){
	epochStarted = TRUE;
	setEpochQuanta();
}

/************************************************
This function checks if the current epoch is over
************************************************/
int isEpochFinished(){
	if((epochCounter <= 0) || (lastkey(rdytail) == 0)){
		return TRUE;
	}
	return FALSE;
}

/****************************************************************
Function to calculate the goodness(dynamic priority) of a process
****************************************************************/
int calculateGoodness(){
	
	int rdyHeadCopy = rdyhead;
	int rdyTailCopy = rdytail;
	int maxGoodnessId = 0;
	int maxGoodnessVal = 0;
	
	while(q[rdyHeadCopy].qnext != rdyTailCopy){
	
		if(proctab[q[rdyHeadCopy].qnext].activeInEpoch == TRUE){
			
			if(proctab[q[rdyHeadCopy].qnext].counter == 0){
				proctab[q[rdyHeadCopy].qnext].goodness = 0;	
			}else{
				proctab[q[rdyHeadCopy].qnext].goodness = proctab[q[rdyHeadCopy].qnext].counter + \
									 proctab[q[rdyHeadCopy].qnext].pprio;
			}

			if(maxGoodnessVal < proctab[q[rdyHeadCopy].qnext].goodness){
				maxGoodnessVal = proctab[q[rdyHeadCopy].qnext].goodness;
				maxGoodnessId = q[rdyHeadCopy].qnext;
			}
		}
		
		rdyHeadCopy = q[rdyHeadCopy].qnext;
	}

	return maxGoodnessId;
}


/************************************************************************
setschedclass(int sched_class): setting the class for scheduling
************************************************************************/
void setschedclass (int sched_class){
	sched_type = sched_class;	

	if(sched_type == LINUXSCHED){
		initializeEpoch();
	}
}

/************************************************************************
getschedclass(): to return the current class for scheduling
************************************************************************/
int getschedclass(){
	return sched_type;
}

/************************************************************************
resched(): rescheduling method
************************************************************************/
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	
	if( EXPDISTSCHED == sched_type ){
		double exp = expdev(0.1);
		int expVal = exp + 0;
		
		optr= (&proctab[currpid]);
                int oldPid = currpid;

		//force current process in the queue
                if(optr->pstate == PRCURR){
                        optr->pstate = PRREADY;
                        insert(oldPid, rdyhead, optr->pprio);
                }

	
		if(lastkey(rdytail) <= expVal) {
		
			nptr = &proctab[(currpid) = getlast(rdytail)];

		}else if(firstkey(rdyhead) >= expVal){

			nptr = &proctab[(currpid) = getfirst(rdyhead)];

		}else{
			nptr = &proctab[(currpid) = getExpKey(rdytail, expVal)];
		}

		//force context switch
		/*if(optr->pstate == PRCURR){
			optr->pstate = PRREADY;
			insert(oldPid, rdyhead, optr->pprio);
		}*/

		nptr->pstate = PRCURR;
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	}else if(LINUXSCHED == sched_type){
		
		//capture the current process

		optr= (&proctab[currpid]);
                int oldPid = currpid;

                //force current process in the queue
                if(optr->pstate == PRCURR){
                        optr->pstate = PRREADY;
                        insert(oldPid, rdyhead, optr->pprio);
                }

		//decrement the epochCounter
                epochCounter -= (optr->counter - preempt) ;

		//decrement the counter for the current process
		optr->counter = preempt;

		//check if the epoch is finished
		if(isEpochFinished()){
			setEpochQuanta();			//set the epochQuanta if it is over
		}
		
		//calculate the dynamic priorities(goodness)
		int maxGoodnessProc = calculateGoodness();		//set the goodness of each process & return maxGoodnessId
		
		//dequeue the maxGoodness item
		dequeue(maxGoodnessProc);
		
		//set the new preempt
		nptr = &proctab[(currpid) = maxGoodnessProc];
		nptr->pstate = PRCURR;

		//do a contextswitch for new process having highest goodness
		#ifdef  RTCLOCK
                        preempt = nptr->counter;              /* reset preemption counter     */
                #endif
                ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	}else{

		/* no switch needed if current process priority higher than next*/

		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
		   (lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}
	
		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		
		#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
	
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
		/* The OLD process returns here when resumed. */

	
	}

	return OK;
}
