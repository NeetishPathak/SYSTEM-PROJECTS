#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
//#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define MIN_THREAD_CNT	1
#define MAX_THREAD_CNT	100

struct timespec start, finish;
long long elapsed;

int NUM_THREADS = 5;
struct threadData *threadDataArray;
struct threadData{
	int tid;
	int count;
	int blockSize;
	char *pattern;
	int patternSize;
	char *blkPointer;
};

struct partialEnds{
	
	char* stPart;
	int stPartLen;
	int numOcurInStPart;
	
	char* endPart;
	int endPartLen;
	int numOcurInEndPart;

	char* fullWord;
	int fullWordLen;
	int numOcurInFullWord;
	
	int isMerged;	
};

/*Help for this function is taken from******************************************************
http://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
Using realloc 
*******************************************************************************************/
char *inputString(FILE* fp, size_t size){
	
	//The size is extended by the input with the value of the provisional
	char *str;
	int ch;
	size_t len = 0;
    
	str = realloc(NULL, sizeof(char)*size);//size is start size
	if(!str)return str;
    
	while(EOF!=(ch=fgetc(fp))){
	        str[len++]=ch;
	        if(len==size){
	            str = realloc(str, sizeof(char)*(size+=16));
	            if(!str)return str;
	        }
	}
	
	str[len++]='\0';


	return realloc(str, sizeof(char)*len);
}

/*Find the number of occurences of a pattern in a string*/
int findPatternCount(char* pattern, int patSize,  char* sStr, int sStrSize, int tid ){

	int i =0; int k =0;
	int counter = 0;
	
	while(i < sStrSize){
		//k=0;
		
		if(sStr[i] == pattern[k]){
			int x = i;
			
			while(k < patSize && x < sStrSize){
				//if((char)sStr[x] == '\n'){x++; continue;}				
				
				if(sStr[x] == pattern[k]){			
					x++;
					k++;
				}else{
					k = 0;
					break;
				}
			}
			
			if(k == patSize){	//pattern matched
				k=0;
				counter++;	
			}
			
			if(x >= sStrSize){break;}
						
		}
		i++;
	}

	//printf("Value of k is %d\n",k);
	if(k > 0){
		//printf("Thread %d has a side edge k = %d\n",tid,k);	
		int st = 0;
		char* nextBlockStart = sStr + sStrSize;
		while(k < patSize){
			//printf("I have a matching in the end\n");
			
			while(nextBlockStart[st] == pattern[k]){
				st++;
				k++;
			}

			if(k == patSize){
				//printf("Old value for thread %d is %d\n",tid, counter);
				//char * temp = sStr;
				//temp[sStrSize+patSize] = '\0';
				//printf("Thread 4 is %c, %c, %c, %c\n",temp[sStrSize-2],temp[sStrSize-1],temp[sStrSize],temp[sStrSize+1]);
				//printf("I was increased at thread %d\n", tid);
				counter++;
			}else{
				break;
			}
		}
	}
	
	//printf("Thread %d found %d\n",tid,counter);
	return counter;
}

/*Thread function*/
void *countWord(void *threadArg){
	
	struct threadData *myThread = (struct threadData*) threadArg;
	myThread->count = findPatternCount(myThread->pattern, myThread->patternSize,	\
				myThread->blkPointer, myThread->blockSize, myThread->tid);		
}


/*Main function*/
int main(int argc, char*argv[]){

	int nT;
	char *iString;
	iString = inputString(stdin, 10);
	
	for(nT=MIN_THREAD_CNT; nT <= MAX_THREAD_CNT; ++nT){	
	
	NUM_THREADS = nT;	
	pthread_t threads[NUM_THREADS];
	
	NUM_THREADS = (NUM_THREADS <= strlen(iString)?NUM_THREADS:strlen(iString));	
	
	threadDataArray = (struct threadData*)malloc(NUM_THREADS*sizeof(struct threadData));
	
	long t =0;	
	int rc;
	int remSize = 0;
	int iStringLen = strlen(iString);
	int blockSize = (iStringLen%NUM_THREADS)!=0?(iStringLen/NUM_THREADS)+1:(iStringLen/NUM_THREADS);
	int lastBlock = ((remSize = iStringLen%blockSize) == 0)?blockSize:remSize;
	int totalCount = 0;
	
	if(argc < 2){
		fprintf(stderr,"Please enter the search string\n");
		exit(-1);
	}
	//printf("Strlen is %d\n",(int)strlen(iString));
	//printf("Block size is %d\n", blockSize);
	//printf("Size of lastBlock is %d\n", lastBlock);

	clock_gettime(CLOCK_MONOTONIC, &start); /* mark start time */
	for(t=0; t<NUM_THREADS; ++t){
 
		//printf("In main: creating thread %ld\n", t);
		threadDataArray[t].tid = t;
		threadDataArray[t].count = 0;
	
		if(t < strlen(iString)){
			threadDataArray[t].blockSize = (t == NUM_THREADS - 1)?lastBlock:blockSize; 
			threadDataArray[t].blkPointer = iString + t*(blockSize);

			threadDataArray[t].patternSize = strlen(argv[1]);
			threadDataArray[t].pattern = argv[1]; 
			
			//printf("%ld blockStarts at %c and ends at %c\n", t, *(threadDataArray[t].blkPointer), \
				*((threadDataArray[t].blkPointer) + threadDataArray[t].blockSize - 1));
        		rc = pthread_create(&threads[t], NULL, countWord, (void*)(&threadDataArray[t]));

			
		        if(rc){
		                printf("Error, return code from pthread_create is %d", rc);
		                exit(-1);
		        }
		}
        }
	
	
	
	for(t=0; t<NUM_THREADS; ++t){
		(void) pthread_join(threads[t], NULL);
	}
	
	clock_gettime(CLOCK_MONOTONIC, &finish); /* mark the end time */ 
	elapsed = 1000000 * (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/1000;
	
	for(t=0; t<NUM_THREADS; ++t){
		totalCount += 	threadDataArray[t].count;
	}

		
	printf("blockSize: %d	 numOfThread: %d	matchCount: %d	 runningTime: %lld us\n",blockSize,NUM_THREADS,totalCount,elapsed);
	
		free(threadDataArray);	
	}

	free(iString);

        /*Last thing that main should do is */
        pthread_exit(NULL);
		
	return 0;
}
