
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


int defaultPort()
{
return 7011;
}

void socketFail()
{
		perror("Socket call failed");
		exit(1);
}



char *  receiveFail()
{
	if (recv(sockfd, received, sizeof(received), 0) < 0)
		printf("Unable to read data from server");
	
}


char *  sendFail(char *send)
{
 	if (send(sockfd, toSend, sizeof(toSend), 0) < 0)
		printf("Unable to send data to server"); 
}

void createSock()
{
	if ((sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1)
		socketFail()
	
	if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1)
		socketFail()
}


void createVars()
{
char *received = malloc(50);	
char *toSend = malloc(50);	
}

void freeVars()
{
close(sockfd);
free(toSend)	
free(received);
}
