// server code file
// Authors: Ryan Quinn, Cameron Christopher, Christian Kiriluk

//Major: Computer Science

//Creation Date: November 30, 2021

//Due Date: December 2, 2021

//Course: CSC328

//Professor: Dr. Frye

//Assignment: RPS Project

//Purpose: This assignment contains a server/client program that runs a game of rock paper scissors between two users.

// Compile: make
// Execute: ./server <number of rounds> <(optional
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
 
#include "Library.c"

#define SIZE sizeof(struct sockaddr_in)
#define MAXLINE 512
#define READ      0
#define WRITE     1

int main(int argc, char **argv)
{
  char *uniq;
  char nick[50];
  char *ready = "READY";
  int PORT = 7008;
  int sockfd, rv;
  int numChild = 0;
  
	if (argc < 2) //usage clause
    {
		printf("\nusage: %s <number of rounds> <port number>(optional) \n\n", argv[0]);
		exit(1);
    }
	int numRounds = atoi(argv[1]); // set number of rounds 
	printf("numRounds: %d\n", numRounds);
	if (argv[2] != NULL) // Set port if user entered 2nd CLA
    {
		PORT = atoi(argv[2]);
		if (checkPort(PORT) == 0){
			printf("Port must be in range 1024-49151\n");
			exit(1);
		}
    }
  
	struct sockaddr_in server = {AF_INET, htons(PORT) ,INADDR_ANY};
  
	// set up the transport end point
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
		perror("socket call failed");
		exit(-1);
    }
  
	// bind and address to the end point
	if (bind(sockfd, (struct sockaddr *)&server, SIZE) == -1)
    {
		perror("bind call failed");
		exit(-1);
    }
  
	// start listening for incoming connections
	if (listen(sockfd, 5) == -1)
    {
		perror("listen call failed");
		exit(-1);
    }
	
	for (;;)
    {
		int scoreP1 = 0, scoreP2 = 0;
		char uniq[50];
		char nick1[50], nick2[50];
		int pid;
		int newsockfd;
		int p2c[2], c2p[2], p2c2[2], c2p2[2]; //created extra pipe to allow parent to keep the children in order
		
		if (pipe(p2c) < 0) {
			perror("Error creating pipe for parent to child:");
			exit(-1);
		}   // end i
		if (pipe(c2p) < 0) {
			perror("Error creating pipe for child to parent:");
			exit(-1);
		}   // end if
		if (pipe(c2p2) < 0) {
			perror("Error creating pipe for child to parent:");
			exit(-1);
		}   // end if
		if (pipe(p2c2) < 0) {
			perror("Error creating pipe for child to parent:");
			exit(-1);
		}   // end if
		
		//create child processes
		if ((pid = fork()) == -1)
		{
			perror("fork failed");
			exit(-1);
		}   // end if
		if (pid != 0){
			if ((pid = fork()) == -1)
			{
				perror("fork failed");
				exit(-1);
			}   // end if
		}
		if (pid == 0) // CHILD Process
		{
			// accept connection
			newsockfd = accept(sockfd, NULL, NULL);
			if (newsockfd == -1)
			{
				if (errno==EINTR) continue;
				perror("accept call failed");
				exit(-1);
			}   // end if
			
			printf("Connection received\n");
			
			char nick[50];
			
			close(p2c[WRITE]); //close unneeded pipes
			close(p2c2[WRITE]);
			close(c2p[READ]);
			close(c2p2[READ]);
			
			rv = send(newsockfd, ready, sizeof(ready), 0); // send ready string 
			if (rv < 0)
				perror("Error sending to socket1");
		  
			rv = recv(newsockfd, nick, sizeof(nick),0); // receive nickname from client
			if (rv < 0)
				perror("Error receiving from socket2");
		  
			printf("Nickname received in child: %s\n", nick);
			
			rv = write(c2p[WRITE], &nick, sizeof(nick));  // write nickname to parent
			if (rv < 0)
				perror("Error writing to parent pipe2");
			rv = read(p2c[READ], &uniq, sizeof(uniq)); //read uniqueness response
			if (rv < 0)
				perror("Error reading from parent pipe");
			
			while (strcmp(uniq, "READY") != 0)
			{
				rv = send(newsockfd, uniq, sizeof(uniq), 0); // send uniq of RETRY response to client
				if (rv < 0)
					perror("Error sending to socket");
				rv = recv(newsockfd, nick, sizeof(nick),0); // receive nickname from client
				if (rv < 0)
					perror("Error receiving from socket");
				rv = write(c2p[WRITE], &nick, sizeof(nick));  // write nickname to parent
				if (rv < 0)
						perror("Error writing to pipes5");
				rv = read(p2c[READ], &uniq, sizeof(uniq)); //read uniqueness response
				if (rv < 0)
						perror("Error reading from pipes6");
				rv = send(newsockfd, uniq, sizeof(uniq), 0); // send uniq of READY response to client
				if (rv < 0)
					perror("Error sending to socket7");
			}
			
			rv = send(newsockfd, uniq, sizeof(uniq), 0); // send uniq of READY response to client
			if (rv < 0)
				perror("Error sending to socket7");
			
			//writing message back to parent that one child has received message
			rv = write(c2p[WRITE], "RECEIVED", 9);  // write nickname to parent
			if (rv < 0)
					perror("Error writing to pipes");
			
			//play game
            //for (int x = 0; x < numRounds; x++) {
				// send GO to client - Ready for RPS selection
                char* goString = "GO";
				printf("About to send 'GO'\n");
				rv = send(newsockfd, goString, sizeof(goString), 0); 
				if (rv < 0)
					perror("Error sending to socket");
				printf("Sent 'GO'\n");
				
				//get choice from CLIENT
                char choice[50];
                rv = recv(newsockfd, choice, sizeof(choice), 0); // receive choice from client
				if (rv < 0)
					perror("Error receiving from socket");
				printf("Choice received from client: %s\n", choice);
				//printf("uniq before reading PLAYER#: %s\n", uniq);
				//read PLAYER#
				rv = write(c2p[WRITE], "START", 6); //pipe choice to parent
				if (rv < 0)
					perror("Error writing to pipes");
				rv = read(p2c[READ], &uniq, sizeof(uniq)); //read PLAYER# from parent (this doesn't matter)
				if (rv < 0)
					perror("Error reading from parent pipe");
				
				printf("Read from parent pipe: %s\n", uniq);
				
				rv = write(c2p[WRITE], &nick, sizeof(nick)); //write nickname to parent
				if (rv < 0)
					perror("Error writing to pipes");
				rv = write(c2p[WRITE], &choice, sizeof(choice)); //write choice to parent
				if (rv < 0)
					perror("Error writing to pipes");

				//if (x < numRounds)
					//send(newsockfd, "NEXT ROUND", 11, 0);
				//else{
					send(newsockfd, "SCORE", 6, 0); //send score back to stop game
					if (rv < 0)
						perror("Error sending to socket");
					//send(newsockfd, "SCORE", 6, 0);
					//break;
				//}
				//printf("ahhh\n");
				//rv = read(p2c[READ], &uniq, sizeof(uniq)); //make sure both children are done
            //}
			
			char winner[50];
			char winnerScore[10];
			char loser[50];
			char loserScore[10];
			
			//read from a single write in the parent so only 1 child can execute this
			printf("Child %s is trying to read from pipe", nick);
			rv = read(p2c2[READ], uniq, sizeof(uniq));
			if (rv < 0)
				perror("Error reading from parent pipe");
			
			//receive information from parent
			rv = read(p2c[READ], &winner, sizeof(winner));
			if (rv < 0)
				perror("Error reading from parent pipe");
				printf("Winner received: %s\n", winner);
			rv = read(p2c[READ], &winnerScore, sizeof(winnerScore));
			if (rv < 0)
				perror("Error reading from parent pipe");
				printf("Winner score received: %s\n", winnerScore);
			rv = read(p2c[READ], &loser, sizeof(loser));
			if (rv < 0)
				perror("Error reading from parent pipe");
				printf("Loser received: %s\n", loser);
			rv = read(p2c[READ], &loserScore, sizeof(loserScore));
			if (rv < 0)
				perror("Error reading from parent pipe");
				printf("Loser score received: %s\n", loserScore);
			
			rv = write(c2p2[WRITE], nick, sizeof(nick));
			if (rv < 0)
					perror("Error writing to pipes");
				printf("Finished reading information in player: %s\n", nick);
			
			printf("\nWinner name: %s\n", winner);
			printf("Winner score: %s\n", winnerScore);
			printf("Loser name: %s\n", loser);
			printf("Loser score: %s\n\n", loserScore);
			
			///send information to client
			send(newsockfd, winner, sizeof(winner), 0);
			if (rv < 0)
					perror("Error sending to socket");
			send(newsockfd, winnerScore, sizeof(winnerScore), 0);
			if (rv < 0)
					perror("Error sending to socket");
			send(newsockfd, loser, sizeof(loser), 0);
			if (rv < 0)
					perror("Error sending to socket");
			send(newsockfd, loserScore, sizeof(loserScore), 0);
			if (rv < 0)
					perror("Error sending to socket");
			rv = recv(newsockfd, uniq, sizeof(uniq), 0);
			if (rv < 0)
					perror("Error receiving from socket");
			
			printf("Should be DONE: %s\n", uniq);
			
			rv = read(p2c2[READ], uniq, sizeof(uniq));
			if (rv < 0)
				perror("Error reading from parent pipe");
			
			close(p2c[READ]); //close unneeded pipes
			close(p2c2[READ]);
			close(c2p[WRITE]);
			close(c2p2[WRITE]);
			close(sockfd);
			close(newsockfd);
			exit(0);
		}
	  	else  // PARENT Process 
		{
			close(p2c[READ]); //close unneeded pipes
			close(p2c2[READ]);
			close(c2p[WRITE]);
			close(c2p2[WRITE]);
			
			char *toSend = malloc(sizeof(char)*10);
			int isReady = 0;
			while (isReady != 1)
			{
				rv = read(c2p[READ], &nick1, sizeof(nick1)); //read nickname from client 1
				if (rv < 0)
					perror("Error reading from pipes12");
				
				rv = read(c2p[READ], &nick2, sizeof(nick2)); // read nickname from client 2
				if (rv < 0)
					perror("Error reading from pipes13");
				
				printf("Nick1 received in parent: %s\n", nick1);
				printf("Nick2 received in parent: %s\n", nick2);
				
				rv = strcmp(nick1, nick2); // check uniqueness 
				if (rv == 0) // send retry if not unique
				{
					toSend = "RETRY";
					for (int i=0; i<2; i++){
						printf("About to send: %s\n", toSend);
						rv = write(p2c[WRITE], toSend, sizeof(toSend));
						if (rv < 0)
							perror("Error writing to pipes14");
					}
				}
				else //send ready if unique
				{
					toSend = "READY";
					isReady = 1;
					for (int i=0; i<2; i++){
						//printf("About to send: %s\n", toSend);
						rv = write(p2c[WRITE], toSend, sizeof(toSend));
						if (rv < 0)
							perror("Error writing to pipes15");
						rv = read(c2p[READ], &uniq, sizeof(uniq)); //read nickname from client 1
						if (rv < 0)
							perror("Error writing to pipes15");
					}
				}
			}
			
				char c1[10], c2[10];
				char player1[50], player2[50];
				//int childNum = 1;
				
				for (int i=0; i<2; ++i){
					rv = read(c2p[READ], uniq, sizeof(uniq));
					if (i == 0){
						printf("getting player1 information:\n");
						printf("before PLAYER1 write\n");
						rv = write(p2c[WRITE], "PLAYER1", 8);
						if (rv < 0)
							perror("Error writing to pipes");
						printf("Read player1 nick\n");
						rv = read(c2p[READ], player1, sizeof(player1));
						if (rv < 0)
							perror("Error reading from parent pipe");
						
						printf("Should be player1: %s\n", player1);
						rv = read(c2p[READ], c1, sizeof(c1));
						if (rv < 0)
							perror("Error reading from parent pipe");
						
						printf("Should be player1's move: %s\n", c1);
					}
					if (i == 1){
						rv = read(c2p[READ], uniq, sizeof(uniq));
						if (rv < 0)
							perror("Error reading from parent pipe");
						printf("getting player2 information:\n");
						printf("before PLAYER2 write\n");
						rv = write(p2c[WRITE], "PLAYER2", 8);
						if (rv < 0)
							perror("Error writing to pipes");
						printf("Read player2 nick\n");
						rv = read(c2p[READ], player2, sizeof(player2));
						if (rv < 0)
							perror("Error reading from parent pipe");
						
						printf("Should be player2: %s\n", player2);
						rv = read(c2p[READ], c2, sizeof(c2));
						if (rv < 0)
							perror("Error reading from parent pipe");
						printf("Should be player2's move: %s\n", c2);
					}
					//childNum = 2;
				}
				
				printf("Player1 move: %s\nPlayer2 move: %s\n", c1, c2);
				
                int ref = decide(c1, c2);
				printf("ref: %d\n", ref);
                switch (ref) { //scoring
                    case 0:{
                        printf("It's a tie.\n");
						break;
					}
                    case 1:{
						printf("player1, nick1: %s, %s\n", player1, nick1);
						if (strcmp(player1, nick1) == 0){
							printf("Player2 wins\n");
							scoreP2++;
						}
						else{
							scoreP1++;
							printf("Player1 wins\n");
						}
						break;
					}
                    case 2:{
						printf("player1, nick1: %s, %s\n", player1, nick1);
                        if (strcmp(player1, nick1) == 0){
							scoreP1++;
							printf("Player1 wins\n");
						}
						else{
							scoreP2++;
							printf("Player2 wins\n");
						}
						break;
					}
                    case 4:
                        printf("Round Canceled: Invalid Entry");
                        break;
                }
				printf("player1 score: %d\nplayer2 score: %d\n", scoreP1, scoreP2);
				
				char winnerScore[10];
				char loserScore[10];
				
				for (int i=0; i<2; ++i){  //iterate 2 times, one for each child depending read nickname
					
					printf("Writing GO to p2c2 pipe.\n");
					rv = write(p2c2[WRITE], "GO", 3);
					if (rv < 0)
						perror("Error writing to pipes");
					if (i == 0){ //communicating with only Player1 child
						
						if (scoreP1 > scoreP2){
							sprintf(winnerScore, "%d", scoreP1);
							sprintf(loserScore, "%d", scoreP2);
							rv = write(p2c[WRITE], player1, sizeof(player1));  //send winner nickname
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], winnerScore, sizeof(winnerScore));
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], player2, sizeof(player2));  //send loser nickname
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], loserScore, sizeof(loserScore));
							if (rv < 0)
								perror("Error writing to pipes");
						}
						else{
							sprintf(winnerScore, "%d", scoreP2);
							sprintf(loserScore, "%d", scoreP1);
							rv = write(p2c[WRITE], player2, sizeof(player2));  //send winner nickname
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], winnerScore, sizeof(winnerScore));  //send winner score
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], player1, sizeof(player1));  //send loser nickname
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], loserScore, sizeof(loserScore));  //send loser score
							if (rv < 0)
								perror("Error writing to pipes");
						}
					}
					else { //communicating with only Player2 child
						if (scoreP1 > scoreP2){
							sprintf(winnerScore, "%d", scoreP1);
							sprintf(loserScore, "%d", scoreP2);
							rv = write(p2c[WRITE], player1, sizeof(player1));  //send winner nickname
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], winnerScore, sizeof(winnerScore));
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], player2, sizeof(player2));  //send loser nickname
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], loserScore, sizeof(loserScore));
							if (rv < 0)
								perror("Error writing to pipes");
						}
						else {
							sprintf(winnerScore, "%d", scoreP2);
							sprintf(loserScore, "%d", scoreP1);
							rv = write(p2c[WRITE], player2, sizeof(player2));  //send winner nickname
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], winnerScore, sizeof(winnerScore));  //send winner score
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], player1, sizeof(player1));  //send loser nickname
							if (rv < 0)
								perror("Error writing to pipes");
							rv = write(p2c[WRITE], loserScore, sizeof(loserScore));  //send loser score
							if (rv < 0)
								perror("Error writing to pipes");
						}
					}
					//rv = read(c2p[READ], uniq, sizeof(uniq));*/
					printf("Checking if first child is done.\n");
					rv = read(c2p2[READ], uniq, sizeof(uniq));
					if (rv < 0)
						perror("Error reading from parent pipe");
					printf("First child is done, received: %s\n", uniq);
				}
			
			rv = write(p2c[WRITE], "DONE", 5);
			if (rv < 0)
					perror("Error writing to pipes");
			rv = write(p2c[WRITE], "DONE", 5);
			if (rv < 0)
					perror("Error writing to pipes");
            //}
			close(p2c[WRITE]);
			close(p2c2[WRITE]);
			close(c2p[READ]);
			close(c2p2[READ]);
		} //end else
		if (wait(NULL) < -1)
			perror("Error in wait: ");
		if (wait(NULL) < -1)
			perror("Error in wait: ");
    } //end for (game loop)
	
	close(sockfd);

	if (wait(NULL) < -1)
		perror("Error in wait: ");
	
	exit(0);
} //end main
