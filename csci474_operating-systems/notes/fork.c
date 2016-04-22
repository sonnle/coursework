/*
 * fork.c
 *
 *  Created on: Sep 18, 2015
 *      Author: sle
 */

#include <sys/wait.h>

int main(void)
{
	//TODO: BLAH BLAH BLAH
	if(fork() == 0)
	{
		//TODO: BLAH BLAH BLAH
		//This will be the code associated with child 1
		if(fork() == 0)
		{
			//This would be a child of child 1, child 1A
		}
		exit(0);
	}
	if(fork() == 0)
	{
		//TODO: BLAH BLAH
		//This will be the code associated with child 2
		exit(0);
	}

	//This would be the implementation for multiple children
	//Change i to the appropriate amount of children
	for(int i = 0; i < 10; i++)
	{
		if(fork() == 0)
		{

			exit(0);
		}
	}
	//TODO: BLAH BLAH BLAH
	//This will be the code associated with parent
	wait();
}

int main2(void)
{
	int fds[3][2];
	int data[3][3];

	int grandTotal = 0, buffer;

	for(int i = 0; i < 3; i++)
	{
		pipe(fds[i]);
	}

	for(int i = 0; i < 3; i++)
	{
		if(fork() == 0)
		{
			int curTotal = 0;

			for(int j = 0; j < 3; j++)
			{
				curTotal += data[i][j];
			}
			write(fds[i][1], &curTotal, sizeof(int));
			exit(0);
		}
	}

	for(int k = 0; k < 3; k++)
	{
		read(fds[k][0], &buffer, sizeof(int));
		grandTotal += buffer;
	}
}

