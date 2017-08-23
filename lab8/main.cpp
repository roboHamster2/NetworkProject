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
UDPGame* game;
bool run;

void startGameWithPeer(){
	cout<<"GameStarted"<<endl;
	game->showGameMenu();
}

void handleConsoleCommand(){
	string command;
	cin >> command;
		if (command == "ou")
		{
			string userName;
			cin >> userName;
			if(messenger->open(userName))
				startGameWithPeer();
		}
		else if (command == "or")
		{
			if(messenger->openRandom())
				startGameWithPeer();
		}
		else if (command == "su")
		{
//			string msg;
//			getline(std::cin,msg);
//			if(msg.size()>0 && msg[0] == ' ')
//				msg.erase(0,1);
//			if (messenger->isActiveClientSession())
//				messenger->send(msg);
//			else
//				cout<<"Error: not connected to peer"<<endl;
			messenger->showAvailableUsers();
		}
		else if (command == "du")
		{
			messenger->closeActiveSession();
		}
		else if (command == "ds")
		{
			messenger->disconnect();
		}
		else if (command == "x")
		{
			run = false;
		} else {
			cout<<"invalid command"<<endl;
		}
}

void startGameMainLoop(){
	messenger->showMenu();
	game = NULL;
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
			if(game != NULL){
				//handleGameCommand();
			}else
				handleConsoleCommand();
		}
	}
}

int main(){

	messenger = new TCPMessengerClient();
	run = messenger->connectToServer();
	if (run)
		run = messenger->loginToServer();

	startGameMainLoop();

	messenger->disconnect();
	delete messenger;
	return 0;
}


#endif /* MAIN_CPP_ */
