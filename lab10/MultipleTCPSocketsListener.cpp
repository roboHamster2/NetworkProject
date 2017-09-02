

#include "MultipleTCPSocketsListener.h"

using namespace std;



void MultipleTCPSocketsListener::addSocket(TCPSocket* socket){
	sockets.push_back(socket);
}

void MultipleTCPSocketsListener::addSockets(tSocketsContainer socketVec){
	sockets = socketVec;
}

vector<TCPSocket*> MultipleTCPSocketsListener::listenToSocket(int timeout){
	struct timeval tv = {timeout, 0};
	tSocketsContainer::iterator iter = sockets.begin();
	tSocketsContainer::iterator endIter = sockets.end();
	fd_set fdset;
	FD_ZERO(&fdset);
	int highfd = 0;
	//fill the set with file descriptors
	for (;iter != endIter;iter++) {
		highfd++;
		FD_SET((*iter)->getSocketFid(), &fdset);
	}
	//perform the select
	int returned;
	if (timeout>0){
		returned = select(sizeof(fdset)*8, &fdset, NULL, NULL, &tv);
	}else{
		cout<< "mmm " << highfd << endl;
		returned = select(sizeof(fdset)*8, &fdset, NULL, NULL, NULL);
		cout<< "The timeout is 0 : " << highfd << endl;
	}
	vector<TCPSocket*> availableSockets;
	if (returned) {
		for (int i = 0; i < highfd; i++) {
			TCPSocket* tmpSocket = sockets[i];
			if (FD_ISSET(tmpSocket->getSocketFid(), &fdset)) {
				availableSockets.push_back(tmpSocket);
			}
		}
	}
	return availableSockets;
}
