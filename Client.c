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
//	How to compile:	make
/////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "Library.c"

void getNickName(int sockfd){
	char nickname[50];
	char response[6];
	
	printf("Choose a nickname (under 50 characters): ");
	scanf("%s", nickname);
	
	if (send(sockfd, nickname, sizeof(nickname), 0) < 0){
		printf("Unable to send data to server\n");
	}
	if (recv(sockfd, response, sizeof(response), 0) < 0){
		printf("Unable to read data from server\n");
	}
	
	if (strcmp(response, "READY") != 0){
		printf("\nNickname is not unique.\nPlease enter a new nickname.\n\n");
		getNickName(sockfd);
	}
}

void getRPS(int sockfd){
	char choice[8];
	printf("Rock, paper, or scissors (type in lowercase): ");
    scanf("%s", choice);

    if (strcmp(choice, "rock") != 0 && 
			strcmp(choice, "paper") != 0 && 
			strcmp(choice, "scissors")){
				
		printf("\nInvalid choice, please enter \"rock\", \"paper\", or \"scissors\"\n\n");
		getRPS(sockfd);
	}
	
	if (recv(sockfd, choice, sizeof(choice), 0) < 0){
		printf("Unable to read data from server\n");
	}
}

int main(int argc, char* argv[]){
	
	if ((argc < 2) || (argc > 3)){	//proper usage check
		printf("Usage: %s hostname port(optional)\n", argv[0]);
		return 0;
	} 
	
	char* type = argv[1];
	char* port = "7006";
	
	if (argc == 3){	//is port specified?
		port = argv[2];
	} 
	
	//declarations
	int sockfd, s;
	struct addrinfo hints, *result;
	char received[50];
	
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
	
	printf("Waiting for READY...");
	if (recv(sockfd, &received, sizeof(received), 0) < 0){
		printf("Unable to read data from server\n");
	}
	printf("READY received");
	
	char keepPlaying = 'y';
	
	while (keepPlaying == 'y'){  //game loop
		char *toSend[50];
		
		getNickName(sockfd); //gets a unique nickname
		keepPlaying = 'n';
		//keep running until "SCORE" is received
		/*while (strcmp(received, "SCORE") != 0){
			//waiting for "GO"
			while(1){
				if (recv(sockfd, &received, sizeof(received), 0) < 0){
					printf("Unable to read data from server\n");
				}
				if (strcmp(received, "GO") == 0) //received "GO"
					break;
			}
			
			getRPS(sockfd);	//gets and sends an inputted move to server
			
			//get response from server, where "SCORE" will stop game
			if (recv(sockfd, &received, sizeof(received), 0) < 0){
				printf("Unable to read data from server\n");
			}
		}
		*/
		/*printf("Would you like to play again? (y/n): ");
		scanf("%c", toSend);
		
		if (send(sockfd, toSend, 1, 0) < 0){
			printf("Unable to send data to server\n");
		}
		
		//receives char for keepPlaying, 'y' being to keep going, 'n' to stop
		if (recv(sockfd, &keepPlaying, 1, 0) < 0){
			printf("Unable to read data from server\n");
		}*/
		//printf("Received from server: %s\n", received);
	}
	close(sockfd);
	
	return 0;
}
