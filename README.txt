/////////////////////////////////////////////////////////////////////////////////
// 	Authors: 		Ryan Quinn, Cameron Christopher, Christian Kiriluk
// 	Major: 			Computer Science
//	Creation Date:	November 30, 2021
//	Due Date: 		December 2, 2021
// 	Course: 		CSC328
//	Professor:		Dr. Frye
//	Assignment:		RPS Project
//	
//	Purpose:		This assignment contains a server/client program that runs
//                  a game of rock paper scissors between two users.
//
/////////////////////////////////////////////////////////////////////////////////

How to compile:
> make

How to run the server: 
> ./server [number of rounds] [port(optional)]

How to run the client: 
> ./client [hostname] [port(optional)]

/* needs manifest file/folder section here*/

/* needs section for our defined protocol */

Assumptions:
  1. Only two clients will be accepted at a time. 
  2. The two clients will stay connected through the entire game.
  3. Input guidelines on each prompt in the client will be followed (e.g. The user is expected to enter 
        of the presented choices in lowercase "Enter rock, paper, or scissors in lowercase: ").
  4. 

Discussions throughout development process:
  - 
  
Status:
  - Currently, our client/server program can run an entire game of rock, paper, scissors between two clients.
  - Our server can successfully run as a daemon server as long as the assumptions are followed. Specifically,
        our server will not operate correctly if assumption 2 is not followed by the user. If a connection is
        forcefully exited during the game, the server cannot handle it and will not allow any games to continue.
  - 
