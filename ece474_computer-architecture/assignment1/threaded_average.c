/*
 * Le_Son_ECE474_Assignment1
 *
 * A C program that uses threading to add up integers pseudo-randomly generated
 * in a 5000 element array.
 *
 * The program is compiled using the basic gcc command:
 * 	gcc multithread_random.c -lpthread -std=c99 -o (output file name)
 * 		-lpthread: used to compile threaded programs
 * 		-std=c99:  used to enable c99 features such as for loops
 * 		-o:		   used to specify output file name (if not given a.out is created)
 *
 * The arrays integers are generated at runtime using the rand()
 * function given in <stdlib.h>. Values range from 0 to 1,000 via mod.
 * When the array is pseudo-randomly generated, each integer is added to
 * arrayTotal for comparison with threaded results.
 * After execution, the program will print out the total and average
 * of both the total gathered when generating the array, as well
 * as when threaded.
 *
 * At execution time, user will have to give an integer as an argument.
 * This integer will specify how many threads are to be created.
 * Running the program in terminal will follow this format:
 * 	./(output file name) (number of threads)
 *
 *  Created on: Oct 12, 2015
 *      Author: sle
 */

#include   <stdio.h> // used for stdio (printf, etc.)
#include  <stdlib.h> // used for randomizing (rand)
#include <pthread.h> // used for threading (pthread_create, pthread_join, etc.)

#define NUMBER_COUNT 5000 // defining number of elements in the array

int randomNumbers[NUMBER_COUNT]; // global array used to store the randomly generated numbers(RGN)
int arrayTotal  = 0; 			 // global used to store the sum of the RGN when generating array
int threadTotal = 0;			 // global used to store the sum of the RGN when using threads

/*
 * Structure threadArgs:
 * used to define the arguments to be passed into each thread
 * threadNum - thread identifier
 * numPerThread - number of integers that need to be processed in each thread
 */
typedef struct s_threadArgs {
    int threadNum;
    int numPerThread;
} threadArgs;

/*
 * Thread addNums:
 * Used to thread the task of adding up all of the numbers in a 5000 integer
 * array. Takes threadArgs structure as an argument, uses threadNum and numPerThread
 * to calculate which elements of the array to process.
 */
void *addNums(void* arg)
{
	threadArgs* curThread = (threadArgs *) arg;

	int startIndex = curThread->threadNum * curThread->numPerThread;
	int endIndex = (curThread->threadNum + 1) * curThread->numPerThread;

	for(int i = startIndex; i < endIndex; i++)
	{
		threadTotal += randomNumbers[i];
	}
	return NULL;
}

/*
 * Main:
 * Used to verify that when running, only one extra argument is provided.
 * This extra argument is used to determine the number of threads created.
 * Then pseudo-randomly generates the integers in the 5000 element array,
 * and keeps running total of sum of the elements in arrayTotal.
 *
 * Creates the threads passing in the structure as an argument.
 *
 * Then the main loop is used to calculate the extra elements not processed
 * in the thread, for example:
 * 	5000 elements & 13 threads = 384 elements processed in each thread,
 * 	but 384 * 13 = 4992,
 * 	so elements 4993-5000 will be processed in main.
 *
 * After extra elements are processed, threads are joined back together
 * before printing out the final totals and averages of arrayTotal and
 * threadTotal.
 */
int main(int argc, char* argv[])
{
	if(argc != 2) // argument check
	{
		fprintf(stderr, "Error: need number of threads.\n");
		exit(0);
	}

	for(int i = 0; i < NUMBER_COUNT; i++) // array generation
	{
		randomNumbers[i] = (rand() % 1000);
		arrayTotal += randomNumbers[i];
	}

	int numThreads = atoi(argv[1]); // thread number

	pthread_t threadId[numThreads];
	threadArgs threads[numThreads];

	int numPerThread = 5000 / numThreads;

	for(int i = 0; i < numThreads; i++) // thread creation
	{
		threads[i].threadNum = i;
		threads[i].numPerThread = numPerThread;

		pthread_create(&threadId[i], NULL, addNums, &threads[i]);
	}

	for(int i = (numPerThread * numThreads); i < 5000; i++) // calculating leftover
	{
		threadTotal += randomNumbers[i];
	}

	for(int i = 0; i < numThreads; i++) // thread join
	{
		pthread_join(threadId[i], NULL);
	}

	// printing results
	printf("Array total: %d\n", arrayTotal);
	printf("Array total avg: %f\n", (double)arrayTotal / 5000);

	printf("Thread total: %d\n", threadTotal);
	printf("Thread total avg: %f\n", (double)threadTotal / 5000);
}

