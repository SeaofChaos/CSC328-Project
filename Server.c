// server code file
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
    }
  
  struct sockaddr_in server = {AF_INET, htons(PORT) ,INADDR_ANY};
  
  // set up the transport end point
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("socket call failed");
      exit(-1);
    }   // end if
  
  // bind and address to the end point
  if (bind(sockfd, (struct sockaddr *)&server, SIZE) == -1)
    {
      perror("bind call failed");
      exit(-1);
    }   // end if bind
  
  // start listening for incoming connections
  if (listen(sockfd, 5) == -1)
    {
      perror("listen call failed");
      exit(-1);
    }   // end if
	
	for (;;)
    {
		int scoreP1 = 0, scoreP2 = 0;
		char uniq[50];
		char nick1[50], nick2[50];
		int pid;
		int newsockfd;
		int p2c[2], c2p[2];
		
		if (pipe(p2c) < 0) {
			perror("Error creating pipe for parent to child:");
			exit(-1);
		}   // end if

		// Create a second pipe to send data child to  parent
		if (pipe(c2p) < 0) {
			perror("Error creating pipe for child to parent:");
			exit(-1);
		}   // end if
		
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
			close(c2p[READ]);
			
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
			read(p2c[READ], &uniq, sizeof(uniq)); //read uniqueness response
			if (rv < 0)
				perror("Error reading from parent pipe2");
			
			printf("uniq: %s\n", uniq);
			
			printf("value: %d\n", strcmp(uniq, "READY"));
			
			while (strcmp(uniq, "READY") != 0)
			{
				printf("hello\n");
				rv = send(newsockfd, uniq, sizeof(uniq), 0); // send uniq of RETRY response to client
				if (rv < 0)
					perror("Error sending to socket3");
				printf("goo\n");
				rv = recv(newsockfd, nick, sizeof(nick),0); // receive nickname from client
				if (rv < 0)
					perror("Error receiving from socket4");
				printf("poopy\n");
				rv = write(c2p[WRITE], &nick, sizeof(nick));  // write nickname to parent
				if (rv < 0)
						perror("Error writing to pipes5");
				printf("mommy\n");
				rv = read(p2c[READ], &uniq, sizeof(uniq)); //read uniqueness response
				if (rv < 0)
						perror("Error reading from pipes6");
				//printf("About to send to client: READY\n");
				
				rv = send(newsockfd, uniq, sizeof(uniq), 0); // send uniq of READY response to client
				if (rv < 0)
					perror("Error sending to socket7");
			}
			//sleep(0.5);
			/*rv = write(c2p[WRITE], &uniq, sizeof(uniq));  // write nickname to parent
			if (rv < 0)
				perror("Error sending to socket7");
			*/
			//i don't know why this printf doesn't print, but we don't talk worry about it
			//because it works :)
			printf("About to send to client: %s\n", uniq);
			
			rv = send(newsockfd, uniq, sizeof(uniq), 0); // send uniq of READY response to client
			if (rv < 0)
				perror("Error sending to socket7");
			
			printf("Got past sending READY to client\n");
			
			//writing message back to parent that one child has received message
			rv = write(c2p[WRITE], "RECEIVED", 9);  // write nickname to parent
				if (rv < 0)
						perror("Error writing to pipes5");
			
			//play game
            for (int x = 0; x < numRounds; x++) {
                char* goString = "GO";
				printf("About to send 'GO'\n");
				rv = send(newsockfd, goString, sizeof(goString), 0); // send GO to client - Ready for RPS selection
				if (rv < 0)
					perror("Error sending to socket");
				printf("Sent 'GO'\n");
				
                char choice[50];
                rv = recv(newsockfd, choice, sizeof(choice), 0); // receive choice from client
                if (rv < 0)
                    perror("Error receiving from socket");
				printf("Choice received from client: %s\n", choice);
				
				rv = read(p2c[READ], &uniq, sizeof(uniq)); //read PLAYER# from parent
				if (rv < 0)
					perror("Error writing to socket");
				
				rv = write(c2p[WRITE], nick, sizeof(nick)); //pipe choice to parent
				if (rv < 0)
					perror("Error writing to socket");
				
				rv = write(c2p[WRITE], &choice, sizeof(choice)); //pipe choice to parent
				if (rv < 0)
					perror("Error writing to socket");
				if (x < numRounds)
					send(newsockfd, "NEXT ROUND", 11, 0);
				else{
					send(newsockfd, "SCORE", 6, 0);
					break;
				}
				
				rv = read(p2c[READ], &uniq, sizeof(uniq)); //make sure both children are done
				if (rv < 0)
					perror("Error writing to socket");
            }
			
			close(p2c[READ]); //close unneeded pipes
			close(c2p[WRITE]);
			close(sockfd);
			close(newsockfd);
			exit(0);
		}
	  	else  // PARENT Process 
		{
			close(p2c[READ]); //close unneeded pipes
			close(c2p[WRITE]);
			
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
						rv = read(c2p[READ], &nick1, sizeof(nick1)); //read nickname from client 1
					}
				}
			}
			
			for (int x = 0; x < numRounds; x++) {
                char c1[10], c2[10];
				char player1[50], player2[50];
				
				rv = write(p2c[WRITE], "PLAYER1", 8);
                if (rv < 0)
                    perror("Error writing to pipes");
				rv = read(c2p[READ], player1, sizeof(player1));
                if (rv < 0)
                    perror("Error reading from pipes");
				rv = read(c2p[READ], c1, sizeof(c1));
                if (rv < 0)
                    perror("Error reading from pipes");
                
				rv = write(p2c[WRITE], "PLAYER2", 8);
                if (rv < 0)
                    perror("Error writing to pipes");
				rv = read(c2p[READ], player2, sizeof(player2));
                if (rv < 0)
                    perror("Error reading from pipes");
				rv = read(c2p[READ], c2, sizeof(c2));
                if (rv < 0)
                    perror("Error reading from pipes");
				
				printf("Player1 move: %s\nPlayer2 move: %s\n", c1, c2);
				
                int ref = decide(c1, c2);
                switch (ref) { //scoring
                    case '0':
                        printf("It's a tie.\n");
                    case '1':
						if (strcmp(player1, nick1) == 0){
							printf("Player1 wins\n");
							scoreP1++;
						}
						else{
							scoreP2++;
							printf("Player2 wins\n");
						}
                    case '2':
                        if (strcmp(player1, nick1) == 0){
							scoreP2++;
							printf("Player2 wins\n");
						}
						else{
							scoreP1++;
							printf("Player1 wins\n");
						}
                    case '4':
                        printf("Round Canceled: Invalid Entry");
                        break;
                }
            }
			close(p2c[WRITE]);
			close(c2p[READ]);
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
