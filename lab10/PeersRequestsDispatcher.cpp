#include "TCPMessengerServer.h"
#include "TCPSessionBroker.h"
#include "PeersRequestsDispatcher.h"


PeersRequestsDispatcher::PeersRequestsDispatcher(TCPMessengerServer* mesgr) {
	messenger = mesgr;
}

void PeersRequestsDispatcher::run() {
	cout << "dispatcher started" << endl;
	while (messenger->running) {
		MultipleTCPSocketsListener msp;
		msp.addSockets(messenger->getPeersVec());
		vector<TCPSocket*> readyPeers = msp.listenToSocket(2);
		vector<TCPSocket*>::iterator iter = readyPeers.begin();
		for (;iter != readyPeers.end();iter++) {
			TCPSocket* readyPeer = *iter;
			HandleCommandFromPeer(readyPeer);
		}
	}
	cout << "dispatcher ended" << endl;
}

void PeersRequestsDispatcher::HandleCommandFromPeer(TCPSocket* readyPeer){
	int command = messenger->readCommandFromPeer(readyPeer);
	string pName;
	TCPSocket* scondPeer;
	switch (command) {
	case OPEN_SESSION_WITH_PEER:
		pName = messenger->readDataFromPeer(readyPeer);
		scondPeer = messenger->getAvailablePeerByName(pName);
		if (scondPeer != NULL) {
			TCPSessionBroker* broker = new TCPSessionBroker(messenger,
					readyPeer, scondPeer);
			broker->start();
		} else {
			cout << "FAIL: didnt find peer:" << pName << endl;
			messenger->sendCommandToPeer(readyPeer, REFUSE);
			messenger->sendDataToPeer(readyPeer,"user not found");
		}
		break;
	case OPEN_RANDOM_SESSION:{
			TCPSessionBroker* broker = new TCPSessionBroker(messenger,readyPeer,NULL);
			broker->start();
		}
		break;
	case LIST_USERS:{
		messenger->sendCommandToPeer(readyPeer, LIST_USERS);
		messenger->sendDataToPeer(readyPeer,messenger->getAvailablePeers());
		}
		break;
	default:
		cout << "peer disconnected: " << readyPeer->destIpAndPort() << endl;
		messenger->peerDisconnect(readyPeer);
		break;
	}
}
