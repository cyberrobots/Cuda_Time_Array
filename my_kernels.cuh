/*
 * my_kernels.cuh
 *
 *  Created on: Nov 23, 2013
 *      Author: cuda
 */

#ifndef MY_KERNELS_CUH_
#define MY_KERNELS_CUH_

__global__ void super_kernel(
		curandState *state,struct timeval *result,
		distr_var *input,
		int threads,int samples)
{
    int id = threadIdx.x + blockIdx.x * threads;
    double other2;
    register double internal,other;
    register unsigned int output;
    int opcode=0;
    opcode=input[id].distr_opcode;
    if(id<samples)
        {
    		/* Each thread gets same seed, a different sequence number,
    		 * no offset */
    		curand_init(id, 0, 0, &state[id]);
    		if(opcode==0)
    		{
    			result[id].tv_sec=0;
    			result[id].tv_usec=0;
    			return;
    		}
    		if(opcode==1)
    		{
    			output = (unsigned int)(input[id].var[1]+(curand_normal(&state[id])*input[id].var[0]));
    		}
    		if(opcode==2)
    		{
    			output = (unsigned int)(curand_poisson(&state[id],input[id].var[0]));
    		}
    		if(opcode==3)
    		{
    			internal=curand_uniform(&state[id]);
    			output =(unsigned int)(input[id].var[2]+input[id].var[1]*((-1/input[id].var[0])*logf(internal)));
    		}
    		if(opcode==4)
    		{
    			internal=(curand_uniform(&state[id]));
    			other= internal/input[id].var[0];
    			other2=(-1)*input[id].var[1];
    			output=(unsigned int)( input[id].var[2] + ( input[id].var[3] * ( ( powf (other, other2) ) ) ) );

    		}
    		if(opcode==5)
    		{
    			internal = (curand_uniform(&state[id])-input[id].var[2]);
    			other= internal/input[id].var[0];
    			other2=(-1)*input[id].var[1];
    			output=(unsigned int)( input[id].var[3] + ( input[id].var[4] * ( ( powf (other, other2) ) ) ) );

    		}
    		result[id].tv_sec=0;
    		result[id].tv_usec=0;
    		while(output>1000000)
    		{
    			result[id].tv_sec++;
    		}
    		result[id].tv_usec=output;
        }
    else
    {
    	return;
    }
}

#endif /* MY_KERNELS_CUH_ */
