/*
 * functions.h
 *
 *  Created on: Nov 23, 2013
 *      Author: cuda
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_


input_var* input_check(int argc,char *argv[])
{
	/*This is the fully functional input function*/
	pid_t pid;
	int input,i,k,w;
	int num_of_dist;
	input_var *import;
	float percentage;
	float distr_percentage_check;
	char a[5],Distrib[MAX_DIST][15];
	/*Code*/
	strcpy(a, "%");
	strcpy(Distrib[0],"Normal");
	strcpy(Distrib[1],"Poisson");
	strcpy(Distrib[2],"Exponential");
	strcpy(Distrib[3],"ParetoI");
	strcpy(Distrib[4],"ParetoII");
	strcpy(Distrib[5],"Random");
	if ((pid= getpid())< 0){
		perror("Unable to get pid!");
		exit(1);}
	if((argc-1)==0)
	{
		fprintf(stderr,"Missing Input Please check info. (type -h for help.)\n");
		exit(1);
	}
	else if((argc-1)==1){
		if(strncmp(argv[1],"-h",2)==0)
		{
			printf(" Help: \n");
			printf(">Mandatory inputs:\n"
					" 1) Incoming Ethernet Interface Name.\n"
					" 2) Outgoing Ethernet Interface Name.\n"
					" 3) Buffer Size.\n");
			printf(">Lossy Operation:\n"
					" 4) Packets to be lost.\n"
					" 5) Ensemble for applying the Loss.\n");
			printf(">Lossy and/or Delay Distribution Operation:\n"
					" 6) Distribution Percentage Contribution (The sum <= 1.0).\n"
					" 7) Distribution Type (check documentation for supported).\n"
					" 8) Distribution Variables (check documentation).\n");
			printf("\n e.g. ./name eth0 eth0 1000 10 1024 0.5 Normal 10000 100 0 0 0\n");
			printf(" Bye!\n");
			exit(1);
		}
		else
		{
			fprintf(stderr,"Missing Input Please check info. (type -h for help.)\n");
			exit(1);
		}
	}
	else if((argc-1)==3)
	{
		printf("\nPass Thought Mode!\n");
		import=(input_var*)malloc(sizeof(input_var));
		import->DEVICE=argv[1];
		import->DEVICE_2=argv[2];
		import->BUFFER_SIZE=atoi(argv[3]);
		import->delay_intro=0;
		import->globalmix=NULL;
		if(import->BUFFER_SIZE>MAXSIZE)
		{
			fprintf(stderr,"TOO BIG BUFFER CAN NOT HANDLE! (type -h for help.)\n");
			exit(1);
		}
		percentage=0;
		import->loss=0;
		import->accurate=0;
		/*Run Statistics and to User Output*/
		printf("*****************************\n");
		printf("****System Specifications****\n");
		printf("*****************************\n");
		printf("Buffer Size     : %10i	(positions).\n",import->BUFFER_SIZE);
		printf("Buffer Memory   : %10li	(bytes).\n",sizeof(place)*import->BUFFER_SIZE);
		printf("*****************************\n");
		printf("The process id is %d.\n", pid);
		printf("*****************************\n");
	}
	else if((argc-1)==5)
	{
		printf("\nPass Thought with Loss Mode!\n");
		import=(input_var*)malloc(sizeof(input_var));
		import->DEVICE=argv[1];
		import->DEVICE_2=argv[2];
		import->BUFFER_SIZE=atoi(argv[3]);
		if(import->BUFFER_SIZE>MAXSIZE)
		{
			fprintf(stderr,"TOO BIG BUFFER CAN NOT HANDLE! (type -h for help.)\n");
			exit(1);
		}
		import->loss=atoi(argv[4]);
		import->accurate=atoi(argv[5]);
		import->delay_intro=0;
		import->globalmix=NULL;
		if(import->accurate==0){
			import->loss=0;/*To avoid mistake an hang the program*/
			percentage=0.0;
		}
		else{
			percentage=(float)(100.0*(float)import->loss/(float)import->accurate);
		}
		/*Run Statistics and to User Output*/
		printf("*****************************\n");
		printf("****System Specifications****\n");
		printf("*****************************\n");
		printf("Buffer Size     : %10i	(positions).\n",import->BUFFER_SIZE);
		printf("Buffer Memory   : %10li	(bytes).\n",sizeof(place)*import->BUFFER_SIZE);
		printf("LOSS PERCENTAGE : %10.4f	.\n",percentage,a);
		printf("*****************************\n");
		printf("The process id is %d.\n", pid);
		printf("*****************************\n");

	}
	else if((argc-1)>BASIC && (argc-1)<=((DISTR_VAR*MAX_DIST)+BASIC))
	{
		import=(input_var*)malloc(sizeof(input_var));
		if(import==NULL){fprintf(stderr,"Memory Allocate 1.\n");exit(1);}
		/*Declare that we are going to delay distribute*/
		import->delay_intro=1;
		/*Mandatory Variables*/
		import->DEVICE=argv[1];
		import->DEVICE_2=argv[2];
		import->BUFFER_SIZE=atoi(argv[3]);
		/*Check Buffer size according systems RAM availability.*/
		if(import->BUFFER_SIZE>MAXSIZE)
		{
			fprintf(stderr,"TOO BIG BUFFER CAN NOT HANDLE! (type -h for help.)\n");
			exit(1);
		}
		/*Importing The loss number and the ensemble*/
		import->loss		=atoi(argv[4]);
		import->accurate	=atoi(argv[5]);
		/*Check the loss and the ensemble numbers.*/
		if(import->loss <0 || import->loss <0){
			fprintf(stderr,"Problem with loss and ensemble (type -h for help.)\n");}
		if(import->loss==0)
		{
			printf("\nDistribution Mode!\n");
		}
		else
		{
			printf("\nDistribution with Loss Mode!\n");
		}
		/*Check for packet Loss*/
		if(import->accurate==0)
		{
			fprintf(stderr,"Problem with ensemble (type -h for help.)\n");exit(1);
			import->loss=0;/*To avoid mistake an hang the program*/
			percentage=0.0;
		}
		else{
			percentage=(float)(100.0*(float)import->loss/(float)import->accurate);
		}
		/*Importing the distributions variables*/
		input=(argc-1)-BASIC;
		num_of_dist=0;i=0;
		while(input!=0)
		{
			if(input<0){fprintf(stderr,"False Input Please check info. (type -h for help.)\n");exit(1);}
			num_of_dist++;
			input=input-DISTR_VAR;
		}
		printf("Number of Distributions :%i\n",num_of_dist);
		import->globalmix=(distr_mix*)malloc(sizeof(distr_mix)*num_of_dist);
		if(import->globalmix==NULL){fprintf(stderr,"Memory Allocate 2.\n");exit(1);}
		import->numofdistr=num_of_dist;
		/*Importing Distributions Variables*/
		for(i=0;i<num_of_dist;i++)
		{
			k=(BASIC+(i*DISTR_VAR));
			import->globalmix[i].percentage				=atof(argv[k+1]);
			/*Import Distributions Name and define the opcode.*/
			if(strncmp(argv[k+2],Distrib[0],(int)(sizeof(argv[k+2])))==0){
				import->globalmix[i].var.distr_opcode		=1;
			}
			else if(strncmp(argv[k+2],Distrib[1],(int)(sizeof(argv[k+2])))==0){
				import->globalmix[i].var.distr_opcode		=2;
			}
			else if(strncmp(argv[k+2],Distrib[2],(int)(sizeof(argv[k+2])))==0){
				import->globalmix[i].var.distr_opcode		=3;
			}
			else if(strncmp(argv[k+2],Distrib[3],(int)(sizeof(argv[k+2])))==0){
				import->globalmix[i].var.distr_opcode		=4;
			}
			else if(strncmp(argv[k+2],Distrib[4],(int)(sizeof(argv[k+2])))==0){
				import->globalmix[i].var.distr_opcode		=5;
			}
			else if(strncmp(argv[k+2],Distrib[5],(int)(sizeof(argv[k+2])))==0){
				import->globalmix[i].var.distr_opcode		=6;
			}
			else{
				fprintf(stderr,"Check Distribution Name.(type -h for help.)\n");
				exit(1);
			}
			/*Import Distributions Variables*/
			for(w=0;w<5;w++)
			{
				import->globalmix[i].var.var[w]			=atof(argv[k+w+3]);
			}
		}
		printf("*****************************\n");
		printf("****System Specifications****\n");
		printf("*****************************\n");
		printf("Buffer Size     : %10i	(positions).\n",import->BUFFER_SIZE);
		printf("Buffer Memory   : %10li	(bytes).\n",sizeof(place)*import->BUFFER_SIZE);
		printf("LOSS PERCENTAGE : %10.4f	.\n",percentage,a);
		printf("Distributions Specifications.\n");
		/*Printing the Distributions for Validation*/
		distr_percentage_check=0.0;
		for(i=0;i<num_of_dist;i++)
		{
			printf("Distribution Percentage: %3.2f %s Opcode: %2i Name:%15s .\n",100*import->globalmix[i].percentage,a,import->globalmix[i].var.distr_opcode,Distrib[import->globalmix[i].var.distr_opcode-1]);
			distr_percentage_check+=import->globalmix[i].percentage;
		}
		if(distr_percentage_check>1.0)
		{
			fprintf(stderr,"\nPlease Check Distributions Percentages. (type -h for help.)\n");
			exit(1);
		}
		printf("*****************************\n");
		printf("The process id is %d.\n", pid);
		printf("*****************************\n");
	}
	else

	{
		fprintf(stderr,"To Many Arguments: False Input Please check info. (type -h for help.)\n");
		exit(1);
	}


	return (import);
}

