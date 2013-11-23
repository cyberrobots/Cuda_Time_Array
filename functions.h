/*
 * functions.h
 *
 *  Created on: Nov 23, 2013
 *      Author: cuda
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_


#define KRED  "\x1B[31m"
#define KNRM  "\x1B[0m"
#define KBLU  "\x1B[34m"
input_var* input_check(int argc,char *argv[])
{
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
			printf("%s",KRED);
			printf(" Help: \n");printf("%s", KNRM);
			printf("%s>Mandatory inputs:%s\n"
					" 1) Incoming Ethernet Interface Name.\n"
					" 2) Outgoing Ethernet Interface Name.\n"
					" 3) Buffer Size.\n",KRED,KNRM);
			printf("%s>Lossy Operation:\n%s"
					" 4) Packets to be lost.\n"
					" 5) Ensemble for applying the Loss.\n",KRED,KNRM);
			printf("%s>Lossy and/or Delay Distribution Operation:%s\n"
					" 6) Distribution Percentage Contribution (The sum <= 1.0).\n"
					" 7) Distribution Type (check documentation for supported).\n"
					" 8) Distribution Variables (check documentation).\n",KRED,KNRM);
			printf("\n e.g. ./name eth0 eth0 1000 10 1024 0.5 Normal 10000 100 0 0 0\n");
			printf(" %sBye!\n",KRED);printf("%s", KNRM);
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
		printf("\n%sPass Thought Mode!%s\n",KBLU,KNRM);
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
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("****System Specifications****\n");
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("Buffer Size     : %10i	(positions).\n",import->BUFFER_SIZE);
		printf("Buffer Memory   : %10li	(bytes).\n",sizeof(place)*import->BUFFER_SIZE);
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("The process id is %d.\n", pid);
		printf("%s*****************************%s\n",KRED,KNRM);
	}
	else if((argc-1)==5)
	{
		printf("\n%sPass Thought with Loss Mode!%s\n",KBLU,KNRM);
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
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("****System Specifications****\n");
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("Buffer Size     : %10i	(positions).\n",import->BUFFER_SIZE);
		printf("Buffer Memory   : %10li	(bytes).\n",sizeof(place)*import->BUFFER_SIZE);
		printf("LOSS PERCENTAGE : %10.4f	%s.\n",percentage,a);
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("The process id is %d.\n", pid);
		printf("%s*****************************%s\n",KRED,KNRM);

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
			printf("%s\nDistribution Mode!%s\n",KBLU,KNRM);
		}
		else
		{
			printf("%s\nDistribution with Loss Mode!%s\n",KBLU,KNRM);
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
		printf("Number of Distributions :%s%i%s\n",KRED,num_of_dist,KNRM);
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
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("****System Specifications****\n");
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("Buffer Size     : %10i	(positions).\n",import->BUFFER_SIZE);
		printf("Buffer Memory   : %10li	(bytes).\n",sizeof(place)*import->BUFFER_SIZE);
		printf("LOSS PERCENTAGE : %10.4f	%s.\n",percentage,a);
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
		printf("%s*****************************%s\n",KRED,KNRM);
		printf("The process id is %d.\n", pid);
		printf("%s*****************************%s\n",KRED,KNRM);
	}
	else

	{
		fprintf(stderr,"To Many Arguments: False Input Please check info. (type -h for help.)\n");
		exit(1);
	}


	return (import);
}

#endif /* FUNCTIONS_H_ */
