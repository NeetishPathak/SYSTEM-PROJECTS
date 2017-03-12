/* task1.c - main.c */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>
#define TRUE 1
#define FALSE 0
#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }


int ProcCDone = FALSE;
char Order[6];
int count;
void writerA (char *msg, int lck)
{
	kprintf("Writer LA Entry\n");
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock\n", msg);
        sleep (3);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
	kprintf("Writer LA Exit\n");
}


void writerB (char *msg, int lck)
{
        kprintf("Writer LB Entry\n");

		kprintf("Loop running In writer B\n");
		sleep(5);

	kprintf("Writer LB Exit\n");

}


void writerC(char *msg, int lck){
	
	kprintf("Writer LC Entry\n");
	kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock\n", msg);
        sleep (3);
        kprintf ("  %s: to release lock\n", msg);
	releaseall (1, lck);
	kprintf("Writer LC Exit\n");
	
}

void testPrioInheritanceWithLocks1()
{
        int     lck;
        int     wA, wB, wC;
	kprintf("---------------------Lock Test-----------------------------");
        kprintf("\nShow priority Inversion Solved with Priority Inheritance\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Lock Test failed");

        wA = create(writerA, 2000, 10, "readerLock", 2, "A", lck);
        wB = create(writerB, 2000, 20, "readerLock", 2, "B", lck);
        wC = create(writerC, 2000, 30, "writerLock", 2, "C", lck);
	
        kprintf("-start writer A, then sleep 1s. lock granted to writer A (prio 10)\n");
        resume(wA);
        sleep (1);

        kprintf("-start writer B(Prio 20), then sleep 1s. writer B will start running due to higher prio than writer A\n");
        resume(wB);
        sleep (1);
	
        kprintf("-start writer C, then sleep 1s. writer C (prio 30) blocked on the lock\n");
        resume (wC);
	sleep(1);	
        kprintf ("Lock Test COMPLETE\n");
	sleep(10);
	kprintf("------------------------------------------------------------\n");
}


void writerSA (char *msg, int sem)
{
	kprintf("Writer SA Entry\n");

        kprintf ("  %s: to acquire semaphore\n", msg);
        wait (sem);
        kprintf ("  %s: acquired semaphore\n", msg);
        sleep (3);
        kprintf ("  %s: to release semaphore\n", msg);
       	signal(sem);

	kprintf("Writer SA Exit\n");
}


void writerSB (char *msg, int sem)
{
	kprintf("Writer SB Entry\n");
        
                //Writer 2 just runs until Proc 3 finished              
       
                sleep(2);

	kprintf("Writer SB Exit\n");
}

void writerSC(char *msg, int sem){
	
	kprintf("Writer SC Entry\n");

        kprintf ("  %s: to acquire semaphore\n", msg);
        wait(sem);
        kprintf ("  %s: acquired semaphore\n", msg);
        sleep (3);
        kprintf ("  %s: to signal semaphore\n", msg);
        signal(sem);
	
	kprintf("Writer SC Exit\n");

}

void testPrioInheritanceWithSemaphores1(){

	int     sem;
        int     wA, wB, wC;
        kprintf("--------------Semaphore Test------------------------");
        kprintf("\nShow priority Inversion Not Solved in Semaphores\n");
        sem  = lcreate (1);
        assert (sem != SYSERR, "Semaphore test Test failed");

        wA = create(writerSA, 2000, 10, "readerLock", 2, "A", sem);
        wB = create(writerSB, 2000, 20, "readerLock", 2, "B", sem);
        wC = create(writerSC, 2000, 30, "writerLock", 2, "C", sem);

        kprintf("-start writer A, then sleep 1s. lock granted to writer A (prio 20)\n");
        resume(wA);
	sleep(1);
        kprintf("-start writer B, then sleep 1s. writer B will start running due to higher prio than writer A\n");
        resume(wB);
	sleep(1);
        kprintf("-start writer C, then sleep 1s. writer C (prio 30) blocked on the lock\n");
        resume (wC);
        sleep(1);
	sleep(10);
        kprintf ("Semaphore Test COMPLETE\n");
        kprintf("------------------------------------------------------------\n");
}




int main( )
{

	/*The following are test cases to compare the priority inheritance 
	  solution provided in PA2 with the original semaphore based imple-
	  mentation in XINU.
	*/	
	testPrioInheritanceWithLocks1();
	
	sleep(5);

	testPrioInheritanceWithSemaphores1();
        /* The hook to shutdown QEMU for process-like execution of XINU.
 *          * This API call exists the QEMU process.
 *                   */
        shutdown();
}



