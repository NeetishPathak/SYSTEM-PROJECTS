###READER/WRITER LOCKS

This project is about Reader/Writer Lock Implementation with priority inheritance in the XINU OS kernel . A basic understanding of Operating System processes, scheduling and synchronization is desired.
A detailed version of the problem statement is found [here](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Locks/Problem_Statement.md).

To build the project, just use make command to run the [Makefile](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Locks/compile/Makefile) file

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
lcreate.c, ldelete.c, linit.c, lock.c, releaseall.c (These are the files that are created for lock implementation

Let's look how a lock struct looks like. Please look at the file [lock.h](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Locks/h/lock.h) for other macros and prototypes

	struct lentry{
	char lstate;	/*state of the lock*/
	int lpriol;	/*priority of the lock*/
	int lcnt;	/*count of the lock*/
	int lqhead;	/*head of the lock*/
	int lqtail;	/*tail of the lock*/
	int ltype;	/*type of the lock*/ (Reader or writer)
	long long lprocbitmask; /* bitmask to store all the processes holding this lock*/
	int counter;	/*to keep track of the number of times this lock entry is initiated*/
	};
	
In this system, the total number of allowed locks are 50. Hence, a counter can be handy to keep a track of number of times a lock is created. Also lprocbitmask is can be useful to keep track of all the processes that are acquiring this lock. The number of processes supported in the system at any moment is 50. So bitmask length of 64 is sufficient to keep a check if a particular process is acquiring this lock.
 
Implementation files:

[linit.c](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Locks/sys/linit.c)
This file initializes all the locks with initial default state as FREE

[lcreate.c](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Locks/sys/lcreate.c)
The function lcreate.c would an available LOCK ID. It changes the state of the lock from FREE to USED

[ldelete.c](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Locks/sys/ldelete.c)
This function would change the state of the lock from USED to again FREE and make it available again

[lock.c](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Locks/sys/lock.c)
Probably the most important function to assign a lock to a process as described above and in the problem statement
A step worth nothing is how the bitmask is maintained using 'lptr->lprocbitmask |= (LL_ONE << currpid);'

[releaseall.c](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Locks/sys/releaseall.c)
Another most important function to release an acquired lock 
Observe how the bitmask is reset when releasing the lock. lptr->lprocbitmask &= ~(LL_ONE << currpid);

If hope these hints can guide you to develop the understanding of the implementation.

