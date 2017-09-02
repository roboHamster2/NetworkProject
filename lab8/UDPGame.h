/*
 * UDPGame.h
 *
 *  Created on: Aug 19, 2017
 *      Author: user
 */

#ifndef UDPGAME_H_
#define UDPGAME_H_
#include "UDPSocket.h"

class UDPGame {
	string peer2IP;
	int peer2Port;
	UDPSocket* udpSocket;
public:
	UDPGame(string peerIP,int peerPort, int myPort);
	void showGameMenu();
	virtual ~UDPGame();
	int startGame();
	void sendTo(string msg,string ip,int port);

	/**
	 * reply to an incoming message, this method will send the given message
	 * the peer from which the last message was received.
	 */
	void reply(string msg);

	/**
	 * close the messenger and all related objects (socket)
	 */
	void close();

	/**
	 * This method runs in a separate thread, it reads the incoming messages
	 * from the socket and prints the content on the terminal.
	 * The thread should exist when the socket is closed
	 */
};

#endif /* UDPGAME_H_ */
