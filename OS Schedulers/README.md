###Schedulers

####Objective: 
To design two new types of schedulers for XINU OS kernel. The details of the problem can be seen [here](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Schedulers/Problem_Statement.md)

####Motivation

The original scheduler in the XINU OS suffers from the problem of starvation. In such a scenario, the low priority processes never
get a chance to get scheduled with high priority process being in runnable state.

####Solution Road
To solve ths problem two approaches are taken

1. Exponentially generate a random number with a certain average inter-arrival time. This value can now be used to select the next process to be scheduled.

2. It is a linux-like epoch based scheduler which gives priority to the processes who save their time quantum when scheduled.

It is important to go through the int resched() fucnction in the [resched.c](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Schedulers/sys/resched.c)
to get an understnading of the the original and the two new scheduling algorithms

####Technology Used

The schedulers have been deployed for XINU OS kernel, which is a lightweight OS originally developed by Dr. Douglas Comer for educational use at Purdue University in the 1980s.More details details about it can be found [here](http://www.xinu.cs.purdue.edu/).
The language of implementation is C

In order to run the program,  use make command using the [Makefile](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Schedulers/compile/Makefile) in compile directory

####After Thoughts
There are certain pros and cons associated with each of the schedulers which are mentioned below


#####1. Policy name: Exponential Distribution scheduler:

Advantages: 
This policy is based on random selection of the process to be run. That is helpful to ensure a level of fairness (but not completely fair) for process scheduling.
This would always ensure progress since the process selection is random. So on an average the starvation will not happen.

Disadvantages:  
Since the process selection is based on exponential random number generation. It is highly dependent on value of lambda that we choose and how exponential distribution happens. It therefore affects how the value gets generated. Since the exponential random values generated are more concentrated towards the lower values closer to 0. We can’t say the scheduler is completely fair
So, the processes with lower priority values have higher probability of getting scheduled than the ones in higher priority.
 This policy involves computation for exponential number generation which is an overhead
 
#####2. Policy name: Linux Like scheduler:

Advantages:
Linux like scheduler is unbiased in execution and ensures that each process is assigned a time quantum for which it can run. It is also fair to the processes that could not use their time quantum since it allocates some percentage of aggregated time quantum to them in subsequent epochs.
It ensures that no process starves for execution if it is runnable.

Disadvantages:
Though the scheduler is fair to the processes in the current epoch, it does not acknowledge the value or we can say, the very existence of new processes being created in the current epoch. It is not a good approach if we assume high priority processes can get created during the epoch that need to be addressed immediately.
This policy involves two additional computation steps (for epoch initialization and goodness factor setting) which is an overhead.



#####3. Policy name:  Original Round robin scheduling policy:

Advantages:
It ensures fairness in scheduling with equal time time quantum for all the processes (provided priority of runnable processes are similar). So average waiting time and response time for each process decreases.
Also the implementation is quite straight-forward with least computation for process selection. 
Disadvantages: 
The process still cannot ensure no starvation in the scenario where a high priority process gets scheduled repeatedly since the process selection is based on priority values and there is no epoch kind of approach.
