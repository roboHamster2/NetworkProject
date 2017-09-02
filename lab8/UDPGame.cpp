/*
 * UDPGame.cpp
 *
 *  Created on: Aug 19, 2017
 *      Author: user
 */

#include "UDPGame.h"
#include "UDPSocket.h"
#include "string.h"

UDPGame::UDPGame(string peerIP,int peerPort, int myPort) {
	this->peer2IP = peerIP;
	this->peer2Port=peerPort;
	udpSocket = new UDPSocket(myPort);
//	startGame();
}

void UDPGame::showGameMenu(){
	cout<<"Choose Rock,Scissors,Paper: s  <message>"<<endl;
	cout<<"To reply to a message type: r <message>"<<endl;
	cout<<"To exit type: x"<<endl;
}

int UDPGame::startGame() {
	int score=0;
	cout << "Welcome to UDP game area" << endl;
	showGameMenu();
	bool invalidInput = true;

	string myCommand;
	while (invalidInput) {
		cin >> myCommand;
		if (!strcmp(myCommand.c_str(), "s") || !strcmp(myCommand.c_str(), "p") || !strcmp(myCommand.c_str(), "r") || !strcmp(myCommand.c_str(), "x"))
			invalidInput = false;
		else
			cout << "invalid input" << endl;
	}

	sendTo(myCommand, peer2IP, peer2Port);

	char buff[2];
	cout << "waiting for opponent to choose" << endl;
	int rc = udpSocket->recv(buff, 2);

	if (rc == 1) {
		buff[rc] = 0;

		bool runGame = true;
		if (!strcmp(myCommand.c_str(), "x"))
			runGame = false;

		if (runGame && !strcmp(buff, "x")){
			cout << "Your opponent quit Unexpectedly" << endl;
			runGame = false;
		}

		if(runGame)
		{
			cout << "the opponent chose:" << buff << "\"" << endl;
			cout << "You chose : " << myCommand << endl;
			if (!strcmp(myCommand.c_str(), "r")) {
				if (!strcmp(buff, "r")) {
					cout << "draw" << endl;
					score = 1;
				} else if (!strcmp(buff, "s")) {
					cout << "you won" << endl;
					score = 3;
				} else {
					cout << "you lose" << endl;
					score = 0;
				}
			} else if (!strcmp(myCommand.c_str(), "s")) {
				if (!strcmp(buff, "s")) {
					cout << "draw" << endl;
					score = 1;
				} else if (!strcmp(buff, "p")) {
					cout << "you won" << endl;
					score = 3;
				} else {
					cout << "you lose" << endl;
					score = 0;
				}
			} else {
				if (!strcmp(buff, "p")) {
					cout << "draw" << endl;
					score = 1;
				} else if (!strcmp(buff, "r")) {
					cout << "you won" << endl;
					score = 3;
				} else {
					cout << "you lose" << endl;
					score = 0;
				}
			}
		}
	}

	close();
	cout << "game has ended" << endl;
	return score;
}

void UDPGame::sendTo(string msg,string ip,int port){
	udpSocket->sendTo(msg,ip,port);
}

void UDPGame::reply(string msg){
	udpSocket->reply(msg);
}

void UDPGame::close(){
	udpSocket->close();
}


UDPGame::~UDPGame() {
	// TODO Auto-generated destructor stub
}
