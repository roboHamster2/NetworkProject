/*
 * AuthRequestsDispatcher.cpp
 *
 *  Created on: Aug 19, 2017
 *      Author: user
 */




#include "TCPMessengerServer.h"
#include "TCPSessionBroker.h"
#include "AuthRequestsDispatcher.h"
#include <sstream>

void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


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
	string input;
	string responseData;
	vector<string> tokens;
	switch (command) {
	case REGISTER:
		input = messenger->readDataFromPeer(readyPeer);
		Tokenize(input, tokens, ";");
		if(tokens.size() == 2){
			responseData = messenger->registerUser(tokens[0],tokens[1]);
			if(responseData.compare("OK") == 0){
				messenger->sendCommandToPeer(readyPeer,LOGIN_ACCEPT);
			} else {
				messenger->sendCommandToPeer(readyPeer,LOGIN_REFUSED);
				messenger->sendDataToPeer(readyPeer,responseData);
			}

		}
		break;
	case LOGIN:
			input = messenger->readDataFromPeer(readyPeer);
			Tokenize(input, tokens, ";");
			if(tokens.size() == 2){
				responseData = messenger->loginUser(tokens[0],tokens[1]);
				if(responseData.compare("OK") == 0){
					messenger->markPeerAsAuthenticated(tokens[0],readyPeer);
					messenger->sendCommandToPeer(readyPeer,LOGIN_ACCEPT);
				} else {
					messenger->sendCommandToPeer(readyPeer,LOGIN_REFUSED);
					messenger->sendDataToPeer(readyPeer,responseData);
				}

			}
			break;
	default:
		cout << "peer disconnected: " << readyPeer->destIpAndPort() << endl;
		messenger->peerDisconnect(readyPeer);
		break;
	}
}
