/* lock.c - lock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

void rampUpPrio(struct lentry *lptr, int newPrio){
	
	if(lptr->lprocbitmask == LL_ZERO){
		return SYSERR;
	}
	
	long long bMask = lptr->lprocbitmask;
	int i=0;
	while((bMask != LL_ZERO) && (i < NPROC)){
		
		if(bMask & LL_ONE){	
				
			if(newPrio !=  SYSERR){
				proctab[i].pprio = newPrio;
			}else{
				proctab[i].pprio = proctab[i].pinh;
			}

			if( (proctab[i].pstate == PRLCKWAIT)){
				rampUpPrio(&lockList[proctab[i].plock], newPrio);
			}
			
		}
		
		bMask = bMask >> LL_ONE; 
		i = i + 1;
	}
}

/***************************************************************************
 *getReadWriteType : get READ or WRITE type process is holding the lock
 *
 * *************************************************************************/
int getReadWriteType(struct lentry *lptr){

	long long bitmask = lptr->lprocbitmask;
	int i=0;
	//for(i=0; i<NPROC; i = i+1){
	
	while((bitmask != LL_ZERO) && (i < NPROC)){
		
		if(bitmask & LL_ONE){
			return proctab[i].plocktype;
		}

		bitmask = bitmask >> LL_ONE;
		i = i+1;
	}	
	return SYSERR; //lock is actually not held
}


//Utility function for printing BitMask
void printBitMask(long long bm){

	long long bmask = bm;
	while(bmask != LL_ZERO){
		if(bmask & LL_ONE){
			printf("1");
		}else{
			printf("0");
		}
		bmask = bmask >> LL_ONE;	
	}	
	return;
}


/*------------------------------------------------------------------------
 *  * lock  --  make current process locked on a lock
 *   *------------------------------------------------------------------------
 *    */
int lock(int ldes1, int type, int priority)
{
	
	int bigDes = ldes1;
	ldes1 = (ldes1 % NLOCKS);
	
	STATWORD ps;    
	struct	lentry	*lptr;
	struct	pentry	*pptr;

	disable(ps);
	

	if(bigDes != (ldes1 + lockList[ldes1].counter)){
                restore(ps);
                return (SYSERR);
        }


	lptr = &lockList[ldes1];
	
	if (isbadlock(ldes1) || (lptr->lstate==LFREE) || ((&proctab[currpid])->plockwaitret == DELETED) ) {
		restore(ps);
		return(SYSERR);
	}
	
	pptr = &proctab[currpid];
	
	if (lptr->lprocbitmask != 0) { //lock is not free
		//kprintf("The lock is not free\n");
	
		if(READ == getReadWriteType(lptr)){
			//kprintf("The lock is of read type\n");
			if(READ == type){

        	                int highestPrioWriter = getHighestWriterPrio(lptr->lqtail, lptr->lqhead);
				//kprintf("priority of highest Writer %d myprio = %d\n", highestPrioWriter, priority);
	                	if(highestPrioWriter <= priority){
					//kprintf("So entering\n");
					 //The current process can acquire the lock
					 lptr->lprocbitmask |= (LL_ONE << currpid);
					 //The current process now holds the lock given by lock descriptor
					 pptr->plockbitmask |= (LL_ONE << ldes1);
                                         //process type (READ/WRITE) the type of the lock
                                         pptr->plocktype = type;
					 pptr->plock = EMPTY;
					restore(ps);
					return(OK);	
                        	}else{
					//The current process should wait since the READer process
					//priority is not as high as WRITEr process priority
				}       

	                }else if(WRITE == type){
				//the current process should wait as WRITE request can't be 
				//granted when READ process holds the lock	
                	}

		}else if(WRITE == getReadWriteType(lptr)){

			//the current process should wait as WRITE locks are exclusive
			//carry out default wait sequence
		}
		pptr->plock = ldes1;	  //this process will wait on lock given by lock descriptor		
		pptr->pstate = PRLCKWAIT; //the state of the process, now the process is waiting for a lock
		pptr->plocktype = type;	  //the waiting process type is type (READ or WRITE)
		pptr->plocktime = ctr1000; //the process was pushed in waiting Q at this time
		insert(currpid, lptr->lqhead, priority); //put the process in lock queue with key as priority
                pptr->plockwaitret = OK;
	        	
		int maxWaitingPrio = getMaxPrioFrmLockWaitQ(lptr->lqtail, lptr->lqhead);
		rampUpPrio(lptr,maxWaitingPrio);
		resched();

              	/*To meet the deletion condition*/
	        /*If the lock is deleted by now, return DELETED*/
        	if((&lockList[ldes1])->lstate == LFREE){
                        pptr->plockwaitret = DELETED;
			restore(ps);
			return pptr->plockwaitret;
	        }else{
        	        pptr->plockwaitret = OK;
               	}
	        			
	}	
		//kprintf("The lock is free\n");
		//The current process is now acquiring the lock
		//Therfore, the current lock is held by the current process
		//kprintf("lock bits mask before setting is");
		//printBitMask(lptr->lprocbitmask);
		//kprintf("\n");
		
		lptr->lprocbitmask |= (LL_ONE << currpid);

		//kprintf("lock bit mask after setting is ");
		//printBitMask(lptr->lprocbitmask);
		//kprintf("\n");
	
		//kprintf("lock process bit  mask before setting is");
                //printBitMask(pptr->plockbitmask);
                //kprintf("\n");

		
		//The current process now holds the lock given by lock descriptor
		pptr->plockbitmask |= (LL_ONE << ldes1);
		
		//kprintf("proc bits mask after  setting is");
                //printBitMask(pptr->plockbitmask);
                //kprintf("\n");


		//save the type (READ/WRITE) for the process

		pptr->plocktype = type;
		pptr->plock = EMPTY; //the process is not waiting on a lock now
		//kprintf("type is %d\n", pptr->plocktype);

	restore(ps);
	//return pptr->plockwiatret;
	return(OK);
}

