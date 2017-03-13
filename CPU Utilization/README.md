###CPU Utilization in an Embedded System

####Motivation
So how fast is the processor used in the project. Selecting the processor is one of the major decisions for any embedded system project. 
Your selection is based on the features required to satisfy the control functionality of the final product and the raw computing power
needed to fulfill those system requirements.

Theories and guidelines suggest that 50-60% CPU utilization is ideal in the most loaded state of the system. However, different and requirements can dictate different levels of utilization.
In many cases, you will never have a precise idea of the  CPU utilization when you select the processor and your selection will be based only on experiential data offered by experienced 
members and the vendor.

But it is always in the best interest of the project to keep a track of the CPU utilization. 


###CPU utilization
In this article, I will describe how CPU utilization can be calculated for a mutitasking real time system. The description is 
inspired by my real time experience working as a SW engineer for embedded system applications. I have seen the sytem touching 
close to 100% CPU utilization and it's a clear call for changing the system processor (in the worst case) or keep optimizing the system.

I will provide small code snippets to get the idea for how this calculation can be acheived which will be mostly a psudo-code but enough to understand the concept.

####Define CPU Utilization
Simply, CPU utilization can be understood in terms of percentage of the time, the system is not performing the IDLE task or null task.
There is a null task which is the no load task which gets scheduled when no other task can be run.
The idle task is the task with the absolute lowest priority in a multitasking system. This task is also sometimes called the background task or background loop.

#####Steps:

1) Identify all the tasks running in the system and change their priority to minimum
2) Make the priority of the IDLE task maximum.
3) Get a BaseCount for CPU clocks used. Let's call it BASE_CNT
4) Raise the priority of the other tasks to their original values
5) Now get the CPU clocks count . Lets call it N_CNT
6) Then CPU utilization is calculated as (BASE_CNT - N_CNT)/BASE_CNT
