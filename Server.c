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


int newsockfd;
 
int main(int argc, char **argv)
{
  char *uniq = malloc(sizeof(char)*5);
  char nick1[50],nick2[50];
  int p2c[2], c2p[2];
  char nick[50];
  char *ready = "READY";
  int PORT = 7006;
  int sockfd, rv;
  struct sockaddr_in server = {AF_INET, htons(PORT) ,INADDR_ANY};
  int pid;
  int numChild = 0;
  
    // Create a pipe to send data parent to child
  if (pipe(p2c) < 0) {
    perror("Error creating pipe for parent to child:");
    exit(-1);
  }   // end if

  // Create a second pipe to send data child to  parent
  if (pipe(c2p) < 0) {
    perror("Error creating pipe for child to parent:");
    exit(-1);
  }   // end if
  
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
  
  // spawn a child to deal with the connection
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
  
	for (;;)
    {
		if (pid == 0) // CHILD Process
		{
			// accept connection
			newsockfd = accept(sockfd, NULL, NULL);
			if (newsockfd == -1)
			{
				perror("accept call failed");
				exit(-1);
			}   // end if
			
			close(p2c[WRITE]);//close unneeded pipes
			close(c2p[READ]);
		  
			rv = send(newsockfd, ready, 50, 0); // send ready string 
			if (rv < 0)
				perror("Error sending to socket");
		  
			rv = recv(newsockfd, nick, 50,0); // receive nickname from client
			if (rv < 0)
				perror("Error receiving from socket");
		  
			printf("Nickname received in child: %s\n", nick);
		  
			write(c2p[WRITE], &nick, 50);  // write nickname to parent
			read(p2c[READ], &uniq, 5); //read uniqueness response
			while (strcmp(uniq, "READY") != 0)
			{
				rv = send(newsockfd, uniq, 5, 0); // send uniq of RETRY response to client
				if (rv < 0)
					perror("Error sending to socket");
				rv = recv(newsockfd, nick, 50,0); // receive nickname from client
				if (rv < 0)
					perror("Error receiving from socket");
				
				write(c2p[WRITE], &nick, 50);  // write nickname to parent
				read(p2c[READ], &uniq, 5); //read uniqueness response
			}
			printf("About to send to client: %s\n", uniq);
			rv = send(newsockfd, uniq, 5, 0); // send uniq of READY response to client
			if (rv < 0)
				perror("Error sending to socket");
			
			close(newsockfd);
			exit(0);
		}
	  	else  // PARENT Process 
		{
			// parent doesn't need newsockfd
			close(newsockfd);
			close(p2c[READ]); //close unneeded pipes
			close(c2p[WRITE]);
		  
			while (strcmp(uniq, "READY") != 0)
			{
				rv = read(c2p[READ], &nick1, 50); //read nickname from client 1
				if (rv < 0)
					perror("Error reading from socket");
				
				rv = read(c2p[READ], &nick2, 50); // read nickname from client 2
				if (rv < 0)
					perror("Error reading from socket");
				
				printf("Nick1 received in parent: %s\n", nick1);
				printf("Nick2 received in parent: %s\n", nick2);
				
				rv = strcmp(nick1, nick2); // check uniqueness 
				if (rv == 0) // send retry if not unique
				{
					uniq = "RETRY";
					printf("%s\n",uniq);
					rv = write(p2c[WRITE], &uniq, 5);
					if (rv < 0)
						perror("Error writing to socket");
					rv = write(p2c[WRITE], &uniq, 5);
					if (rv < 0)
						perror("Error writing to socket");
				}
				else //send ready if unique
				{
					uniq = "READY";
					printf("%s\n",uniq);     
					rv = write(p2c[WRITE], &uniq, 5);
					if (rv < 0)
						perror("Error writing to socket");
					rv = write(p2c[WRITE], &uniq, 5);
					if (rv < 0)
						perror("Error writing to socket");
				}
			}	
			close(p2c[WRITE]);
			close(c2p[READ]);
		} //end else
    } //end for (game loop)
	
	if (wait(NULL) < -1)
		perror("Error in wait: ");
} //end main
