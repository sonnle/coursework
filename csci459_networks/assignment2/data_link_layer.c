/*
 * data_link_layer.c
 *
 * One in a trio of programs used to simulate the different layers of a network.
 * This data link layer is the intermediate step in the program for messages to be sent.
 * Progress through the network follow:
 *
 * network(sender) -> data_link(sender) -> physical_wire -> data_link(receiver) -> network(receiver)
 *
 * Packets sent from the network layer is read in the data link layer. The packets are then
 * packaged up into another intermediate structure called a frame to be sent to the physical
 * wire. The frame structure consists of a packet, as well as message number, and type.
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
#include <netdb.h> 
#include <pthread.h>

#include "structs.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/* Global variables to identify where the network and physical wire sockets are */
int network_layersockfd;
int wiresockfd;

/* Receives frames from the wiresocket and extracts the packet and sends that to the networksocket */
void * rcvfromwiresend2network_layer ( char *argv[] )
{
	/* Local variables */
	int threadN;
	frame in_frame;

	/*
	 * Receives the frame from the physical wire, prints out the frame number and type,
	 * then writes the packet to the network layer.
	 */
	while (1)
	{
		/* Receive frame from wire */
		threadN = read(wiresockfd, &in_frame, sizeof(frame));
		if(threadN < 0)
			error("ERROR reading from wire socket");

		/* Print out information */
		printf("Frame received from wire, frame number: %d, frame type: %d\n", in_frame.seq_num, in_frame.type);

		/* Send the packet to the network layer*/
		threadN = write(network_layersockfd, &in_frame.my_packet, sizeof(packet));
		if(threadN < 0)
			error("ERROR writing to network socket");
	}

	return NULL;
}


int main(int argc, char *argv[])
{
	/* Local variables to connect to the physical wire */
	int wireportno, n;
	struct sockaddr_in wireserv_addr;
	struct hostent *wireserver;

	char buffer[256];

	/* Check number of arguments */
	if (argc < 4) {
		fprintf(stderr,"Usage: %s  wire__IP  wire_port data_port\n",argv[0] );
		exit(1);
	}

	/* Connecting to the physical wire */
	wireportno = atoi(argv[2]);
	wiresockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (wiresockfd < 0)
		error("ERROR opening socket");

	wireserver = gethostbyname(argv[1]);
	if (wireserver == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &wireserv_addr, sizeof(wireserv_addr));
	wireserv_addr.sin_family = AF_INET;
	bcopy((char *)wireserver->h_addr_list[0], (char *)&wireserv_addr.sin_addr.s_addr, wireserver->h_length);
	wireserv_addr.sin_port = htons(wireportno);

	if (connect(wiresockfd,(struct sockaddr *) &wireserv_addr,sizeof(wireserv_addr)) < 0)
		error("ERROR connecting");

	/* Thread to receive from the physical wire and send off to network layer */
	pthread_t wirepth;	// this is our thread identifier
	pthread_create(&wirepth,NULL,rcvfromwiresend2network_layer, NULL);

	/* Create and listen to a socket for a connection from the network layer*/
	/* Local variables to listen for a connection from the network layer */
	int dataportno, datasockfd;

	int num_packets = 0;

	socklen_t clilen;

	struct sockaddr_in dataserv_addr, datacli_addr;

	packet in_packet;
	frame out_frame;

	datasockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(datasockfd < 0)
		error("ERROR opening socket");

	network_layersockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (network_layersockfd < 0)
		error("ERROR opening socket");

	bzero((char *) &dataserv_addr, sizeof(dataserv_addr));

	dataportno = atoi(argv[3]);

	dataserv_addr.sin_family = AF_INET;
	dataserv_addr.sin_addr.s_addr = INADDR_ANY;
	dataserv_addr.sin_port = htons(dataportno);

	if (bind(datasockfd, (struct sockaddr *) &dataserv_addr, sizeof(dataserv_addr)) < 0)
		error("ERROR on binding");

	listen(datasockfd, 5);

	clilen = sizeof(datacli_addr);

	/* Accept just one request from the network layer */
	network_layersockfd = accept(datasockfd, (struct sockaddr *) &datacli_addr, &clilen);

	close(datasockfd);

	/* Receive messages from the network layer and package them up into frames to be sent to physical wire*/
	while (1)
	{
		/* Receive packet from network layer */
		n = read(network_layersockfd, &in_packet, sizeof(packet));
		if(n < 0)
			error("ERROR reading from network layer");

		/* Packaging into a frame structure */
		out_frame.my_packet = in_packet;
		out_frame.seq_num = num_packets++;
		out_frame.type = 0;

		/* Send frame to physical wire */
		n = write(wiresockfd, &out_frame, sizeof(frame));
		if(n < 0)
			error("ERROR writing to wire");

		/* Check to see if the message "EXIT". If true, close socket */
		if (strcmp (in_packet.message, "EXIT\n")==0)
		{
			pthread_cancel(wirepth);
			close(wiresockfd);
			close (network_layersockfd);
			return 0;
		}
	}
}

