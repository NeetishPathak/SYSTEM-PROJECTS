This project is about Reader/Writer Lock Implementation with priority inheritance in the XINU OS kernel . A basic understanding of Operating System processes, scheduling and synchronization is desired.

####Motivation:
The reader/writer locks are used to synchronize access to shared data structure. A lock can be acquired for read and write operations. A lock acquired for reading can be shared by other reader processes but a lock acquired for writing should have exclusive access. The scheduling of the processes will happen as per their priority in the system and hence it should be ensured that the system does not enter in a deadlock condition to handle synchronization.

Let me break the problem down for clarity:

1) There is a shared data resource. Let say, a common array (buffer) in which the writer processes want to write and the reader processes want to read from.

2) So a lock will give access to these reader and writer processes to perform their operations

3) The lock is important so that the integrity of data in the buffer is maintained (No overwriting happens).

4)Since reader processes are just interested in reading from the buffer, there can be multiple readers that are reading from the buffer. But the writer process actually modifies the buffer content. So no two writer process should be given access together.

5) Also of the reader process is reading, the writer process should not be given access and vice-versa

6) The processes have some inherent priority based on which they are scheduled. Now, the synchronization mechanism will interfere with the scheduling since the processes may be waiting for acquiring locks. It has to be ensured that the deadlock condition does not arise in such a scenario. For this priority inheritance should be implemented to overcome the priority inversion problem

####Solution Road:

The complete implemented solution for this problem can be found at my git-hub page. 
I will explain the vital parts of the application here. Let's start with the platform and the tools used.

#####Technology Used
The lock implementation is done for XINU OS kernel, which is a lightweight OS originally developed by Dr. Douglas Comer for educational use at Purdue University in the 1980s.More details details about it can be found [here](http://www.xinu.cs.purdue.edu/).

The development in C language and the lock implementation is partly like semaphore implementation originally present in the code.

In order to fully understand the implementation for this code, it is advisable to go through the following files.
For semaphore implementation:
wait.c, signal.c, screate.c, sdelete.c  (These are the original semaphore files in the XINU os)

For lock implementation:
lcreate.c, ldelete.c, linit.c, lock.c, releaseall.c (These are the files that are created for lock implementation)

linit.c : initialize a data structure for stroing locks

'''
struct lentry lockList[NLOCKS];
int nextlock;

void linit(){
	
	struct lentry *lptr;
	int i=0;
	nextlock = NLOCKS-1;
	
	/*initialize all the locks*/

	for(i = 0; i < NLOCKS; ++i){
		
		(lptr = &lockList[i])->lstate = LFREE;
		lptr->lqtail = 1 + (lptr->lqhead = newqueue());
		lptr->lprocbitmask = 0;
		lptr->counter = 0;
	}

}
'''
