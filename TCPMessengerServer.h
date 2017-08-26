#ifndef TCPMESSENGERSERVER_H_
#define TCPMESSENGERSERVER_H_

#include <strings.h>
#include <map>
#include <vector>
#include "MThread.h"
#include "TCPSocket.h"
#include "MultipleTCPSocketsListener.h"
#include "TCPMessengerProtocol.h"
#include "PeersRequestsDispatcher.h"
#include "AuthRequestsDispatcher.h"
#include "User.h"

#define BUFFER_SIZE 1500
#define USERS_FILE "users.csv"

using namespace std;

class TCPMessengerServer: public MThread {
	friend class PeersRequestsDispatcher;
	friend class AuthRequestsDispatcher;
	friend class TCPSessionBroker;

	PeersRequestsDispatcher* dispatcher;
	AuthRequestsDispatcher* authDispatcher;
	TCPSocket* tcpServerSocket;
	bool running;

	map<string,User*> users;

	vector<TCPSocket*> unauthenticatedPeers;

	typedef map<string, TCPSocket*> tOpenedPeers;
	tOpenedPeers openedPeers;
	tOpenedPeers busyPeers;

public:
	/**
	 * Construct a TCP server socket
	 */
	TCPMessengerServer();

	/**
	 * the messenger server main loop- this loop waits for incoming clients connection,
	 * once a connection is established the new connection is added to the openedPeers
	 * on which the dispatcher handle further communication
	 */
	void run();

	/**
	 * print out the list of connected clients (for debug)
	 */
	void showPeers();

	/**
	 * returning the User by a TCPSocket
	 */
	User* getUserBySocket(TCPSocket* socket);

	/**
	 * close the server
	 */
	void close();

private:
	/**
	 * returns the open peers in a vector
	 */
	vector<TCPSocket*> getPeersVec();

	vector<TCPSocket*> getUnathenticatedPeersVec();

	/**
	 * return the open peer that matches the given name (IP:port)
	 * returns NULL if there is no match to the given name
	 */
	TCPSocket* getAvailablePeerByName(string peerName);

	/**
	 * remove and delete the given peer
	 */
	void peerDisconnect(TCPSocket* peer);

	/**
	 * move the given peer from the open to the busy peers list
	 */
	void markPeerAsUnavailable(TCPSocket* peer);

	/**
	 * move the given peer from the busy to the open peers list
	 */
	void markPeerAsAvailable(TCPSocket* peer);

	/**
	 * remove and delete the given peer
	 */
	void peerDisconnect(string peerName);

	/**
	 * move the given peer from the open to the busy peers list
	 */
	void markPeerAsUnavailable(string peerName);

	/**
	 * move the given peer from the busy to the open peers list
	 */
	void markPeerAsAvailable(string peerName);


	/**
	 * read command from peer
	 */
	int readCommandFromPeer(TCPSocket* peer);

	/**
	 * read data from peer
	 */
	string readDataFromPeer(TCPSocket* peer);

	/**
	 * send command to peer
	 */
	void sendCommandToPeer(TCPSocket* peer, int command);

	/**
	 * send data to peer
	 */
	void sendDataToPeer(TCPSocket* peer, string msg);

	/**
	 * write users to CSV
	 */
	void writeUsersToFile(string path,vector<User*> users);

	/**
	 * load users from CSV
	 */
	map<string,User*> loadUsersFromFile(string path);
};

#endif
