### ID Block
***
Authors: 		Ryan Quinn, Cameron Christopher, Christian Kiriluk

Major: 			Computer Science

Creation Date:	November 30, 2021

Due Date: 		December 2, 2021

Course: 		CSC328

Professor:		Dr. Frye

Assignment:		RPS Project

Purpose:		This assignment contains a server/client program that runs
				a game of rock paper scissors between two users.
***

### Running the program
How to compile:
```
> make
```

How to run the server: 
```
> ./server [number of rounds] [port(optional)]
```

How to run the client: 
```
> ./client [hostname] [port(optional)]
```

### Manifest files/folders
1. Client.c
    - The client program is used to connect to the server and interacts with the user
      to run the game properly. Once it receives information from the user, it sends
      it to the server.
2. Server.c
    - Contains the server code which, once compiled and executed, will open a daemon
      server and constantly listen for connections. Once two client connections are
      accepted, it will run the code for the game. When the game is finished, it closes
      the sockets connecting the clients and will wait for two more connections to run
      again.
3. Library.c
    - The library code which is used between both programs. Some common functionality
      includes error checking on sockets, sending/receiving information, and creating
      default values which are shared between the programs, like the default port.
4. makefile
    - The makefile allows for all the program files to be compiled in the proper order
      by inputting the command "make".

### Responsibility Matrix
|                       |  Ryan  Quinn  |  Cameron Christopher  | Christian Kiriluk |
| --------------------- | ------------- | --------------------- | ----------------- |
|       Client.c        |       Y       |           Y           |         Y         |
|       Server.c        |       Y       |           Y           |         Y         |
|       Library.c       |       Y       |           Y           |         Y         |
|        readme         |       Y       |           N           |         N         |
|       makefile        |       Y       |           N           |         N         |
| Updated Project Plan  |       Y       |           Y           |         Y         |

### Defined Protocol
- TCP/IP

### Assumptions:
  1. Only two clients will be accepted at a time. 
  2. The two clients will stay connected through the entire game.
  3. Input guidelines on each prompt in the client will be followed (e.g. The user is expected to enter 
        of the presented choices in lowercase "Enter rock, paper, or scissors in lowercase: ").
  4. 

### Discussions throughout development process:
  - 
  
### Status
  - Currently, our client/server program can run an entire game of rock, paper, scissors between two clients.
  - Our server can successfully run as a daemon server as long as the assumptions are followed. Specifically,
        our server will not operate correctly if assumption 2 is not followed by the user. If a connection is
        forcefully exited during the game, the server cannot handle it and will not allow any games to continue.
  - 
