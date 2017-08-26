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
	user1 = NULL;
	user2 = NULL;

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
	string userName = messengerServer->getUserBySocket(sender)->getUsername();
	messengerServer->sendDataToPeer(receiver, userName);
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

void TCPSessionBroker::setupUsers(){
	user1 = messengerServer->getUserBySocket(peer1);
	user2 = messengerServer->getUserBySocket(peer2);
}


void TCPSessionBroker::run() {
	messengerServer->markPeerAsUnavailable(peer1);
	if (peer2 != NULL) {
		messengerServer->markPeerAsUnavailable(peer2);
		if (CreateP2PSession()) {
			setupUsers();
			startGame();
		}else {
			string userName = messengerServer->getUserBySocket(peer2)->getUsername();
			messengerServer->sendCommandToPeer(peer1, REFUSE);
			messengerServer->sendDataToPeer(peer1, userName+" Refused your request");
		}
	} else {
		int buffer = 10;
		int i;
		for (i=0 ; i < buffer; i++) {
			peer2 = pickRandomPeer();
			messengerServer->markPeerAsUnavailable(peer2);
			if (CreateP2PSession()) {
				setupUsers();
				startGame();
				break;
			}else { messengerServer->markPeerAsAvailable(peer2);}

		}
		if (i == buffer){
			messengerServer->sendCommandToPeer(peer1, REFUSE);
			messengerServer->sendDataToPeer(peer1, "Did't found players");
		}
	}
	cout << "closing session:" << peer1->destIpAndPort() << " -> "
			<< peer1->destIpAndPort() << endl;
	messengerServer->markPeerAsAvailable(peer1);
	messengerServer->markPeerAsAvailable(peer2);
}

void TCPSessionBroker::updateScore(){
	int score1;
	int score2;
	score1 = atoi(messengerServer->readDataFromPeer(peer1).c_str());
	score2 = atoi(messengerServer->readDataFromPeer(peer2).c_str());
	user1->setScore(score1);
	user2->setScore(score2);
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
			updateScore();
			messengerServer->sendCommandToPeer(peer1, SESSION_ABOUT_TO_CLOSE);
			messengerServer->sendCommandToPeer(peer2, SESSION_ABOUT_TO_CLOSE);
			messengerServer->markPeerAsAvailable(peer1);
			messengerServer->markPeerAsAvailable(peer2);
			run = false;
			break;
		}
	}
}
