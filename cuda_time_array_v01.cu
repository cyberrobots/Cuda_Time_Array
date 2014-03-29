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

#define PRINT_RESULT_CPU_NO 	// Print the calculated results

#define PRINT_RESULT_GPU_NO		// Print the calculated results

int main (int argc, char *argv[])
{
	int i;
	int threadsPerBlock 			= 	256;	// Threads per Block
	int blocksPerGrid;							// Blocks per Grid
	distr_var 		*array			=	NULL;
	distr_var 		*d_array		=	NULL;
	struct timeval	*Time_results	=	NULL;
	struct timeval	*d_Time_results	=	NULL;
	struct timeval t0;							// measuring the cpu time
	struct timeval t1;							// measuring the cpu time
	long elapsed;
	unsigned long	*sec;
	unsigned long	*h_sec;
	unsigned long	*ph_sec;
	unsigned long	*usec;
	unsigned long	*h_usec;
	unsigned long	*ph_usec;
	float 			run_time;
	cudaEvent_t 	start, stop;
	curandState 	*devStates;
	cudaError_t 	err				= 	cudaSuccess;
	input_var 		*input			=	input_check(argc,argv); // grab the input from user

	/* Functions */

	// mix the array and fix the distributions according user request
	array=distribution_mix(input->globalmix,input->numofdistr,input->accurate);

	// permutate the array.
	permutate(array, input->accurate);

	// create events for time measuring
	cudaEventCreate(&start);


	cudaEventCreate(&stop);

	/* Out put Times */
	/* Allocate Space for the results */


	// Time structs
	Time_results=(struct timeval *)malloc(sizeof(struct timeval)*input->accurate);
	// Array for the seconds
	h_sec=(unsigned long *)malloc(sizeof(unsigned long)*input->accurate);
	// Array for the uSeconds
	h_usec=(unsigned long *)malloc(sizeof(unsigned long)*input->accurate);

	printf("Size of the calculated ensemble : %i \n",input->accurate);

	/*CPU Calculation*/
	gettimeofday(&t0,NULL);
	for(i=0;i<input->accurate;i++)
	{
		delay_fix(&Time_results[i],&array[i]);
	}
	gettimeofday(&t1,NULL);

	elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;

	printf("The CPU processing elapsed : %f ms\n",(double)elapsed/1000);

#ifdef PRINT_RESULT_CPU
	for(i=0;i<input->accurate;i++){
		// Print the time for each
		printf("%4i) sec:%1li  usec:%4li\n",i,Time_results[i].tv_sec,Time_results[i].tv_usec);
	}
#endif

	/*Pinned Memory*/
	// Allocate
	err = cudaHostAlloc((void **)&ph_sec,(size_t)(sizeof(unsigned long)*input->accurate),cudaHostAllocDefault);

	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU Pinned Mem.\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	// Allocate
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

	// Allocate the seed.
	err = cudaMalloc((void **)&devStates,(size_t)(input->accurate*sizeof(curandState)));

	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU States MEM\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	// Allocate data array
	err = cudaMalloc((void **)&d_array,(size_t)(sizeof(distr_var)*input->accurate));

	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU MEM\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	// Allocate the seconds array
	err = cudaMalloc((void **)&sec,(size_t)(sizeof(unsigned long)*input->accurate));
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU MEM\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	// Allocate the useconds array
	err = cudaMalloc((void **)&usec,(size_t)(sizeof(unsigned long)*input->accurate));
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU MEM\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	// Memory copy the data array
	err = cudaMemcpy(d_array, array, (size_t)(sizeof(distr_var)*input->accurate), cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU in MEMCPY\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	// Initialize the kernel and calculate the blocks and threads per block
	blocksPerGrid=(input->accurate + threadsPerBlock - 1) / threadsPerBlock;

	printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);

	cudaEventRecord(start, 0); // start the timer.

	super_kernel<<<blocksPerGrid,threadsPerBlock>>>(devStates,ph_sec,ph_usec,d_array,input->accurate);

	err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to launch vectorAdd kernel (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	cudaEventRecord(stop, 0); // stop the timer

	cudaEventSynchronize(stop); // sync the event in order to stop.

	cudaEventElapsedTime(&run_time, start, stop); // calculate the time

	printf ("Time for the kernel: %f ms\n", run_time);

	// Retrieve data from the  gpu.
	// Seconds section
	err = cudaMemcpy(h_sec,sec,(size_t)(sizeof(unsigned long)*input->accurate),cudaMemcpyDeviceToHost);

	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU out MEMCPY\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	// USeconds section
	err = cudaMemcpy(h_usec,usec,(size_t)(sizeof(unsigned long)*input->accurate),cudaMemcpyDeviceToHost);

	if (err != cudaSuccess)
	{
		fprintf(stderr,"GPU out MEMCPY\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	/* Print the Results for debug */
#ifdef PRINT_RESULT_GPU

	for(i=0;i<input->accurate;i++)
	{
		//printf("%4i) sec:%4li  usec:%6li\n",i,h_sec[i],h_usec[i]);
		printf("%6li,\n",ph_usec[i]);
		fflush(stderr);
	}

#endif


	/* Free Memory Section */
	//	GPU
	cudaFree(d_array);
	cudaFree(d_Time_results);
	//	CPU
	free(array);
	free(Time_results);
	free(input->globalmix);
	free(input);

	return 0;
}
