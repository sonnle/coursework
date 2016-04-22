/*
 * threads.c
 *
 *  Created on: Oct 5, 2015
 *      Author: sle
 */


#include <pthread.h>	//for thread
#include <stdio.h>

/* This is our thread function.  It is like main(), but for a thread*/
void *threadFunc(void *arg)
{
	char *str;
	int i = 0;

	str=(char*)arg;

	while(i < 15 )
	{
		usleep(1);
		printf("threadFunc says: %s\n",str);
		printf("threadFunc iteration: %d\n",i);
		++i;
	}

	return NULL;
}

int main(void)
{
	pthread_t pth;	// this is our thread identifier
	int i = 0;

	pthread_create(&pth,NULL,threadFunc,"foo");
	/*
	int pthread_create(pthread_t * pth, pthread_attr_t *att, void * (*function), void * arg);
	The first argument is a pointer to a pthread_t,
		where the function stores the identifier of the newly-created thread.
	The next argument is the attribute argument.
		This is typically NULL, but can also point to a structure that changes the thread's attributes.
	The third argument is the function the new thread will start at.
		If the thread returns from the function, the thread is terminated as well.
	The final argument is passed to the function when the thread is started.
		this is similar to the argc/argv command line arguments tomain
	Zero is returned on success, otherwise a failure of some variety happened.

	Inside the thread function, a thread can terminate itself by returning from the thread function
		or by calling pthread_exit. They behave identically.
	*/


	while(i < 10)
	{
		usleep(40); //increase sleep to 40 and see how it changes the output
		//printf("main is running...\n");
		printf("main is running iteration: %d\n", i);
		++i;
	}

	printf("main waiting for thread to terminate...\n");
	pthread_join(pth,NULL); //remove this satement, main would not wait for ThreadFunc. That means ThreadFunc may not be able to finish all iterations
							// move this statement ahead of while loop, see what happen.
	/*
	It is also possible to make one thread wait for another thread to finish.
	This is accomplished with pthread_join.
	This function takes a pthread_t identifier to pick which thread to wait for,
	and takes a void ** parameter to capture the return value.
	(in this case, we could capture the return value in the pthread_join()
	call if we used a variable instead ofNULL for the second argument.)
	*/

	return 0;
}
