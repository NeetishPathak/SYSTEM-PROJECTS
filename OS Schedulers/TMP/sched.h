#ifndef _SCHED_H_
#define _SCHED_H_

#define EXPDISTSCHED 1
#define LINUXSCHED 2
#define DEFAULT	3

extern int sched_type;

void setschedclass(int sched_class);

int getschedclass();

int isEpochFinished();

void initializeEpoch();

void setEpochQuanta();

int calculateGoodness();

#endif
