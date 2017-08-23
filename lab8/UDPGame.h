/*
 * UDPGame.h
 *
 *  Created on: Aug 19, 2017
 *      Author: user
 */

#ifndef UDPGAME_H_
#define UDPGAME_H_
#include "TCPSocket.h"

class UDPGame {
	TCPSocket* peer2;
public:
	UDPGame(TCPSocket* peer2);
	void showGameMenu();
	virtual ~UDPGame();
};

#endif /* UDPGAME_H_ */
