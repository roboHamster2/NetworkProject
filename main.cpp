//============================================================================
// Name        : TCPMessengerServer
// Author      : Eliav Menachi
// Version     :
// Copyright   :
// Description :
//============================================================================

#ifndef MAIN_CPP_SERVER_
#define MAIN_CPP_SERVER_

#include <iostream>
#include "TCPMessengerServer.h"
using namespace std;

void printInstructions() {
	cout << "-----------------------" << endl;
	cout << "sp - show peers" << endl;
	cout << "x - shutdown server" << endl;
	cout << "-----------------------" << endl;
}

int main() {
	cout << "Welcome to TCP messenger Server" << endl;
	printInstructions();
	TCPMessengerServer msngrServer;
	msngrServer.start();
	bool loop = true;
	while (loop) {
		string msg;
		string command;
		cin >> command;
		if (command == "sp") {
			msngrServer.showPeers();
		} else if (command == "x") {
			loop = false;
		} else {
			cout << "wrong input" << endl;
			printInstructions();
		}
	}
	msngrServer.close();
	msngrServer.waitForThread();
	cout << "messenger was closed" << endl;
	return 0;
}

#endif /* MAIN_CPP_SERVER_ */
