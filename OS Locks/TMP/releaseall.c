/* releaseall.c - releaseall */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>
#define TRUE 	1
#define FALSE	0


int getMaxPrioFrmRemLocks(struct pentry* pptr){
	
	int newPrio = 0;
	if(pptr->plockbitmask == 0){
		//since the process does not hold any other locks
		//resume to original priority
		newPrio = pptr->pinh;
	}else{
		//the process still holds some locks
		int i=0;
		long long bMask = pptr->plockbitmask;

		while(bMask != LL_ZERO){
			
			if(bMask & LL_ONE){
				int nPrio = getMaxPrioFrmLockWaitQ(lockList[i].lqtail, lockList[i].lqhead);
				if(nPrio > newPrio){
					newPrio = nPrio;
				}
			}

			bMask = bMask >> LL_ONE;
			i = i+1;
		}		
	}
	
	return newPrio;
}


/*---------------------------------------------------------------------------
 *  * releaseall  --  release a lock, releasing one or more waiting process
 *   *------------------------------------------------------------------------
 *    */


int releaseall (numlocks, args)
int numlocks;	/*no. of locks to be released*/
int args;	/*arguments treated like an array in the code*/	
{
	STATWORD ps;    
	
	register struct	lentry	*lptr;
	unsigned long	*a;		/* points to list of args	*/

	int lockHeld = TRUE;

	disable(ps);

	/* push arguments */
	
	a = (unsigned long *)(&args) + (numlocks-1); /* last argument	*/
	for ( ; numlocks > 0 ; numlocks--){	/* machine dependent; copy args	*/
		int lckDes = (int)(*a--);	/* onto created process' stack	*/
		
		lckDes = lckDes % NLOCKS;
			
		if (isbadlock(lckDes) || ((lptr= &lockList[lckDes])->lstate==LFREE)) {
                	lockHeld = FALSE;
			continue;
        	}
					
		if ((lptr->lprocbitmask) != 0){

			//the current process is leaving the lock
			lptr->lprocbitmask &= ~(LL_ONE << currpid);
			//the process no longer holds the current lock
			proctab[currpid].plockbitmask &= ~(LL_ONE << lckDes);
			
			//get the new priority for the current process
			proctab[currpid].pprio = getMaxPrioFrmRemLocks(&proctab[currpid]);	
			
			if((lptr->lprocbitmask) == 0){//now the lock is available
                		
				//schedule the other process based on locking policy
				int hWtrPrio = getHighestWriterPrio(lptr->lqtail, lptr->lqhead);
				int hRdrPrio = getHighestReaderPrio(lptr->lqtail, lptr->lqhead); 
				
				//select the process to be scheduled based on lock priority
				if(hWtrPrio > hRdrPrio){
					
					int hWtrPId = getHighestWriterProcId(lptr->lqtail, lptr->lqhead);
					
					if(FALSE == isbadpid(hWtrPId)){

						ready(dequeue(hWtrPId), RESCHYES);
					}
				
				}else if(hWtrPrio < hRdrPrio){
					
					while(hRdrPrio >= hWtrPrio){
						
						int hRdrPId = getHighestReaderProcId(lptr->lqtail, lptr->lqhead);
						
	                                        if(FALSE == isbadpid(hRdrPId)){	ready(dequeue(hRdrPId), RESCHNO); }else{break;}
						
						hRdrPrio = getHighestReaderPrio(lptr->lqtail, lptr->lqhead);
					}
					//Now that all the reader processes are put in service queue
					//Reschdule
					resched();
				
				}else if(hWtrPrio == hRdrPrio){

					int hWtrPId = getHighestWriterProcId(lptr->lqtail, lptr->lqhead);
					int hRdrPId = getHighestReaderProcId(lptr->lqtail, lptr->lqhead);
					
					if( (FALSE == isbadpid(hWtrPId)) && (FALSE == isbadpid(hRdrPId)) ){
						
						int hWtrWaitTime = proctab[hWtrPId].plocktime;
						int hRdrWaitTime = proctab[hRdrPId].plocktime;
						if((hWtrWaitTime -  hRdrWaitTime < 1000) && (hWtrWaitTime - hRdrWaitTime > -1000)){
							//Writer should be given preference if waiting time is within 1000 ms
							ready(dequeue(hWtrPId), RESCHYES);
						}else{

							if(hWtrWaitTime < hRdrWaitTime){
								//Writer's waiting time is more
								ready(dequeue(hWtrPId), RESCHYES);	
							}else{
								//Reader's waiting time is more
								while(hRdrPrio >= hWtrPrio){

                                        	 	       		hRdrPId = getHighestReaderProcId(lptr->lqtail, lptr->lqhead);

                                	                		if(FALSE == isbadpid(hRdrPId)){
										 ready(dequeue(hRdrPId), RESCHNO); 
									}else{
										break;
									}

                        	                        		hRdrPrio = getHighestReaderPrio(lptr->lqtail, lptr->lqhead);
                                       			
								 }
								//Now that all the reader processes are put in service queue
								//Reschedule
								resched();
							}
						}		
					}
				}				
			}
		}else{
			//kprintf("Lock is already Available\n");
		}	
	}

	restore(ps);

	if(FALSE == lockHeld)
	return(SYSERR);
	
	return(OK);
}

