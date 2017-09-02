/*
 * main.cpp

 *
 *  Created on: June 20, 2017
 *      Author: user
 */
#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <iostream>
#include <string.h>
#include "TCPMessengerClient.h"
#include "UDPGame.h"

#define STDIN 0

using namespace std;

TCPMessengerClient* messenger;

bool run;

void handleConsoleCommand(){
	string command;
	cin >> command;
		if (command == "ou")
		{
			string userName;
			cin >> userName;
			if(messenger->open(userName))
				messenger->startGameWithPeer(true);
		}
		else if (command == "or")
		{
			if(messenger->openRandom())
				messenger->startGameWithPeer(true);
		}
		else if (command == "su")
		{
			messenger->showAvailableUsers();
		}
		else if (command == "ss")
		{
			messenger->showPosiotionAndScore();
		}
		else if (command == "du")
		{
			messenger->closeActiveSession();
		}
		else if (command == "ds")
		{
			messenger->disconnectFromServer();
		}
		else if (command == "x")
		{
			messenger->disconnectFromServer();
			run = false;
		} else {
			cout<<"invalid command"<<endl;
		}
}

void startMainLoop(){
	messenger->showMenu();

	fd_set master;   // master file descriptor list
	fd_set read_fds; // temp file descriptor list for select()

	FD_ZERO(&master); // clear the master and temp sets
	FD_ZERO(&read_fds);
	FD_SET(STDIN, &master);
	int socket = messenger->socket->getSocketFid();
	FD_SET(socket, &master);
	while(run == true){
		read_fds = master; // copy it
		if (select(socket + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			run= false;
		}
		if (FD_ISSET(socket, &read_fds))
		{
			messenger->handleServerCommand();
		}
		if(FD_ISSET(STDIN, &read_fds))
		{
			handleConsoleCommand();
		}
	}
}

int main(){

	messenger = new TCPMessengerClient();
	run = messenger->connectToServer();
	if (run)
	{
		run = messenger->loginToServer();
		if(run)
			startMainLoop();
		messenger->disconnectFromServer();
	}
	delete messenger;
	return 0;
}


#endif /* MAIN_CPP_ */
