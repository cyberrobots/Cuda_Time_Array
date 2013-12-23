/*
 * math_func.h
 *
 *  Created on: Nov 23, 2013
 *      Author: cuda
 */

#ifndef MATH_FUNC_H_
#define MATH_FUNC_H_
inline int	paretoII_lomax(int factor,int shift, double alfa,double sigm,double mmi)
{
	/*Pareto I distribution.*/
	double r;
	double out;
	r	=((double) rand() / (RAND_MAX));
	out	=(1- ( pow ( ( ( r-mmi) / sigm ),( (-1)*alfa) ) ) );
	out	=shift+(factor*out);
	return (int)out;
}
inline int	paretoI(int factor,int shift, double alfa,double sigm)
{
	/*Pareto I distribution.*/
	double r;
	double out;
	r	=((double) rand() / (RAND_MAX));
	out	=(pow((r/sigm),((-1)*alfa)));
	out	=shift+(factor*out);
	return (int)out;
}
inline long rand_normal(double mean, double stddev,int shift){
     static double n2 = 0.0;
     static int n2_cached = 0;
     if (!n2_cached) {
         double x, y, r;
 	do {
 	    x = 2.0*rand()/RAND_MAX - 1;
	    y = 2.0*rand()/RAND_MAX - 1;

 	    r = x*x + y*y;
 	} while (r == 0.0 || r > 1.0);
         {
         double d = sqrt(-2.0*log(r)/r);
 	double n1 = x*d;
 	n2 = y*d;
         long result = n1*stddev + mean;
         n2_cached = 1;
         return (long)(result+shift);
         }
     } else {
         n2_cached = 0;
         return (long)(n2*stddev + mean+shift);
     }
 }
inline long exponential(double lamda,int factor,int shift)
{
	long result;
	double x;
	x=((double) rand() / (RAND_MAX));
	result=(shift+(factor*((-1)*(1/lamda)*log(x))));
	return result;

}
inline int poissonRandom(double expectedValue,int factor,int shift)
{
	int n;
	double x;
	n=1;
	x=1.0;
	x=(double) rand()/INT_MAX;
	while(x>=exp((-1)*expectedValue)){
		x=(x*((double) rand()/INT_MAX));
		n=n+1;
	}
	n=shift+(factor*n);
	return n;
}
inline int* bubble_sort(int *input,int size)
{
	/*Bubble sort the random values.*/
	int swap;
	int i,k;
	for(i=0;i<size-1;i++)
	{
		for(k=0;k<size-i-1;k++){
			if(input[k]>input[k+1]){
				swap=input[k];
				input[k]=input[k+1];
				input[k+1]=swap;
			}
		}
	}
	return input;
}


#endif /* MATH_FUNC_H_ */
