/*
 * csci474_preproject.c
 *
 *  Created on: Aug 28, 2015
 *      Author: sle
 */

#include <stdio.h>

#define MAXNUM 3

FILE *infile;

char charNum[MAXNUM];

int main(int argc, char* argv[])
{
	/*
	 * Do for each file specified
	 */
	for(int i = 1; i < argc; i++)
	{

		/*
		 * Open up file given as command-line argument for reading
		 */
		infile = fopen(argv[i], "r");
		int numTotal = 0;

		/*
		 * Make sure that file is valid
		 */
		if(infile == NULL)
		{
			printf("ERROR: File is empty.\n");
		}
		else
		{
			/*
			 * Used to grab first char and make sure it is not EOF
			 */
			int  ch 	= getc(infile);
			int  count 	= 0;

			/*
			 * Cycles through all chars in file
			 */
			while(ch != EOF)
			{
				/*
				 * If the char is not a newline or return char,
				 * we then add it to our current char array,
				 * and increment the index of our current char array
				 */
				if(ch != '\n' && ch != '\r')
				{
					charNum[count] = (char)ch;
					count++;
				}
				/*
				 * Used to handle the "\r\n" character sequence,
				 * we ignore the return char so that we do not get duplicates,
				 * when the sequence appears, we convert the current array into
				 * an integer and add that onto our total,
				 * we then reset the count to write over our current array with new number
				 */
				else if(ch != '\r');
				else
				{
					int curInt;
					sscanf(charNum, "%d", &curInt);
					numTotal += curInt;
					count = 0;
				}
				/*
				 * Retrieves next char in file
				 */
				ch = getc(infile);
			}
			/*
			 * Print out total accumulated throughout run
			 */
			printf("%s total: %d\n", argv[i], numTotal);
		}
	}
}

