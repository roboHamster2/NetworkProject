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
	User* user1;
	User* user2;
public:
	TCPSessionBroker(TCPMessengerServer* msgr, TCPSocket* p1, TCPSocket* p2);
	bool CreateP2PSession();
	void run();
	TCPSocket* pickRandomPeer();
private :
	void startGame();
	void updateScore(TCPSocket* peer);
	string connectionDetails(TCPSocket* peer2Socket, int peer2Port, int peer1Port);
};




#endif /* TCPSESSIONBROKER_H_ */
