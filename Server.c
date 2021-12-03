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

int newsockfd;

int main(int argc, char **argv)
{
  char newmsg[50];
  char *msg = "pingus";
  char *PORT = getDefaultPort();;
  int   sockfd, rv;
  int pid;
  int numChild = 0;
  
  if (argc < 2) //usage clause
    {
      printf("\nusage: %s <number of rounds> <port number>(optional) \n\n", argv[0]);
      exit(1);
    }
  int numRounds = *argv[1]; // set number of rounds 
  
  if (argv[2] != NULL) // Set port if user entered 2nd CLA
    {
      PORT = argv[1];
    }
	
	printf("%d", atoi(PORT));
	
	struct sockaddr_in server = {AF_INET, atoi(PORT),INADDR_ANY};
  
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
      // accept connection
      // if returns an EINTR (interrupted system call) then can re-call
      while (((newsockfd = accept(sockfd, NULL, NULL)) == -1) &&
	     (errno == EINTR));
      if (newsockfd == -1)
	{
	  perror("accept call failed");
	  exit(-1);
	}   // end if
      
      // spawn a child to deal with the connection
      if ((pid = fork()) == -1)
	{
	  perror("fork failed");
	  exit(-1);
	}   // end if
      
      if (pid == 0)
	{
	  rv = send(newsockfd, msg, 50, 0); // attempt to send char string 
	  if (rv < 0)
	    perror("Error sending to socket");
	  rv = recv(newsockfd, newmsg, sizeof(newmsg),0); // attempt to receive
	  if (rv < 0)
	    perror("Error receiving from socket");
	  printf("%s\n",newmsg); // print number of connections to server 
	  close(newsockfd);
	  
	  exit(0);
	}   // end child process
	  
      // parent doesn't need newsockfd
      close(newsockfd);
      numChild++;
      printf("%d\n",numChild);
    }   // end for
  
  // basic wait for all the children created
  while (numChild > 0) {
	if (wait(NULL) < -1)
	  perror("Error in wait: ");
	else
	  numChild--;
  }  // end while
}
