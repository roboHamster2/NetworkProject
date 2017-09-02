#ifndef PEERSREQUESTSDISPATCHER_H_
#define PEERSREQUESTSDISPATCHER_H_

#include "TCPMessengerServer.h"
#include "MThread.h"

class TCPMessengerServer;
/**
 * Reads incoming commands from open peers and performs the required operations
 */
class PeersRequestsDispatcher: public MThread {
	TCPMessengerServer* messenger;

public:
	/**
	 * constructor that receive a reference to the parent messenger server
	 */
	PeersRequestsDispatcher(TCPMessengerServer* mesgr);
	bool stopIteration = false;
	/**
	 * The Dispatcher main loop
	 */
	void run();

private:
	void HandleCommandFromPeer(TCPSocket* readyPeer);

};

#endif /* PEERSREQUESTSDISPATCHER_H_ */
