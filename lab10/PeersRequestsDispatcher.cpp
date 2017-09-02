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
		for (; iter != readyPeers.end(); iter++) {
			TCPSocket* readyPeer = *iter;
			HandleCommandFromPeer(readyPeer);
		}
	}
	cout << "dispatcher ended" << endl;
}

void PeersRequestsDispatcher::HandleCommandFromPeer(TCPSocket* readyPeer) {
	int command = messenger->readCommandFromPeer(readyPeer);
	if (command > 0 && command <= 20) {
		string pName;
		TCPSocket* scondPeer;
		switch (command) {
		case OPEN_SESSION_WITH_PEER:
			pName = messenger->readDataFromPeer(readyPeer);
			scondPeer = messenger->getAvailablePeerByName(pName);
			if (scondPeer != NULL && scondPeer != readyPeer) {
				TCPSessionBroker* broker = new TCPSessionBroker(messenger,
						readyPeer, scondPeer);
				broker->start();
			} else if (scondPeer == NULL) {
				cout << "FAIL: didn't find peer:" << pName << endl;
				messenger->sendCommandToPeer(readyPeer, REFUSE);
				messenger->sendDataToPeer(readyPeer, "user not found");
			} else if (scondPeer == readyPeer) {
				messenger->sendCommandToPeer(readyPeer, REFUSE);
				messenger->sendDataToPeer(readyPeer,
						"User cannot play with itself");
			}
			break;
		case OPEN_RANDOM_SESSION: {
			if (messenger->hasAvailablePeers(readyPeer)) {
				TCPSessionBroker* broker = new TCPSessionBroker(messenger, readyPeer, NULL);
				broker->start();
			}else {
				cout << "Did not find any peers " << endl;
				messenger->sendCommandToPeer(readyPeer, REFUSE);
			}

		}
			break;
		case LIST_USERS:{
				messenger->sendCommandToPeer(readyPeer, LIST_USERS);
				messenger->sendDataToPeer(readyPeer,messenger->getAvailablePeers(readyPeer));
			}
			break;
		case SHOW_SCORE:{
				messenger->sendCommandToPeer(readyPeer, SHOW_SCORE);
				map<TCPSocket*,string>::iterator it = messenger->socketToUser.find(readyPeer);
					if(it != messenger->socketToUser.end())
					{
					   //element found;
						string userName = it->second;
						messenger->sendDataToPeer(readyPeer,messenger->getPositionAndScore(messenger->users[userName]));
					}

			}
			break;
		case DISCONNECT: {
			cout << "peer disconnected " << readyPeer->destIpAndPort() << endl;
			messenger->markPeerAsUnauthenticated(readyPeer);
			messenger->sendCommandToPeer(readyPeer, DISCONNECT);
			break;
		}
		case DISCONNECT_FROM_SERVER: {
			cout << "peer disconnected " << readyPeer->destIpAndPort() << endl;
			messenger->sendCommandToPeer(readyPeer, DISCONNECT_FROM_SERVER);
			messenger->peerDisconnect(readyPeer);
			break;
		}
		default:
		{
			cout << "peer disconnected: " << readyPeer->destIpAndPort() << endl;
			messenger->peerDisconnect(readyPeer);
			break;
		}
		}
	}
}
