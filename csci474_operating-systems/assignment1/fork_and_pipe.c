/*
 * IndividualAssignment1_Le.c
 *
 * Program using fork() and pipe() to sum up numbers gathered from a .dat file.
 * Aside from summing up numbers, also keeps track of time to execute.
 * Outputs results to terminal window during execution.
 *
 * Compiled using standard Linux gcc command formatted:
 * 	gcc IndividualAssignment1_Le.c -o (output)
 * 	NOTE: some versions of gcc require -std=c99 option when compiling
 *
 * Ran using ./(output) (number of processes 1, 2, or 4) (file)
 *  (file) = file1.dat, file2.dat, file3.dat, file4.dat
 *  NOTE: .dat file must be in the same directory as output
 *
 * Terminal output generates two lines, Total and Time
 *  Total - Result of adding up all elements in file (sum)
 *  Time - Time in seconds taken to execute from the time child
 *  	   processes are created to end of execution (execution time)
 *  NOTE: simulation must be manually ran for each number of threads and file
 *
 *  Created on: Oct 23, 2015
 *      Author: sle
 */

#include 	<stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include <sys/time.h>

int main (int argc, char* argv[])
{
	clock_t begin, end; // clock times to calculate time elapsed

	if(argc != 3) // ensure that there is the correct amount of arguments before processing.
	{
		fprintf(stderr,"Error: need two arguments. \n1.Number of threads \n2.File name\n");
		exit(0); // print error message and exit if not satisfied
	}

	int numProcess;
	switch(atoi(argv[1])) // first argument given is the number of concurrent processes to be run
	{					  // only options are 1, 2, or 4 processes
		case 1:
		case 2:
		case 4:
			numProcess = atoi(argv[1]); // if 1, 2, or 4, numProcess
			break;
		default: // otherwise print error message and exit
			fprintf(stderr, "Error: number of processes must be 1, 2, or 4\n");
			exit(0);
	}

	int arraySize; // defining size of array depending on file name

	if(strcmp(argv[2], "file1.dat") == 0) // compare the second argument to specified file names
		arraySize = 1000;				  // assign arraySize values accordingly
	else if(strcmp(argv[2], "file2.dat") == 0)
		arraySize = 10000;
	else if(strcmp(argv[2], "file3.dat") == 0)
		arraySize = 100000;
	else if(strcmp(argv[2], "file4.dat") == 0)
		arraySize = 1000000;
	else
	{
		fprintf(stderr, "Error: file name not recognized\n");
		exit(0); // if file name is not recognized, print error message and exit
	}

	FILE *infile = fopen(argv[2], "r"); // once verified that name of file is appropriate,
										// make file pointer to the file

	int numArray[arraySize]; // create an array with size values assigned above depending on file name
	size_t nbytes = 5; // number of bytes per line, used in getline
	for(int i = 0; i < arraySize; i++) // populate the array with integers from the .dat file
	{
		char* line = (char*)malloc(5 * sizeof(char));
		int tempNum;

		getline(&line, &nbytes, infile);
		sscanf(line, "%d", &tempNum); // takes string line, and stores corresponding int into tempNum

		//printf("%d\n" , tempNum);
		numArray[i] = tempNum;
	}

	int numPerProcess = arraySize / numProcess; // used to calculate the number of integers that
												// each process will calculate
	int fds[numProcess][2]; // creating pipes to write to
	for(int i = 0; i < numProcess; i++)
	{
		pipe(fds[i]); // initializing pipes
	}

	begin = clock(); // start timing of execution

	for(int i = 0; i < numProcess; i++) // creating child processes
	{
		if(fork() == 0)
		{
			int start = numPerProcess * i;   // calculating the starting and ending calculations
			int end = numPerProcess + start; // of each thread
			int processTotal = 0;
			for(int j = start; j < end; j++)
			{
				//printf("thread: %d: number: %d arrayNum: %d\n", i, j, numArray[j]);
				processTotal += numArray[j];
			}
			write(fds[i][1], &processTotal, sizeof(int)); // write to the pipe after all numbers are
														  // added together
			exit(0);
		}
	}

	int arrayTotal = 0; // keeping track of total

	for(int i = 0; i < numProcess; i++) // reading from each child process' pipe
	{
		int temp;
		read(fds[i][0], &temp, sizeof(int));
		//printf("temp %d: %d\n", i, temp);
		arrayTotal += temp;
	}

	printf("Total: %d\n", arrayTotal); // print out total

	end = clock(); // end timing of execution

	double time = (double)(end - begin) / CLOCKS_PER_SEC; // calculate difference in times
	printf("Time: %f\n", time); // print out time

}

