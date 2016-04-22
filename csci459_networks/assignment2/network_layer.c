/*
 * network_layer.c
 *
 * One in a trio of programs used to simulate the different layers of a network.
 * This network layer is the main interface for users, allowing users to type in
 * messages to be sent. Progress through the network follow:
 *
 * network(sender) -> data_link(sender) -> physical_wire -> data_link(receiver) -> network(receiver)
 *
 * User messages are packaged up into a structure called packet. A packet has
 * two parameters, the nickname of the sender and the message to be sent.
 *
 *  Created on: Nov 9, 2015
 *      Author: sle
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

#include "structs.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void * rcvmsg (int threadsockfd)
{
	/* Local variables */
	int threadN;
	packet in_packet;

	while (1)
	{
		/*
		 * Reads a packet from the data_link_layer and
		 * prints the sender and message to the screen
		 */
		threadN = read(threadsockfd, &in_packet, sizeof(packet));
		if(threadN < 0)
			error("ERROR reading from data link layer");

		printf("\nMessage from %s: %s", in_packet.nickname, in_packet.message);
	}
	return NULL;
}


int main(int argc, char *argv[])
{
	/* Local variables */
	int datasockfd, dataportno, n;
	struct sockaddr_in dataserv_addr;
	struct hostent *dataserver;
	packet out_packet;

	char buffer[256];


	/* Check number of arguments */
	if (argc < 4)
	{
		fprintf(stderr,"usage %s data_add data_port nickname\n", argv[0]);
		exit(0);
	}

	/* Creates a socket and connects to the data_link_layer */
	dataportno = atoi(argv[2]);
	datasockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (datasockfd < 0)
		error("ERROR opening socket");

	dataserver = gethostbyname(argv[1]);
	if (dataserver == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &dataserv_addr, sizeof(dataserv_addr));
	dataserv_addr.sin_family = AF_INET;
	bcopy((char *)dataserver->h_addr_list[0], (char *)&dataserv_addr.sin_addr.s_addr, dataserver->h_length);
	dataserv_addr.sin_port = htons(dataportno);

	if (connect(datasockfd,(struct sockaddr *) &dataserv_addr,sizeof(dataserv_addr)) < 0)
		error("ERROR connecting to data link layer");


	/* Thread that receives packets from the data_link_layer */
	pthread_t pth;
	pthread_create(&pth, NULL, rcvmsg, datasockfd);

	printf("Please enter the messages now.\n");

	/* Receive messages from the keyboard and send them to the data_link_layer */
	while (1)
	{
		/*
		 * Receive message from keyboard, package into
		 * a packet and send send to data_link_layer
		 */
		bzero(buffer,256);
		fgets(buffer,255,stdin);

		strcpy(out_packet.message, buffer);
		strcpy(out_packet.nickname, argv[3]);

		n = write(datasockfd, &out_packet, sizeof(packet));

		/* Check to see if the message "EXIT". If true, close socket */
		if (strcmp (buffer, "EXIT\n")==0)
		{
			pthread_cancel(pth);
			close(datasockfd);
			return 0;
		}
	}
}

