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
  int PORT = 7007;
  int sockfd, rv;
  struct sockaddr_in server = {AF_INET, htons(PORT) ,INADDR_ANY};
  int numChild = 0;
  
  if (argc < 2) //usage clause
    {
      printf("\nusage: %s <number of rounds> <port number>(optional) \n\n", argv[0]);
      exit(1);
    }
  int numRounds = *argv[1]; // set number of rounds 
  
  if (argv[2] != NULL) // Set port if user entered 2nd CLA
    {
      PORT = *argv[1];
    }
  
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
		char uniq[5];
		char nick1[50],nick2[50];
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
			
			char nick[50];
			
			close(p2c[WRITE]); //close unneeded pipes
			close(c2p[READ]);
			
			rv = send(newsockfd, ready, 50, 0); // send ready string 
			if (rv < 0)
				perror("Error sending to socket1");
		  
			rv = recv(newsockfd, nick, 50,0); // receive nickname from client
			if (rv < 0)
				perror("Error receiving from socket2");
		  
			//printf("Nickname received in child: %s\n", nick);
		  
			write(c2p[WRITE], &nick, 50);  // write nickname to parent
			read(p2c[READ], &uniq, 5); //read uniqueness response
			while (strcmp(uniq, "READY") != 0)
			{
				rv = send(newsockfd, uniq, 5, 0); // send uniq of RETRY response to client
				if (rv < 0)
					perror("Error sending to socket3");
				rv = recv(newsockfd, nick, 50,0); // receive nickname from client
				if (rv < 0)
					perror("Error receiving from socket4");
				
				rv = write(c2p[WRITE], &nick, 50);  // write nickname to parent
				if (rv < 0)
						perror("Error writing to pipes5");
				rv = read(p2c[READ], &uniq, 5); //read uniqueness response
				if (rv < 0)
						perror("Error reading from pipes6");
			}
			
			//i don't know why this printf doesn't print, but we don't talk worry about it
			//because it works :)
			//printf("About to send to client: READY\n");
			
			rv = send(newsockfd, uniq, 5, 0); // send uniq of READY response to client
			if (rv < 0)
				perror("Error sending to socket7");
			
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
			
			int isReady = 0;
			while (isReady != 1)
			{
				rv = read(c2p[READ], &nick1, 50); //read nickname from client 1
				if (rv < 0)
					perror("Error reading from pipes8");
				
				rv = read(c2p[READ], &nick2, 50); // read nickname from client 2
				if (rv < 0)
					perror("Error reading from pipes9");
				
				//printf("Nick1 received in parent: %s\n", nick1);
				//printf("Nick2 received in parent: %s\n", nick2);
				
				rv = strcmp(nick1, nick2); // check uniqueness 
				if (rv == 0) // send retry if not unique
				{
					rv = write(p2c[WRITE], "RETRY", 5);
					if (rv < 0)
						perror("Error writing to pipes10");
					rv = write(p2c[WRITE], "RETRY", 5);
					if (rv < 0)
						perror("Error writing to pipes11");
				}
				else //send ready if unique
				{
					isReady = 1;
					rv = write(p2c[WRITE], "READY", 5);
					if (rv < 0)
						perror("Error writing to pipes12");
					rv = write(p2c[WRITE], "READY", 5);
					if (rv < 0)
						perror("Error writing to pipes13");
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
