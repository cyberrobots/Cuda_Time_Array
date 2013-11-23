/*
 * cuda_time_array_v01.cu
 *
 *  Created on: Nov 23, 2013
 *      Author: cuda
 */



#define DEBUG

#include "libraries.h"
#include "typedefs.h"
#include "GPU_libraries.cuh"
#include "math_func.h"
#include "functions.h"
#include "my_kernels.cuh"


int main (int argc, char *argv[])
{
	int i;
	distr_var 		*array=NULL;
	struct timeval	*Time_results=NULL;
	input_var 		*input=input_check(argc,argv);
	array=distribution_mix(input->globalmix,input->numofdistr,input->accurate);
	permutate(array, input->accurate);
	/*Times*/
	Time_results=(struct timeval *)malloc(sizeof(struct timeval)*input->accurate);

	for(i=0;i<input->accurate;i++)
	{
		delay_fix(&Time_results[i],&array[i]);
		printf("%4i) sec:%1li  usec:%4li\n",i,Time_results[i].tv_sec,Time_results[i].tv_usec);
		fflush(stderr);
	}




	free(array);
	free(Time_results);
	free(input->globalmix);
	free(input);
	return 0;
}
