/*
 * typedefs.h
 *
 *  Created on: Nov 23, 2013
 *      Author: cuda
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_


/*Buffers & Lists Stuff.*/
#define SIZE					1518		/*MIN 50 MAX 1500.*/
#define MAXSIZE					50000		/*Max buffer size system can handle.*/
#define INTERFACE_NUMBER_NAME	4			/*The maximum number of char the interface's name could be.*/
#define DISTR_VAR				7			/*The necessary variables for each distribution*/
#define	MAX_DIST				6			/*Maximum number of Distributions.*/
#define BASIC					5			/*The Basic variable for loss only function.*/

/* small type definition files*/

typedef struct{
	unsigned int	distr_opcode;	/*Opcode of each Distribution.*/
	double			var[5];			/*Variables for each Distribution.*/
}distr_var;

typedef struct{
	double		percentage;		/*The percentage of each Distribution*/
	distr_var 	var;			/*Variables for each Distribution.*/
}distr_mix;

typedef struct
{
	int					packsize;	/*Packet's size.*/
	int					delay;		/*Pacekt's delay-jitter.*/
	int					status;		/*Place's status 0=free,1=for process,2=waiting for transmit.*/
	struct timeval 		target;		/*Target Time for departure.*/
	struct timeval 		arrival;		/*Target Time for departure.*/
	pthread_mutex_t		mutex;
	uint8_t				bufptr[1518];	/*Packets Data goes here after proper allocation.*/
}place;


typedef struct{
	int			BUFFER_SIZE;	/*The slices main buffer is divided.*/
	int			loss;
	int 		accurate;
	int			numofdistr;
	int			delay_intro;
	distr_mix	*globalmix;
	char		*DEVICE;
	char		*DEVICE_2;
	place 		*buffer;
}input_var;


#endif /* TYPEDEFS_H_ */
