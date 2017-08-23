#include "TCPSocket.h"
#include "TCPMessengerProtocol.h"
#include "TCPSessionBroker.h"
#include <stdlib.h>

TCPSessionBroker::TCPSessionBroker(TCPMessengerServer* msgr, TCPSocket* p1, TCPSocket* p2) {
	if (p2 == NULL)
		cout << "Create new Broker with AutoPlay initiated for : " << p1->destIpAndPort() << endl;
	else
		cout << "Create new broker between: " << p1->destIpAndPort() << " and "
				<< p2->destIpAndPort() << endl;
	peer1 = p1;
	peer2 = p2;
	messengerServer = msgr;

}

bool TCPSessionBroker::CreateP2PSession(){

	cout << "Broker thread started" << endl;
	TCPSocket* sender = peer1;
	TCPSocket* receiver = peer2;

	int response;
	//Send request to peer
	cout << "sending request form:" << sender->destIpAndPort() << " to: "
			<< receiver->destIpAndPort() << " " << endl;

	messengerServer->sendCommandToPeer(receiver, SEND_REQUEST_TO_PEER);
	//need to send to the receiver the username
	messengerServer->sendDataToPeer(receiver, sender->destIpAndPort());
	response = messengerServer->readCommandFromPeer(receiver);
	if (response == ACCEPT) {
		//sending the peers their connection information
		messengerServer->sendCommandToPeer(sender, START_SESSION_WITH_PEER);
		messengerServer->sendDataToPeer(sender, receiver->destIpAndPort());
		messengerServer->sendCommandToPeer(receiver, START_SESSION_WITH_PEER);
		messengerServer->sendDataToPeer(receiver, sender->destIpAndPort());
		return true;
	} else if (response == REFUSE) {
		return false;
	}
	cout << "Invalid Response command" << endl;
	return false;
}

TCPSocket* TCPSessionBroker::pickRandomPeer(){
	int sizeOfOpenPeers = messengerServer->openedPeers.size();
	map<string, TCPSocket*>::iterator item = messengerServer->openedPeers.begin();
	std::advance(item, rand() % sizeOfOpenPeers);
	return (*item).second;
}


void TCPSessionBroker::run() {
	messengerServer->markPeerAsUnavailable(peer1);
	if (peer2 != NULL) {
		messengerServer->markPeerAsUnavailable(peer2);
		if (CreateP2PSession()) {
			startGame();
		}else {messengerServer->sendCommandToPeer(peer1, REFUSE);}
	} else {
		int buffer = 10;
		int i;
		for (i=0 ; i < buffer; i++) {
			peer2 = pickRandomPeer();
			messengerServer->markPeerAsUnavailable(peer2);
			if (CreateP2PSession()) {
				startGame();
				break;
			}else { messengerServer->markPeerAsAvailable(peer2);}

		}
		if (i == buffer){
			messengerServer->sendCommandToPeer(peer1, REFUSE);
		}

	}
	cout << "closing session:" << peer1->destIpAndPort() << " -> "
			<< peer1->destIpAndPort() << endl;
	messengerServer->markPeerAsAvailable(peer1);
	messengerServer->markPeerAsAvailable(peer2);
}

void TCPSessionBroker::startGame(){
	bool run = true;
	while (run) {
		MultipleTCPSocketsListener multipleTCPSocketsListener;
		multipleTCPSocketsListener.addSocket(peer1);
		multipleTCPSocketsListener.addSocket(peer2);

		vector<TCPSocket*> senders = multipleTCPSocketsListener.listenToSocket();
		TCPSocket* sender = senders.at(0);

		int command = messengerServer->readCommandFromPeer(sender);
		switch (command){
		case CLOSE_SESSION_WITH_PEER:
			cout << "closing session:" << peer1->destIpAndPort() << " -> " << peer1->destIpAndPort() << endl;
			messengerServer->markPeerAsAvailable(peer1);
			messengerServer->markPeerAsAvailable(peer2);
			run = false;
			break;
		}
	}
}

//void TCPSessionBroker::CreateP2PSession(){
//
//	cout << "Broker thread started" << endl;
//	MultipleTCPSocketsListener multipleTCPSocketsListene;
//	multipleTCPSocketsListene.addSocket(peer1);
//	multipleTCPSocketsListene.addSocket(peer2);
//	bool run = true;
//	while (run) {
//		vector<TCPSocket*> senders = multipleTCPSocketsListene.listenToSocket();
//		TCPSocket* sender = senders.at(0);
//		TCPSocket* receiver = peer1;
//		if (receiver == sender)
//			receiver = peer2;
//		int command = messengerServer->readCommandFromPeer(sender);
//		string msg;
//		switch (command) {
//		//Send message to peer
//		case SEND_MSG_TO_PEER:
//			cout << "sending msg form:" << sender->destIpAndPort()
//					<< " to: " << receiver->destIpAndPort() << " " << msg
//					<< endl;
//			msg = messengerServer->readDataFromPeer(sender);
//			messengerServer->sendCommandToPeer(receiver, SEND_MSG_TO_PEER);
//			messengerServer->sendDataToPeer(receiver, msg);
//			break;
//			//Close a Connection between peers
//		case CLOSE_SESSION_WITH_PEER:
//			cout << "closing session between:" << sender->destIpAndPort()
//					<< " and: " << receiver->destIpAndPort() << endl;
//			messengerServer->sendCommandToPeer(receiver,
//					CLOSE_SESSION_WITH_PEER);
//			messengerServer->markPeerAsAvailable(sender);
//			messengerServer->markPeerAsAvailable(receiver);
//			run = false;
//			break;
//		}
//	}
//	cout << "closing broker:" << peer1->destIpAndPort() << "-"
//			<< peer2->destIpAndPort() << endl;
//}
