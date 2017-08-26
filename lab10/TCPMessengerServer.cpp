#include "TCPMessengerServer.h"
#include "TCPMessengerProtocol.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
//for Ubuntu
//#include <tr1/functional>
//for iOS
#include <functional>


using namespace std;


TCPMessengerServer::TCPMessengerServer() {
	tcpServerSocket = new TCPSocket(MSNGR_PORT);
	running = false;
	dispatcher = NULL;
	authDispatcher = NULL;
}

User* TCPMessengerServer::getUserBySocket(TCPSocket* socket){
	map<string, TCPSocket*>::iterator item;
	for (item = openedPeers.begin(); item != openedPeers.end() ; item++){
		if ((*item).second == socket){
				return users[(*item).first];
			}
	}
	return NULL;
}

void TCPMessengerServer::run() {

	this->users = loadUsersFromFile(USERS_FILE);
//	User * u1 = new User("etr","ert",1);
//	User * u2 = new User("etr2","ert2",2);
//	User * u3 = new User("etr3","ert3",3);

//	vector<User*> users;
//	users.push_back(new User("etr","ert",1));
//	users.push_back(new User("etr2","ert2",2));
//	users.push_back(new User("etr3","ert3",3));

//	vector <User*> usersToSave;
//
//	for (std::map<string,User*>::iterator it = this->users.begin(); it != this->users.end(); ++it) {
//		usersToSave.push_back(it->second);
//	}
//
//	writeUsersToFile(USERS_FILE, usersToSave);

	running = true;
	dispatcher = new PeersRequestsDispatcher(this);
	dispatcher->start();
	authDispatcher = new AuthRequestsDispatcher(this);
	authDispatcher->start();
	while (running) {
		TCPSocket* peerSocket = tcpServerSocket->listenAndAccept();
		if (peerSocket != NULL) {
			cout << "new peer connected: " << peerSocket->destIpAndPort()
					<< endl;
			//openedPeers[peerSocket->destIpAndPort()] = peerSocket;
			unauthenticatedPeers.push_back(peerSocket);
		}
	}
}

void TCPMessengerServer::close() {
	cout << "closing server" << endl;
	running = false;
	tcpServerSocket->cclose();
	tOpenedPeers::iterator iter = openedPeers.begin();
	tOpenedPeers::iterator endIter = openedPeers.end();
	for (; iter != endIter; iter++) {
		((*iter).second)->cclose();
	}
	dispatcher->waitForThread();
	iter = openedPeers.begin();
	endIter = openedPeers.end();
	for (; iter != endIter; iter++) {
		delete (*iter).second;
	}

	vector<TCPSocket*>::iterator unauthIter = unauthenticatedPeers.begin();
	vector<TCPSocket*>::iterator unauthEndIter = unauthenticatedPeers.end();
	for (; unauthIter != unauthEndIter; unauthIter++) {
		((*unauthIter))->cclose();
	}
	authDispatcher->waitForThread();
	unauthIter = unauthenticatedPeers.begin();
	unauthEndIter = unauthenticatedPeers.end();
	for (; unauthIter != unauthEndIter; unauthIter++) {
		delete (*unauthIter);
	}

	vector<User*> usersToSave;

	for (std::map<string, User*>::iterator it = this->users.begin();it != this->users.end(); ++it) {
		usersToSave.push_back(it->second);
	}

	writeUsersToFile(USERS_FILE, usersToSave);
	cout << "server closed" << endl;
}

void TCPMessengerServer::showPeers() {
	tOpenedPeers::iterator iter = openedPeers.begin();
	tOpenedPeers::iterator endIter = openedPeers.end();
	for (; iter != endIter; iter++) {
		cout << (*iter).second->destIpAndPort() << endl;
	}
}

TCPSocket* TCPMessengerServer::getAvailablePeerByName(string peerName) {
	tOpenedPeers::iterator iter = openedPeers.find(peerName);
	tOpenedPeers::iterator endIter = openedPeers.end();
	if (iter == endIter) {
		return NULL;
	}
	return (*iter).second;
}

vector<TCPSocket*> TCPMessengerServer::getPeersVec() {
	vector<TCPSocket*> vec;
	tOpenedPeers::iterator iter = openedPeers.begin();
	tOpenedPeers::iterator endIter = openedPeers.end();
	for (; iter != endIter; iter++) {
		vec.push_back((*iter).second);
	}
	return vec;
}

vector<TCPSocket*> TCPMessengerServer::getUnathenticatedPeersVec() {
	return unauthenticatedPeers;
}


void TCPMessengerServer::peerDisconnect(TCPSocket* peer) {
	openedPeers.erase(peer->destIpAndPort());
	busyPeers.erase(peer->destIpAndPort());
	unauthenticatedPeers.erase(std::remove(unauthenticatedPeers.begin(), unauthenticatedPeers.end(), peer), unauthenticatedPeers.end());

	peer->cclose();
	delete peer;
}

void TCPMessengerServer::markPeerAsUnavailable(TCPSocket* peer) {
	openedPeers.erase(peer->destIpAndPort());
	busyPeers[peer->destIpAndPort()] = peer;
}

