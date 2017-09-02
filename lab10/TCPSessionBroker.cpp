#include "TCPSocket.h"
#include "TCPMessengerProtocol.h"
#include "TCPSessionBroker.h"
#include <stdlib.h>
#include <string>

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
	string userName = messengerServer->socketToUser[sender];
	messengerServer->sendDataToPeer(receiver, userName);
	response = messengerServer->readCommandFromPeer(receiver);
	if (response == ACCEPT) {
		//sending the peers their connection information
		messengerServer->sendCommandToPeer(sender, START_SESSION_WITH_PEER);
		messengerServer->sendDataToPeer(sender, connectionDetails(receiver, 44444, 44445));
//		messengerServer->sendDataToPeer(sender, receiver->destIpAndPort());
		messengerServer->sendCommandToPeer(receiver, START_SESSION_WITH_PEER);
		messengerServer->sendDataToPeer(receiver, connectionDetails(sender, 44445, 44444));
//		messengerServer->sendDataToPeer(receiver, sender->destIpAndPort());
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
//	return (*item).second;
	return (*item).second != peer1 ? (*item).second : NULL;
}

string TCPSessionBroker::connectionDetails(TCPSocket* peer2Socket, int peer2Port, int peer1Port){
	string peerData;
	string ip = peer2Socket->fromAddr();
	string peer1port = to_string(peer1Port);
	string peer2port = to_string(peer2Port);
	peerData.append(ip);
	peerData.append(":");
	peerData.append(peer1port);
	peerData.append(":");
	peerData.append(peer2port);
	return peerData;
}

void TCPSessionBroker::run() {
	messengerServer->markPeerAsUnavailable(peer1);
	if (peer2 != NULL && peer1 != peer2) {
		messengerServer->markPeerAsUnavailable(peer2);
		//sleep(2);
		if (CreateP2PSession()) {
			startGame();
		}else {
			string userName = messengerServer->socketToUser[peer2];
			messengerServer->sendCommandToPeer(peer1, REFUSE);
			messengerServer->sendDataToPeer(peer1, userName+" Refused your request");
		}
	} else {
		int buffer = 10;
		int i;
		for (i=0 ; i < buffer; i++) {
			peer2 = pickRandomPeer();

			if (peer2 == NULL) {
				break;
			}

			messengerServer->markPeerAsUnavailable(peer2);
			if (CreateP2PSession()) {
				startGame();
				break;
			}else { messengerServer->markPeerAsAvailable(peer2);}

		}
		if (i == buffer){
			messengerServer->sendCommandToPeer(peer1, REFUSE);
			messengerServer->sendDataToPeer(peer1, "Did't found players");
		}
	}
	cout << "mark peer as available" << endl;
	messengerServer->markPeerAsAvailable(peer1);
	messengerServer->markPeerAsAvailable(peer2);
}

void TCPSessionBroker::updateScore(TCPSocket* peer){
	int score;
	score = atoi(messengerServer->readDataFromPeer(peer).c_str());
	string userName = messengerServer->socketToUser[peer];
	User* user = messengerServer->users[userName];
	user->setScore(user->getScore() + score);
	messengerServer->writeUsersToFile();
	cout << user->getUsername() <<" got : " << user->getScore() << endl;
}

void TCPSessionBroker::startGame(){
	bool run = true;
	bool user1Active = true;
	bool user2Active = true;
	MultipleTCPSocketsListener multipleTCPSocketsListener;
	multipleTCPSocketsListener.addSocket(peer1);
	multipleTCPSocketsListener.addSocket(peer2);
	while (run) {

		vector<TCPSocket*> senders = multipleTCPSocketsListener.listenToSocket();
		vector<TCPSocket*>::iterator iter = senders.begin();

		for (; iter != senders.end(); iter++) {
			TCPSocket* sender = *iter;

			int command = messengerServer->readCommandFromPeer(sender);
			switch (command) {
			case CLOSE_SESSION_WITH_PEER: {
			updateScore(sender);
//				messengerServer->markPeerAsAvailable(sender);
				if (peer1 == sender)
					user1Active = false;
				else if (peer2 == sender)
					user2Active = false;

				if (!user1Active && !user2Active)
					run = false;
				break;
			}
			default: {
				cout << "default case" << endl;
				break;
			}
			}
		}
	}
}
