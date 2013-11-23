/*
 * my_kernels.cuh
 *
 *  Created on: Nov 23, 2013
 *      Author: cuda
 */

#ifndef MY_KERNELS_CUH_
#define MY_KERNELS_CUH_

__global__ void super_kernel(
		curandState *state,
		long *sec,long *usec,
		const distr_var *input,
		int samples)
{
    int id = blockDim.x * blockIdx.x + threadIdx.x;
    double other2;
    double internal,other;
    unsigned long output;
    int opcode=0;
    opcode=input[id].distr_opcode;
    if(id<samples)
        {
    		/* Each thread gets same seed, a different sequence number,
    		 * no offset */
    		curand_init(id, 0, 0, &state[id]);
    		if(opcode==0)
    		{
    			sec[id]=0;
    			usec[id]=0;
    			return;
    		}
    		if(opcode==1)
    		{
    			output = ( unsigned long)(input[id].var[1]+(curand_normal(&state[id])*input[id].var[0]));
    		}
    		if(opcode==2)
    		{
    			output = ( unsigned long)(curand_poisson(&state[id],input[id].var[0]));
    		}
    		if(opcode==3)
    		{
    			internal=curand_uniform(&state[id]);
    			output =( unsigned long)(input[id].var[2]+input[id].var[1]*((-1/input[id].var[0])*logf(internal)));
    		}
    		if(opcode==4)
    		{
    			internal=(curand_uniform(&state[id]));
    			other= internal/input[id].var[0];
    			other2=(-1)*input[id].var[1];
    			output=( unsigned long)( input[id].var[2] + ( input[id].var[3] * ( ( powf (other, other2) ) ) ) );

    		}
    		if(opcode==5)
    		{
    			internal = (curand_uniform(&state[id])-input[id].var[2]);
    			other= internal/input[id].var[0];
    			other2=(-1)*input[id].var[1];
    			output=( unsigned long)( input[id].var[3] + ( input[id].var[4] * ( ( powf (other, other2) ) ) ) );

    		}
    		sec[id]=0;
    		usec[id]=0;
    		while(output>1000000)
    		{
    			sec[id]++;
    			output=output-1000000;
    		}
    		usec[id]=output;
        }
    else
    {
    	return;
    }


}

#endif /* MY_KERNELS_CUH_ */