void TCPMessengerServer::markPeerAsAvailable(TCPSocket* peer) {
	busyPeers.erase(peer->destIpAndPort());
	openedPeers[peer->destIpAndPort()] = peer;
}

void TCPMessengerServer::peerDisconnect(string peerName) {
	TCPSocket* peer = openedPeers[peerName];
	if(peer != NULL){
		peer->cclose();
		delete peer;
	}
	openedPeers.erase(peerName);
	peer = busyPeers[peerName];
	if(peer != NULL){
			peer->cclose();
			delete peer;
		}
	busyPeers.erase(peerName);

}

void TCPMessengerServer::markPeerAsUnavailable(string peerName) {
	TCPSocket* sok = openedPeers[peerName];
	if(sok != NULL){
		busyPeers[peerName] = sok;
		openedPeers.erase(peerName);
	}
}

void TCPMessengerServer::markPeerAsAvailable(string peerName) {
	TCPSocket* sok = busyPeers[peerName];
		if(sok != NULL){
			openedPeers[peerName] = sok;
			busyPeers.erase(peerName);
		}
}

int TCPMessengerServer::readCommandFromPeer(TCPSocket* peer) {
	int command = 0;
	int rt = peer->recv((char*) &command, 4);
	if (rt < 1)
		return rt;
	command = ntohl(command);
	return command;
}

string TCPMessengerServer::readDataFromPeer(TCPSocket* peer) {
	string msg;
	char buff[BUFFER_SIZE];
	int msgLen;
	peer->recv((char*) &msgLen, 4);
	msgLen = ntohl(msgLen);
	int totalrc = 0;
	int rc;
	while (totalrc < msgLen) {
		rc = peer->recv((char*) &buff[totalrc], msgLen - totalrc);
		if (rc > 0) {
			totalrc += rc;
		} else {
			break;
		}
	}
	if (rc > 0 && totalrc == msgLen) {
		buff[msgLen] = 0;
		msg = buff;
	} else {
		peer->cclose();
	}
	return msg;
}

void TCPMessengerServer::sendCommandToPeer(TCPSocket* peer, int command) {
	command = htonl(command);
	peer->send((char*) &command, 4);
}

void TCPMessengerServer::sendDataToPeer(TCPSocket* peer, string msg) {
	int msgLen = msg.length();
	msgLen = htonl(msgLen);
	peer->send((char*) &msgLen, 4);
	peer->send(msg.data(), msg.length());
}

void TCPMessengerServer::writeUsersToFile(string path, vector<User*> users) {
	ofstream myfile;
	myfile.open(path.c_str());
	for (std::vector<User*>::iterator it = users.begin() ; it != users.end(); ++it){
		myfile << (*it)->getUsername()<<","<< (*it)->getPassword()<<","<< (*it)->getScore() << "\n";
	}
	myfile.close();
}

map<string,User*> TCPMessengerServer::loadUsersFromFile(string path){
	ifstream file(path.c_str());
	string   line;
	map<string,User*> users;

	while(getline(file, line))
	{
		string word;
		stringstream stream(line);
		vector<string> splited;
		while (std::getline(stream, word, ','))
		        splited.push_back(word);
		if(splited.size() == 3){
			User* r = new User(splited[0],splited[1],std::atoi(splited[2].c_str()));
			users[r->getUsername()]= r;
		}
	}
	return users;
}
//string TCPMessengerServer::tr1::registerUser(string name, string password){
string TCPMessengerServer::registerUser(string name, string password){
	if (name.length() < 4 && password.length() < 4){
		return "password and user must be at least 4 characters";
	}
	if (users[name]!=NULL){
		return "user already taken";
	}
	std::hash<string> hash_fn;
	size_t str_hash = hash_fn(password);
	int res = str_hash ;
	stringstream ss;
	ss << res;
	string str = ss.str();
	users[name] = new User(name,str,0);
	return "OK";
}

//string TCPMessengerServer::tr1::loginUser(string name, string password){
string TCPMessengerServer::loginUser(string name, string password){
	if (name.length() < 4 && password.length() < 4){
		return "password and user must be at least 4 characters";
	}
	if (users[name]==NULL){
		return "user does not exist";
	}
	std::hash<string> hash_fn;
	size_t str_hash = hash_fn(password);
	int res = str_hash ;
	stringstream ss;
	ss << res;
	string str = ss.str();
	User* user = users[name];
	if (user != NULL && str.compare(user->getPassword()) == 0){
		return "OK";
	} else {
		return "Password is incorrect";
	}
}

void TCPMessengerServer::markPeerAsAuthenticated(string username,TCPSocket* socket) {
	if(socket != NULL){
		unauthenticatedPeers.erase(std::remove(unauthenticatedPeers.begin(), unauthenticatedPeers.end(), socket), unauthenticatedPeers.end());
		socketToUser[socket] = username;
		userToSocket[username] = socket;
		openedPeers[username] = socket;
	}
}
