/*
 * http_request.c
 *
 * Program used to connect to a server, send a HTTP request, and read the response back
 * No HTML parsing is implemented, the information comes back as a stream.
 *
 * The program is compiled using the basic gcc command:
 * 	gcc http_request.c -std=c99 -o (output file name).exe
 * 		-std=c99:  used to enable c99 features such as for loops with declared variables
 * 		-o:		   used to specify output file name (if not given a.out is created)
 *
 * At execution time, user will have to give three arguments.
 * Running the program in terminal will follow this format:
 * 	./(output file name) (host name) (file path) (port number) > output.html
 * 	The piping will put all of the contents of the console into specified .html file,
 * 	otherwise .html content will be displayed on console.
 *
 *  Created on: Dec 2, 2015
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

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	// Declare variables
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    if (argc < 4) {
       fprintf(stderr,"usage %s hostname filepath port\n", argv[0]);
       exit(0);
    }

    char *request;
    char *testRequest;

    // Format string for HTTP request
    asprintf(&request, "GET %s HTTP/1.1\r\nHost:%s\r\n\r\n", argv[2], argv[1]);

    portno = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    n = write(sockfd, request, 255);
    if (n < 0)
    	error("ERROR writing to socket");

    // Read until the end of the data stream, print out to console
    do
    {
      n = read(sockfd, buffer, 255);
      if (n < 0)
        error("ERROR reading from socket");
      printf("%s\n",buffer);
    }while(n != 0);

    close(sockfd);
    return 0;
}

