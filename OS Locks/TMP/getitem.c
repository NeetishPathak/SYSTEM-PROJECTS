/* getitem.c - getfirst, getlast */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <proc.h>
#include <lock.h>
/*---------------------------------------------------------------------------------------------------------
 *  *  * getMaxPrioFrmLockWaitQ  --  get the maximum scheduling priority from the waiting queue of the lock
 *   *   *--------------------------------------------------------------------------------------------------
 *    *    */
int getMaxPrioFrmLockWaitQ(int lockTail, int lockHead){

	if(isempty(lockHead)){
		return SYSERR;
	}

	int maxPrio = 0;
	int listIndex = q[lockTail].qprev;
	struct pentry *curItem;

	while(listIndex != lockHead){
		
		curItem = &proctab[listIndex];
		
		if(curItem->pprio > maxPrio){
			maxPrio = curItem->pprio;
		}		

		listIndex = q[listIndex].qprev;
	}

	return maxPrio;
}


/*---------------------------------------------------------------------------------------------------------------
 *  *  * getMaxPrioIndexFrmWaitQ  --  get the index of the maximum priority process from the lock's waiting queue
 *   *   *-------------------------------------------------------------------------------------------------------
 *    *    */
int getMaxPrioIndexFrmLockWaitQ(int lockTail, int lockHead){
	
	if(isempty(lockHead)){
                return SYSERR;
        }

        int maxPrio = 0; int maxIndex = SYSERR;
        int listIndex = q[lockTail].qprev;
        struct pentry *curItem;

        while(listIndex != lockHead){

                curItem = &proctab[listIndex];

                if(curItem->pprio > maxPrio){
                        maxPrio = curItem->pprio;
			maxIndex = listIndex;
                }

                listIndex = q[listIndex].qprev;
        }

        return maxIndex;
}

/*------------------------------------------------------------------------------------------------------
 *  * getHighestWriterPrio  --  get the lock priority of the highest Writer in the lock's wiating queue
 *   *--------------------------------------------------------------------------------------------------
 *    */
int getHighestWriterPrio(int lockTail, int lockHead)
{
	if(isempty(lockHead)){
		return SYSERR; //if the waiting q of thelock is empty return SYSERR
	}

	int prioHighestWriter = (q[q[(lockTail)].qprev].qkey);
	int listIndex = q[lockTail].qprev;
	struct pentry *curItem;

	while(listIndex != lockHead){
		
		curItem = &proctab[listIndex];
		if(curItem->plocktype == WRITE){
			prioHighestWriter = q[listIndex].qkey;
			return prioHighestWriter; 
		}
		listIndex = q[listIndex].qprev;
	}
	return SYSERR;
}
/*----------------------------------------------------------------------------------------------------------------
 *  *  * getHighestWriterProcId  --  get the process ID  of the highest Writer Process in the lock's waiting queue
 *   *   *---------------------------------------------------------------------------------------------------------
 *    *    */
int getHighestWriterProcId(int lockTail, int lockHead){
	
	//check if the lockWaiting queue is empty
	if(isempty(lockHead)){
		return SYSERR;
	}

	int highestWriterProcId;
        int listIndex = q[lockTail].qprev;
        struct pentry *curItem;

	while(listIndex != lockHead){

                curItem = &proctab[listIndex];

                if(curItem->plocktype == WRITE){
                        highestWriterProcId = listIndex;	//return the index (pid) of highest Writer process
                        return highestWriterProcId;
                }

                listIndex = q[listIndex].qprev;
        }

     	return SYSERR;
}

/*---------------------------------------------------------------------------------------------------------
 *  *  * getHighestReaderPrio  --  get the lock priority of the highest Reader in the lock's waiting queue
 *   *   *-------------------------------------------------------------------------------------------------
 *    *    */
int getHighestReaderPrio(int lockTail, int lockHead){
	
	//check if the lockWaiting Queue is empty
	if(isempty(lockHead)){
		return SYSERR;
	}

	int prioHighestReader = (q[q[(lockTail)].qprev].qkey);
        int listIndex = q[lockTail].qprev;
        struct pentry *curItem;

        while(listIndex != lockHead){
                curItem = &proctab[listIndex];
                if(curItem->plocktype == READ){
                        prioHighestReader = q[listIndex].qkey;
                        return prioHighestReader;
                }

                listIndex = q[listIndex].qprev;
        }
        return SYSERR;
}

/*----------------------------------------------------------------------------------------------------------------
 *  *  *  * getHighestReaderProcId  --  get the process ID  of the highest Reader Process in the lock's waiting queue
 *   *   *   *---------------------------------------------------------------------------------------------------------
 *    *    *    */
int getHighestReaderProcId(int lockTail, int lockHead){
	
	//check if the lock waiting queue is empty	
	if(isempty(lockHead)){
                return SYSERR;
        }

        int highestReaderProcId;
        int listIndex = q[lockTail].qprev;
        struct pentry *curItem;

        while(listIndex != lockHead){

                curItem = &proctab[listIndex];

                if(curItem->plocktype == READ){
                        highestReaderProcId = listIndex;        //return the index (pid) of highest Reader process
                        return highestReaderProcId;
                }

                listIndex = q[listIndex].qprev;
        }

        return SYSERR;


}

/*------------------------------------------------------------------------
 * getfirst  --	 remove and return the first process on a list
 *------------------------------------------------------------------------
 */
int getfirst(int head)
{
	int	proc;			/* first process on the list	*/

	if ((proc=q[head].qnext) < NPROC)
		return( dequeue(proc) );
	else
		return(EMPTY);
}



/*------------------------------------------------------------------------
 * getlast  --  remove and return the last process from a list
 *------------------------------------------------------------------------
 */
int getlast(int tail)
{
	int	proc;			/* last process on the list	*/

	if ((proc=q[tail].qprev) < NPROC)
		return( dequeue(proc) );
	else
		return(EMPTY);
}
