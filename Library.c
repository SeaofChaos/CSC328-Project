
/*************************************************************************/
/* */
/* Function name: checkPort */
/* Description: Checks that the given port is valid */
/* int givenPort: the port the user supplied */
/* Return Value: int - the port given if valid or a defualt port if it is invalid  */
/* */
/*************************************************************************/
int checkPort(int givenPort)
{
    if(givenPort<1024||givenPort>49151)
        return defaultPort();
 
    else 
        return givenPort;
}

/*************************************************************************/
/* */
/* Function name: defaultPort */
/* Description: returns a valid port number*/
/* Return Value: int -a port number  */
/* */
/*************************************************************************/
int defaultPort()
{
return 7011;
}

/*************************************************************************/
/* */
/* Function name: socketFail */
/* Description: prints error then exits*/
/* */
/*************************************************************************/
void socketFail()
{
	perror("Socket call failed");
	exit(1);
}



char *  sendFail()
{
	perror("Failed to send message");
	exit(1);
	
}


char *  send(char * toSend)
{	
	int rv = send(newsockfd, ready, 50, 0); // send ready string 
	      if (rv < 0)
		perror("Error sending to socket");
	      
	      rv = recv(newsockfd, toSend, 50,0); // receive nickname from client
	      if (rv < 0)
		perror("Error receiving from socket");
}


void createSock(int sockfd)
{
	if ((sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1)
		socketFail()
	
	if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1)
		socketFail()
}


);
}
