/*
 * semaphore_intro.c
 *
 *  Created on: Oct 21, 2015
 *      Author: sle
 */

#include <sys/wait.h>
#include  <pthread.h>
#include   <signal.h>

#define size 100

const int n = 4;

typedef struct s_Semaphore
{

}Semaphore;

Semaphore s = 1; // Used to ensure mutual exclusion
Semaphore t = 0; // Used for event scheduling
Semaphore u = size; // Used to add data to the buffer

void Producer(int i)
{
	while(1)
	{
		wait(u);
		produce();
		wait(s);
			// Add data to the buffer
		signal(s);
		signal(t);
	}
}

void Consumer(int i)
{
	while(1);
	{
		wait(t);
		wait(s);
			// Take data from buffer
		signal(s);
		consume();
		signal(u);
	}
}

