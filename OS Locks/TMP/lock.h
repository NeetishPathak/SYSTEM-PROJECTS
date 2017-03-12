#ifndef _LOCK_H_
#define _LOCK_H_

#define L_COUNT_CHANGE	1

#define L_TAKEN 0
#define L_AVLBL	1
#define READ 2
#define WRITE 3
#define DELETE 4
#define LUSED	5
#define LFREE	6
#define NLOCKS	50

#define	isbadlock(s)	(s<0 || s>=NLOCKS)

#define LL_ONE 1LL
#define LL_ZERO 0LL

struct lentry{

	char lstate;	/*state of the lock*/
	int lpriol;	/*priority of the lock*/
	int lcnt;	/*count of the lock*/
	int lqhead;	/*head of the lock*/
	int lqtail;	/*tail of the lock*/
	int ltype;	/*type of the lock*/
	long long lprocbitmask; /* bitmask to store all the processes holding this lock*/
	
	int counter;	/*to keep track of the number of times this lock ebtry is initiated*/
};


extern struct lentry lockList[];
extern int nextlock;
extern unsigned long ctr1000;
 
void linit();
int lcreate();
int ldelete(int lockdescriptor);
int lock(int ldes1, int type, int priority);
int releaseall (int numlocks, int args, ...);

int getMaxHeldProcPrio(struct lentry *lptr);
void rampUpPrio(struct lentry *lptr, int newPrio);
void doPriorityInversion(struct lentry *lptr, struct pentry *pptr);


#endif
