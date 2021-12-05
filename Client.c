
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "Library.c"

char* tolowerString(char* str){
	char *lower = malloc(sizeof(str));
	for (int i=0; i<sizeof(str); ++i)
		lower[i] = tolower(str[i]);
	return lower;
}

void getNickName(int sockfd){
	char nickname[50];
	char response[50];
	
	printf("Choose a nickname (under 50 characters): ");
	scanf("%s", nickname);
	
	if (send(sockfd, nickname, sizeof(nickname), 0) < 0){
		printf("Unable to send data to server\n");
	}
	if (recv(sockfd, response, sizeof(response), 0) < 0){
		printf("Unable to read data from server\n");
	}
	
	printf("\nReady or retry: %s\n", response);
	
	if (strcmp(response, "READY") != 0){
		printf("\nNickname is not unique.\nPlease enter a new nickname.\n\n");
		getNickName(sockfd);
	}
}

void getRPS(int sockfd){
	char choice[10];
	printf("Rock, paper, or scissors (type in lowercase): ");
    scanf("%s", choice);
	tolowerString(choice);
	printf("Inputted choice in lowercase: %s\n", choice);
	
    if (strcmp(choice, "rock") != 0 && 
			strcmp(choice, "paper") != 0 && 
			strcmp(choice, "scissors") != 0){
		
		printf("\nInvalid choice, please enter \"rock\", \"paper\", or \"scissors\"\n\n");
		getRPS(sockfd);
	}
	
	if (send(sockfd, choice, sizeof(choice), 0) < 0){
		printf("Unable to read data from server\n");
	}
	
	/*if (recv(sockfd, choice, sizeof(choice), 0) < 0){
		printf("Unable to read data from server\n");
	}*/
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
	
	printf("Waiting for READY...\n");
	if (recv(sockfd, &received, sizeof(received), 0) < 0){
		printf("Unable to read data from server\n");
	}
	printf("%s received\n\n", received);
	
	char keepPlaying = 'y';
	
	while (keepPlaying == 'y'){  //game loop
		char *toSend[50];
		
		getNickName(sockfd); //gets a unique nickname
		
		//waiting for "GO"
		if (recv(sockfd, &received, sizeof(received), 0) < 0){
			printf("Unable to read data from server\n");
		}
		printf("received (looking for GO): %s!\n", received);
		
		//keep running until "SCORE" is received
		while (strcmp(received, "SCORE") != 0){
			getRPS(sockfd);	//gets and sends an inputted move to server
			
			//get response from server, where "SCORE" will stop game
			if (recv(sockfd, &received, sizeof(received), 0) < 0){
				printf("Unable to read data from server\n");
			}
			printf("received: %s\n", received);
		}
		keepPlaying = 'n';
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
