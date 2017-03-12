/* getitem.c - getfirst, getlast */

#include <conf.h>
#include <kernel.h>
#include <q.h>

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


/*----------------------------------------------------------------------------
 * getExpKey  --  remove and return the process that has priority just higher
 * 				  than exponential key
 *----------------------------------------------------------------------------
 */

int getExpKey(int tail, double expVal){

	int proc;		/*process to be scheduled next*/
	int compVal = (int) expVal;	/*exponential value to be compared*/

	while((q[proc = q[tail].qprev].qkey) > compVal){
		tail = q[tail].qprev;
	}
	proc = q[proc].qnext;	/*very next process has just higher priority*/

	if(proc < NPROC){		/*it is a valid process*/
		return dequeue(proc);

	}else
		return EMPTY;

}

