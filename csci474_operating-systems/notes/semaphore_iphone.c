/*
 * sempahore_iphone.c
 *
 *  Created on: Oct 23, 2015
 *      Author: sle
 */


#include <sys/wait.h>
#include  <pthread.h>
#include   <signal.h>

#define size 100

typedef struct s_Semaphore
{

}Semaphore;

typedef enum {false, true} bool;

Semaphore s = 1; // Used to ensure mutual exclusion
Semaphore t = 0; // Used for event scheduling

int bankAccount = 0;
bool isBought = false;

void Work(int hours, int hourlyPay)
{
	while(1)
	{
		work();
		wait(s);
		deposit();
		if(bankAccount >= 649 && !isBought)
		{
			signal(t);
			isBought = true;
		}
		signal(s);
	}
}

void BuyiPhone()
{
	while(1)
	{
		wait(t);
		wait(s);
		withdraw();
		signal(s);
		buy();
	}
}

