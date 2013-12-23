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

#define PRINT_RESULT

int main (int argc, char *argv[])
{
	int i;
	distr_var 		*array=NULL;
	distr_var 		*d_array=NULL;
	struct timeval	*Time_results=NULL;
	struct timeval	*d_Time_results=NULL;
	unsigned long	*sec;
	unsigned long	*h_sec;
	unsigned long	*ph_sec;
	unsigned long	*usec;
	unsigned long	*h_usec;
	unsigned long	*ph_usec;
	float 			run_time;
	cudaEvent_t 	start, stop;
	curandState 	*devStates;
	cudaError_t 	err = cudaSuccess;
	input_var 		*input=input_check(argc,argv);
	int threadsPerBlock = 256;
	int blocksPerGrid;
	array=distribution_mix(input->globalmix,input->numofdistr,input->accurate);
	permutate(array, input->accurate);
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	/*Out put Times*/
	Time_results=(struct timeval *)malloc(sizeof(struct timeval)*input->accurate);
	h_sec=(unsigned long *)malloc(sizeof(unsigned long)*input->accurate);
	h_usec=(unsigned long *)malloc(sizeof(unsigned long)*input->accurate);
	/*CPU Calculation*/
	for(i=0;i<input->accurate;i++)
	{
		delay_fix(&Time_results[i],&array[i]);
#ifdef PRINT_RESULT
		printf("%4i) sec:%1li  usec:%4li\n",i,Time_results[i].tv_sec,Time_results[i].tv_usec);
#endif
	}

	/*Pinned Memory*/
	err = cudaHostAlloc((void **)&ph_sec,(size_t)(sizeof(unsigned long)*input->accurate),cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr,"GPU Pinned Mem.\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}
	err = cudaHostAlloc((void **)&ph_usec,(size_t)(sizeof(unsigned long)*input->accurate),cudaHostAllocDefault);
		if (err != cudaSuccess)
		{
			fprintf(stderr,"GPU Pinned Mem.\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}
//		/*Allocate pinned memory and initalize it.*/
//	err = cudaHostAlloc((void **)&d_array,(size_t)(sizeof(distr_var)*input->accurate),cudaHostAllocDefault);
//		if (err != cudaSuccess)
//		{
//			fprintf(stderr,"GPU MEM\n", cudaGetErrorString(err));
//			exit(EXIT_FAILURE);
//		}
//		d_array=distribution_mix(input->globalmix,input->numofdistr,input->accurate);
//		//////////////////////////////////////////
	err = cudaMalloc((void **)&devStates,(size_t)(input->accurate*sizeof(curandState)));
		if (err != cudaSuccess)
		{
			fprintf(stderr,"GPU States MEM\n", cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}
	err = cudaMalloc((void **)&d_array,(size_t)(sizeof(distr_var)*input->accurate));
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU MEM\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	err = cudaMalloc((void **)&sec,(size_t)(sizeof(unsigned long)*input->accurate));
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU MEM\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaMalloc((void **)&usec,(size_t)(sizeof(unsigned long)*input->accurate));
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU MEM\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_array, array, (size_t)(sizeof(distr_var)*input->accurate), cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU in MEMCPY\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	blocksPerGrid=(input->accurate + threadsPerBlock - 1) / threadsPerBlock;
	printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
	cudaEventRecord(start, 0);
	super_kernel<<<blocksPerGrid,threadsPerBlock>>>(devStates,ph_sec,ph_usec,d_array,input->accurate);
	err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to launch vectorAdd kernel (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&run_time, start, stop);
	printf ("Time for the kernel: %f ms\n", run_time);
	err = cudaMemcpy(h_sec,sec,(size_t)(sizeof(unsigned long)*input->accurate),cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU out MEMCPY\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	err = cudaMemcpy(h_usec,usec,(size_t)(sizeof(unsigned long)*input->accurate),cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU out MEMCPY\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}


#ifdef PRINT_RESULT
	for(i=0;i<input->accurate;i++)
	{
		//printf("%4i) sec:%4li  usec:%6li\n",i,h_sec[i],h_usec[i]);
		printf("%6li,\n",ph_usec[i]);
		fflush(stderr);
	}
#endif

	cudaFree(d_array);
	cudaFree(d_Time_results);
	free(array);
	free(Time_results);
	free(input->globalmix);
	free(input);
	return 0;
}
