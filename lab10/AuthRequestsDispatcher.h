#ifndef AUTHREQUESTSDISPATCHER_H_
#define AUTHREQUESTSDISPATCHER_H_

#include "TCPMessengerServer.h"
#include "MThread.h"

class TCPMessengerServer;
/**
 * Reads incoming commands from open peers and performs the required operations
 */
class AuthRequestsDispatcher: public MThread {
	TCPMessengerServer* messenger;
public:
	/**
	 * constructor that receive a reference to the parent messenger server
	 */
	AuthRequestsDispatcher(TCPMessengerServer* mesgr);

	/**
	 * The Dispatcher main loop
	 */
	void run();

private:
	void HandleCommandFromPeer(TCPSocket* readyPeer);

};

#endif /* AUTHREQUESTSDISPATCHER_H_ */
