/*
 * AuthRequestsDispatcher.cpp
 *
 *  Created on: Aug 19, 2017
 *      Author: user
 */




#include "TCPMessengerServer.h"
#include "TCPSessionBroker.h"
#include "AuthRequestsDispatcher.h"


AuthRequestsDispatcher::AuthRequestsDispatcher(TCPMessengerServer* mesgr) {
	messenger = mesgr;
}

void AuthRequestsDispatcher::run() {
	cout << "dispatcher started" << endl;
	while (messenger->running) {
		MultipleTCPSocketsListener msp;
		msp.addSockets(messenger->getUnathenticatedPeersVec());
		vector<TCPSocket*> readyPeers = msp.listenToSocket(2);
		vector<TCPSocket*>::iterator iter = readyPeers.begin();
		for (;iter != readyPeers.end();iter++) {
			TCPSocket* readyPeer = *iter;
			HandleCommandFromPeer(readyPeer);
		}
	}
	cout << "dispatcher ended" << endl;
}

void AuthRequestsDispatcher::HandleCommandFromPeer(TCPSocket* readyPeer){
	int command = messenger->readCommandFromPeer(readyPeer);
	string pName;
	TCPSocket* scondPeer;
	switch (command) {
	case OPEN_SESSION_WITH_PEER:
		pName = messenger->readDataFromPeer(readyPeer);
		cout << "got open session command:" << readyPeer->destIpAndPort()
				<< "->" << pName << endl;
		if (pName == TEST_PEER_NAME) {
			cout << "open test session" << endl;
			messenger->sendCommandToPeer(readyPeer, SESSION_ESTABLISHED);
			messenger->markPeerAsUnavailable(readyPeer);
			TCPSessionBroker* broker = new TCPSessionBroker(messenger,
					readyPeer, NULL);
			broker->start();
		} else {
			scondPeer = messenger->getAvailablePeerByName(pName);
			if (scondPeer != NULL) {
				messenger->sendCommandToPeer(readyPeer,
				SESSION_ESTABLISHED);
				messenger->sendCommandToPeer(scondPeer,
				OPEN_SESSION_WITH_PEER);
				messenger->sendDataToPeer(scondPeer,
						readyPeer->destIpAndPort());
				messenger->markPeerAsUnavailable(scondPeer);
				messenger->markPeerAsUnavailable(readyPeer);
				TCPSessionBroker* broker = new TCPSessionBroker(messenger,
						readyPeer, scondPeer);
				broker->start();
			} else {
				cout << "FAIL: didnt find peer:" << pName << endl;
				messenger->sendCommandToPeer(readyPeer, SESSION_REFUSED);
			}
		}
		break;
	default:
		cout << "peer disconnected: " << readyPeer->destIpAndPort() << endl;
		messenger->peerDisconnect(readyPeer);
		break;
	}
}
