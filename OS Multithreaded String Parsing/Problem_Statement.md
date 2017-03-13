###Problem Statement
Develop a program using POSIX threads to find the number of occurrences of an input search string in a given new-line delimited source. Your program needs to operate as follows:
string_search "searchString" < inputFile

The two parameters are an input file with the source and the string to search the source for. You can assume search strings will be no longer than 80 characters. Searches should be parallelized to threads where each thread operates on a different data block (not necessarily terminated by a newline).The number of matches per block (partial result of a thread) may be stored in a global array that is aggregated in the main thread (after joining with other threads) to produce a final result number. Plot your running time while using different numbers of threads and block sizes in order to find an optimal run configuration on a Linux machine. Submit the plot as a jpg file.

Hints: use the following man pages: pthread_create, pthread_join, make, gcc.

Search strings may extend past data block boundaries, you must account for this. For example consider searching for "foo" in the following:

----------Data block handled by Thread 1 ---------------
This is some test text with the word foo\n
two times. Once at the beginning and fo


----------Data block handled by Thread 2 ---------------
o again there.\n
