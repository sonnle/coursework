/*
 * physical_wire.c
 *
 * One in a trio of programs used to simulate the different layers of a network.
 * This physical wire is the most bare bones program in the message transmission.
 * Progress through the network follow:
 *
 * network(sender) -> data_link(sender) -> physical_wire -> data_link(receiver) -> network(receiver)
 *
 * The physical wire manages two connections from data link layers. When the wire
 * receives a frame from one of the data links, it reads the information and then
 * writes it to the other wire.
 *
 *  Created on: Nov 9, 2015
 *      Author: sle
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "structs.h"

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/* Global variable to keep track of the two connections to the physical wire */
int clientlist[2];

/* Receive frame from one data link layer and send it off to the other data link layer*/
void * onesocket(int threadsockfd)
{
	/* Local variables */
	int threadN;
	frame incoming_frame;

	while (1)
	{
		/* Receive a frame from one of the data link layers */
		threadN = read(threadsockfd, &incoming_frame, sizeof(frame));
		if (threadN < 0)
			error("ERROR reading from socket");

		/* Print out confirmation */
		printf("Received message from data link layer.\n");

		/* Check to see if the message "EXIT". If true, close thread socket */
		if(strcmp(incoming_frame.my_packet.message, "EXIT\n") == 0)
		{
			close(threadsockfd);
			return NULL;
		}
		/* Otherwise send it off to the other data link layer */
		else
		{
			/* Check which client is sending message */
			for(int i = 0; i < 2; i++)
			{
				if(clientlist[i] != threadsockfd)
				{
					/* Printing out sender and receiver */
					if(i == 0)
						printf("Passing from sockfd %d to %d\n", clientlist[1], clientlist[i]);
					else
						printf("Passing from sockfd %d to %d\n", clientlist[0], clientlist[i]);
					/* Sending frame to other data link layer */
					threadN = write(clientlist[i], &incoming_frame, sizeof(frame));
					if(threadN < 0)
						error("Error writing to socket");
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	/* Local variables */
	int sockfd, newsockfd, portno, n, i;

	socklen_t clilen;

	struct sockaddr_in serv_addr, cli_addr;

	pthread_t threadlist[2];

	/* Check number of arguments */
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	/* Setting up socket to listen for conncections from data link layer */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;

	serv_addr.sin_addr.s_addr = INADDR_ANY;

	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	for(i = 0; i < 2; i++) /* Acccept two requests */
	{
		listen(sockfd, 5);
		clilen = sizeof(cli_addr);

		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		/* Store in the global for access from the threads */
		clientlist[i]=newsockfd;

		/* Create new thread to manage connection */
		pthread_t pth;
		pthread_create(&pth,NULL,onesocket,clientlist[i]);
		threadlist[i] = pth;
	}

	close(sockfd);
	pthread_join(threadlist[0],NULL);
	pthread_join(threadlist[1],NULL);
	return 0;
}

