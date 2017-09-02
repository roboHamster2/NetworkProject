#ifndef TCPSESSIONBROKER_H_
#define TCPSESSIONBROKER_H_

#include "MThread.h"
#include "TCPSocket.h"
#include "TCPMessengerServer.h"

/**
 * Manages a session between two peers.
 */
class TCPSessionBroker: public MThread {
	TCPMessengerServer* messengerServer;
	TCPSocket* peer1;
	TCPSocket* peer2;
public:
	TCPSessionBroker(TCPMessengerServer* msgr, TCPSocket* p1, TCPSocket* p2);
	bool CreateP2PSession();
	void run();
	TCPSocket* pickRandomPeer();
private:
	void startGame();
};




#endif /* TCPSESSIONBROKER_H_ */
