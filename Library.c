
//check that port given is valid
int checkPort(int givenPort)
{
    if(givenPort<1024||givenPort>49151)
        return 7011;
 
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

char *  receiveMsg(char *received)
{
  
}
