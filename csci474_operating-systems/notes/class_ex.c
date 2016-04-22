/*
 * class_ex.c
 *
 *  Created on: Sep 21, 2015
 *      Author: sle
 */

#include <unistd.h>
#include <stdio.h>

int main() //when a wait operation is not needed
{

	/*
		Set global variables
		int fds[2];
		pipe(fds);

		Inside of File Descriptor Table managed by OS,
		we have pointers to pipe reading point and writing point,
			fds[0] and fds[1]
		both are defined in parent, and results in being defined in child

		Reads will blocked on an empty pipe, until there is something to read

		Within the child class
		if( fork() == 0)
		{
			//Other child code here
			After code execution,
			write(fds[1], *, *);
			exit(0);
		}
		//Other parent code here
		After code execution
		read(fds[0], *, *);
	*/
	return 0;
}

int main2() // when a wait operation is needed
{
	/*
			Set global variables
			int fds[2];
			pipe(fds);

			Inside of File Descriptor Table managed by OS,
			we have pointers to pipe reading point and writing point,
				fds[0] and fds[1]
			both are defined in parent, and results in being defined in child

			Reads will blocked on an empty pipe, until there is something to read

			Within the child class
			if( fork() == 0)
			{
				//Other child code here
				After code execution,
				read(fds[0], *, *);
				exit(0);
			}
			//Other parent code here
			After code execution
			write(fds[1], *, *);
			wait();
		*/
	return 0;
}

int main3() //multiple child
{
	/*
			Set global variables
			int fds[10][2];
			for(int i = 0; i < 10; i++)
			{
				pipe(fds[i]);
			}
			Inside of File Descriptor Table managed by OS,
			we have pointers to pipe reading point and writing point,
				fds[0] and fds[1]
			both are defined in parent, and results in being defined in child

			Reads will blocked on an empty pipe, until there is something to read

			Within the child class
			for(int i = 0; i < 10, i++)
			{
				if( fork() == 0)
				{
					//Other child code here
					After code execution,
					for(int i = 0; i < 10; i++)
					{
						read(fds[0], *, *);
					}
					exit(0);
				}
			}
			//Other parent code here
			After code execution
			write(fds[1], *, *);
			wait();
	*/
	return 0;
}