inline distr_var* distribution_mix(distr_mix *ptr,int dist_num,int ensamble)
{
	distr_var *array=NULL;
	distr_mix *local=NULL;
	int l,k,i,j,w,p;

	/*	Functions	*/

	// Allocate the array is going to be returned
	array=(distr_var*)malloc(sizeof(distr_var)*ensamble);

	// Allocate small arrays according the number of the distributions
	local=(distr_mix*)malloc(sizeof(distr_mix)*dist_num);

	if(local==NULL ||array==NULL){perror("No Mem Allocated!\n");}

	memcpy(local,ptr,(sizeof(distr_mix)*dist_num));

	i=0; j=0; p=0; w=0;

	k=ensamble;
	/* memcpy the distributions vars into each array places spot */
	for(j=0;j<dist_num;j++)
	{
		w=(ensamble-k);
		p=ensamble*local[j].percentage+w;
		for(i=w;i<p;i++)
		{
			/*Copy the contents.*/
			memcpy(&array[i],&local[j].var,sizeof(distr_var));
			/*calculating remaining positions*/
			k--;
		}
	}
	if(k!=0)
	{
		/*Fill the gap if no distribution applies to the rest of the packets.*/
		for(i=(ensamble-k);i<ensamble;i++)
		{
			array[i].distr_opcode=0;
			for(l=0;l<5;l++)
			{
				array[i].var[l]=0.0;
			}
		}
	}
	// Free Local memory and return
	free(local);
	return (array);
}

