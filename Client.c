/////////////////////////////////////////////////////////////
// 	Author: 		Ryan Quinn
// 	Major: 			Computer Science
//	Creation Date:	November 30, 2021
//	Due Date: 		December 2, 2021
// 	Course: 		CSC328 010
//	Professor:		Dr. Frye
//	Assignment:		RPS Project
//	FileName:		Client.cpp
//	
//	Purpose:		This program will run the client side of a 
//                  game of rock-paper-scissors that a user
//                  interacts with. It only deals with
//                  inputs and outputs of information to and
//                  from the server.
//
//	How to compile:	
/////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	
	if ((argc < 2) || (argc > 3)){	//proper usage check
		printf("Usage: %s hostname port(optional)\n", argv[0]);
		return 0;
	}
	
	char* type = argv[1];
	char* port = "7000";
	
	if (argc == 3){	//is port specified?
		port = argv[2];
	}
	
	//declarations
	int sockfd, s;
	struct addrinfo hints, *result;
	
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;
	
	//get IP address from hostname
    if ((s = getaddrinfo(argv[1], port, &hints , &result)) != 0) 
    {
        perror("Error getting dns");
        exit(1);
    }
	
	//create socket
	if ((sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1)
    {
		perror("Socket call failed");
		exit(1);
    }
	
	//connect socket
	if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1)
	{
		perror("Connect call failed");
		exit(1);
	}
	freeaddrinfo(result);
	
	char *received = malloc(sizeof(char));
	char *toSend = "a";
	//char *received = "hello";
	
	if (recv(sockfd, received, sizeof(received), 0) < 0){
		printf("Unable to read data from server");
	}
	if (send(sockfd, toSend, sizeof(toSend), 0) < 0){
		printf("Unable to send data to server");
	}
	
	printf("Received from server: %c\n", received);
	//printf("Sent to server: %s\n", received);
	
	close(sockfd);
	
	free(received);
	
	return 0;
}
