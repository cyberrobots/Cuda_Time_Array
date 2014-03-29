/*
 * my_kernels.cuh
 *
 *  Created on: Nov 23, 2013
 *      Author: cuda
 */

#ifndef MY_KERNELS_CUH_
#define MY_KERNELS_CUH_

__global__ void super_kernel(
		curandState *state,							// 	initialization seed
		unsigned long *sec,unsigned long *usec,		//	the returning values
		const distr_var *input,						//	the input array
		int samples)								//	samples to be created
{
    int id = threadIdx.x + blockDim.x * blockIdx.x;	// thread's id
    unsigned long output;							// result of the distributions
    double other2;
    double internal,other;
    int opcode=input[id].distr_opcode;				// distribution opcode

    if(id<samples)
        {
    		/* Each thread gets same seed, a different sequence number, no offset */

    		curand_init(id, 0, 0, &state[id]);

    		sec[id]	=0;
    		usec[id]=0;

    		/* Calculation of the delay according the opcode */
    		switch(opcode){
    		//if(opcode==1)
    		//{
    		case 1 :
    			/*Normal*/
    			output = (unsigned long)((curand_normal(&state[id])*input[id].var[1])+input[id].var[0]);
    		//}
    		break;
    		//else if(opcode==2)
    		//{
    		case 2 :
    			/*Poisson*/
    			output = ( unsigned long)((curand_poisson(&state[id],input[id].var[0])*input[id].var[1])+input[id].var[2]);
    		//}
    		break;
    		//else if(opcode==3)
    		//{
    		case 3 :
    		/*Exponential*/
    			internal=curand_uniform(&state[id]);
    			output =( unsigned long)(input[id].var[2]+(input[id].var[1]*((-1/input[id].var[0])*logf(internal))));
    		//}
    		break;
    		//else if(opcode==4)
    		//{
    		case 4 :
    			/*Pareto I*/
    			internal=(curand_uniform(&state[id]));
    			other= internal/input[id].var[0];
    			other2=(-1)*input[id].var[1];
    			output=(unsigned long)( input[id].var[3] + ( input[id].var[2] * ( ( powf (other, other2) ) ) ) );
    		break;
    		//}
    		//else if(opcode==5)
    		//{
    		case 5 :
    			/*Pareto II*/
    			internal = (curand_uniform(&state[id])-input[id].var[2]);
    			other= (internal/input[id].var[0]);
    			other2=(-1)*input[id].var[1];
    			output=(unsigned long)(input[id].var[4] + ( input[id].var[3] * ( 1+ ( powf (other, other2) ) ) ) );
    		break;
    		//}
    		//else if(opcode==6)
			//{
    		case 6 :
    			/*Uniform Distribution*/
    			output=(unsigned long)(input[id].var[1]+(curand_uniform(&state[id])*input[id].var[0]));
    		break;
			//}
    		//else
			//{
    		default:
				sec[id]=0;
				usec[id]=0;
			break;
    		}
    		/* Convert the result to timevar compatible format */
    		while(output>1000000)
    		{
    			sec[id]++;
    			output=output-1000000;
    		}
    		usec[id]=output;
        }
    //__syncthreads();


}

#endif /* MY_KERNELS_CUH_ */