inline void delay_fix(struct timeval *out, distr_var *input)
{
	/************************/
	/*		Functions		*/
	/************************/
	/*
		0 Pass without processing.
		1 result = rand_normal (MEAN , SIGMA); // Gauss
		2 result = exponential (LAMDA);
		3 result = poissonRandom (EXPECTED , FACTOR);
		4 result = paretoI (FACTOR , SHIFT , ALFA , SIGM);
		5 result = paretoII (FACTOR , SHIFT , ALFA , SIGM);
		6 result = random(FACTOR , SHIFT);	// Uniform
	*/
	/************************/

	long result=0;

	switch(input->distr_opcode){
//	if(input->distr_opcode==0)
//	{
	case 0 :
		out->tv_sec=0;
		out->tv_usec=0;
		result=0;
		//return;
//	}
	break;
	case 1 :
	//else if(input->distr_opcode==1)
	//{
		result=rand_normal(input->var[0],input->var[1],(int)input->var[2]);
	//}
	break;
	//else if(input->distr_opcode==2)
	//{
	case 2 :
		result=poissonRandom((int)input->var[0],(int)input->var[1],(int)input->var[2]);
	//}
	break;
	//else if(input->distr_opcode==3)
	//{
	case 3 :
		result=exponential(input->var[0],(int)input->var[1],(int)input->var[2]);
	//}
	break;

	//else if(input->distr_opcode==4)
	//{
	case 4 :
		result=paretoI((int)input->var[0],(int)input->var[1],input->var[2],input->var[3]);
	//}
	break;
	//else if(input->distr_opcode==5)
	//{
	case 5 :

		result=paretoII_lomax((int)input->var[0],(int)input->var[1],input->var[2],input->var[3],input->var[4]);
	//}
	break;
	//else if(input->distr_opcode==6)
	//{
	case 6 :
		result=(rand()%(int)input->var[0]+input->var[1]);
	//}
	break;
	default:
	break;
	}
	// Convert Long delay to seconds and microseconds.
	out->tv_sec=0;
	out->tv_usec=0;
	while(result>1000000)
	{
		out->tv_sec++;
		result=result-1000000;
	}
	out->tv_usec=result;
	return;
}

inline void permutate(distr_var *input, int size)
{
//This is an implementation of Fisher–Yates shuffle algorithm,
//which produces unique random numbers.
//Wiki	:	http://tinyurl.com/o5uk3t
//Stack	:	http://tinyurl.com/8tagkfj

	int a;
	register int i;
	i=size-1;
	distr_var swap;

	while(i!=0)
	{
		a=rand()%i;
		memmove(&swap,			&input[a],	sizeof(distr_var));
		memmove(&input[a],		&input[i],	sizeof(distr_var));
		memmove(&input[i],		&swap,		sizeof(distr_var));
		i--;
	}

#ifdef DEBUG
	printf("Permutation Done!\n");
#endif

}

#endif /* FUNCTIONS_H_ */
