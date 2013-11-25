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
		unsigned long *sec,unsigned long *usec,
		const distr_var *input,
		int samples)
{
    int id = threadIdx.x + blockDim.x * blockIdx.x;
    unsigned long output;
    double other2;
    double internal,other;
    int opcode=input[id].distr_opcode;
    if(id<samples)
        {
    		/* Each thread gets same seed, a different sequence number,
    		 * no offset */
    		curand_init(id, 0, 0, &state[id]);
    		sec[id]	=0;
    		usec[id]=0;
    		if(opcode==1)
    		{	/*Normal*/
    			output = (unsigned long)((curand_normal(&state[id])*input[id].var[1])+input[id].var[0]);
    		}
    		else if(opcode==2)
    		{	/*Poisson*/
    			output = ( unsigned long)(curand_poisson(&state[id],input[id].var[0]));
    		}
    		else if(opcode==3)
    		{	/*Exponential*/
    			internal=curand_uniform(&state[id]);
    			output =( unsigned long)(input[id].var[2]+input[id].var[1]*((-1/input[id].var[0])*logf(internal)));
    		}
    		else if(opcode==4)
    		{	/*Pareto I*/
    			internal=(curand_uniform(&state[id]));
    			other= internal/input[id].var[0];
    			other2=(-1)*input[id].var[1];
    			output=( unsigned long)( input[id].var[2] + ( input[id].var[3] * ( ( powf (other, other2) ) ) ) );

    		}
    		else if(opcode==5)
    		{	/*Pareto II*/
    			internal = (curand_uniform(&state[id])-input[id].var[2]);
    			other= internal/input[id].var[0];
    			other2=(-1)*input[id].var[1];
    			output=( unsigned long)( input[id].var[3] + ( input[id].var[4] * ( ( powf (other, other2) ) ) ) );

    		}
    		else if(opcode==6)
			{
    			/*Uniform Distribution*/
    			output=(unsigned long)(input[id].var[1]+(curand_uniform(&state[id])*input[id].var[0]));

			}
    		else
			{
				sec[id]=0;
				usec[id]=0;
				return;
			}
    		while(output>1000000)
    		{
    			sec[id]++;
    			output=output-1000000;
    		}
    		usec[id]=output;
        }
    __syncthreads();


}

#endif /* MY_KERNELS_CUH_ */



//    		if(opcode==2)
//    		{
//    			output = ( unsigned long)(curand_poisson(&state[id],input[id].var[0]));
//    		}
//    		if(opcode==3)
//    		{
//    			internal=curand_uniform(&state[id]);
//    			output =( unsigned long)(input[id].var[2]+input[id].var[1]*((-1/input[id].var[0])*logf(internal)));
//    		}
//    		if(opcode==4)
//    		{
//    			internal=(curand_uniform(&state[id]));
//    			other= internal/input[id].var[0];
//    			other2=(-1)*input[id].var[1];
//    			output=( unsigned long)( input[id].var[2] + ( input[id].var[3] * ( ( powf (other, other2) ) ) ) );
//
//    		}
//    		if(opcode==5)
//    		{
//    			internal = (curand_uniform(&state[id])-input[id].var[2]);
//    			other= internal/input[id].var[0];
//    			other2=(-1)*input[id].var[1];
//    			output=( unsigned long)( input[id].var[3] + ( input[id].var[4] * ( ( powf (other, other2) ) ) ) );
//
//    		}
//

