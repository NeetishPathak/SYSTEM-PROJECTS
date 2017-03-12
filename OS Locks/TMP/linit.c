/*linit.c , initialize all the locks*/

#include <lock.h>
#include <q.h>

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
