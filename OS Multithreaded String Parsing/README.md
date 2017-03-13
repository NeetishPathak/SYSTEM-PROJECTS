
####Motivation:
This is a C implemetation for multithreaded string parsing to understand concurrency in the system.
Complete **Problem Statement** can be seen [here](https://github.com/NeetishPathak/SYSTEM-PROJECTS/blob/master/OS%20Multithreaded%20String%20Parsing/Problem_Statement.md).

The problem is straight forward. Here the user is given a file with string in it and he is supposed to count the occurence of a 
particualr string in the file.

A simple application of this program can be found in when using (Ctrl + F) in any web page

Following hints will be useful to understand the implementation

###Solution Road:

- Find the length of the string in the file. This can be done in O(n) time.

The following sniiped is useful to understand how the whole file is read to count the number of characters

  while(EOF!=(ch=fgetc(fp))){
            str[len++]=ch;
            if(len==size){
                str = realloc(str, sizeof(char)*(size+=16));
                if(!str)return str;
            }
  }
  
  input size is initially passed as 10 but as per the need it is increased.

- Then assuming T threads are to be created, split the block size for wach of the thread using (Total_Size)/(T)
- create the thread using ''' rc = pthread_create(__Reference to thread__, NULL, __Thread function Ptr__, __Arg (Some Data Structure)__)); '''
- After initailizing all the threads, wait for threads to finish  using ''' pthread_join(threads[t], NULL); ''' before the main function finishes
- After the threads have executed, they will save the count in the data structure passed as argument
- To ensure the words that lie on the border of two thread blocks are counted, use an additional check to go and test in the subsequent block for that word completion. For this, a variable can be maintained that keeps track of matching characters


