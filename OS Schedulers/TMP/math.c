#include <math.h>
#define TAYLOR_SERIES_MAX	20
#define RAND_MAX	32767

/******************************************
expdev(double): generate exponential random
		variable
*****************************************/
double expdev(double lambda) {
    double dummy;
    do
        dummy= (double) rand() / RAND_MAX;
    while (dummy == 0.0);
    return -log(dummy) / lambda;
}

double log2(double x){
	/*Use Taylor series*/
	/*find y*/
	int i; 
	double logVal = 0;

	double y = (double)(x - 1)/(x + 1);
	
	for(i=1; i< TAYLOR_SERIES_MAX; i = i+2){
		logVal += pow(y,i)/i;		
	}
	
	return 2*logVal;
}

/**********************************
pow(double y, int p)
**********************************/
double pow(double x, int y){
	int i;
	double res = 1.0;
	for(i=1; i<= y; ++i){
		res *= x;
	}
	return res;
}

/*************************************
log(double x) Function to find log		
*************************************/
double log(double x){
	/*Use Taylor series*/
	/*find y*/

	int i; 
	double logVal = 0;

	double y = 1.0 - x;
	
	for(i=1; i< TAYLOR_SERIES_MAX; ++i){
		logVal += pow(y,i)/i;		
	}
	
	return ((-1)*logVal);
}

/***************************************************
*************** test function *********************
int main(){
	double y = 55064.222633;
	printf("pow of %lf = %lf\n",y, pow(y,4));		
	printf("log val of %lf = %lf\n",y, log(y));
	printf("expdev for value: %lf\n",expdev(0.1));
	return 0;
}
*****************************************************/
