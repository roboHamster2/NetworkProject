#include "TCPMessengerClient.h"
#include "TCPMessengerProtocol.h"
#include "TCPSocket.h"


TCPMessengerClient::TCPMessengerClient(){
	socket = NULL;
	//sessionAddress = "";
	sessionActive = false;
	connected = false;
}

void TCPMessengerClient::showConnectMenu(){
	cout<<"c <server ip> - connect to server"<<endl;
	cout<<"x - exit"<<endl;
}

void TCPMessengerClient::showLoginMenu(){
	cout<<"r <userName, password> - Register to server"<<endl;
	cout<<"l <userName, password> - Login to server"<<endl;
	cout<<"x - exit"<<endl;
}

void TCPMessengerClient::showMenu(){
//need to add edge scenarios in server (like open connection when your connected)
	cout<<"ou <userName> - open new session"<<endl;
	cout<<"or - open random session"<<endl;
	cout<<"su - show available users"<<endl;
	cout<<"du - disconnect user"<<endl;
	cout<<"ds - disconnect from server"<<endl;
	cout<<"x - exit"<<endl;
}


/**
 * client main loop
 */
void TCPMessengerClient::handleServerCommand()
{
	int command = readCommand();
	switch (command){
		case SEND_REQUEST_TO_PEER:
		{
			string msg;
			msg = readCommandData();
			cout<<">>"<<msg<<"[y/n]"<<endl;
			string answear;
			cin>>answear;
			if(answear=="y"){
				sendCommand(ACCEPT);
				int comm = readCommand();
				if(comm!=START_SESSION_WITH_PEER)
					cout<< "got invalid command from server"<<endl;
				else{
					string peerDetails = readCommandData();
					//newUDPGAME
				}
			}else
				sendCommand(REFUSE);

			break;
		}
//		case CLOSE_SESSION_WITH_PEER:
//		{
//			cout<<"Session was closed by remote peer"<<endl;
//			break;
//		}
//		case OPEN_SESSION_WITH_PEER:
//		{
//			sessionAddress = readCommandData();
//			cout<<"Session was opened by remote peer: "<<sessionAddress<<endl;
//			sessionActive = true;
//			break;
//		}
//		case SESSION_ESTABLISHED:{
//			cout<<"Session was established"<<endl;
//			sessionActive = true;
//			break;
//		}
		default:
		{
			cout<<"communication with server was interrupted - connection closed"<<endl;
			socket->cclose();
			connected = false;
			sessionActive = false;
			//sessionAddress = "";
			delete socket;
			break;
		}
	}
}

bool TCPMessengerClient::loginToServer(){
	showLoginMenu();
	while(true){
		string command;
		cin >> command;
		if (command == "r")
		{
			string user;
			string password;
			cin >> user;
			cin >> password;
			if(authenticate(REGISTER ,user, password))
				return true;
			else
				cout<<"failed to register"<<endl;
		} else if(command == "l")
		{
			string user;
			string password;
			cin >> user;
			cin >> password;
			if(authenticate(LOGIN,user, password))
				return true;
			else
				cout<<"failed to login"<<endl;
		} else if (command == "x")
		{
			return false;
		} else {
			cout<<"invalid command"<<endl;
		}
	}
	return false;
}

bool TCPMessengerClient::authenticate(int authCommand, string user, string password){
	sendCommand(authCommand);
	sendCommandData(user+";"+password);
	int command = readCommand();
	if (command == LOGIN_ACCEPT)
	{
		sessionActive = true;
		return true;
	}
	else if(command == LOGIN_REFUSED)
	{
		string error = readCommandData();
		cout<<error<<endl;
	}
	else
		cout<<"unexpected command from server"<<endl;
	return false;
}

bool TCPMessengerClient::connectToServer(){
	showConnectMenu();
	while(true){
		string command;
		cin >> command;
		if (command == "c")
		{
			cout << "enter server ip" << endl;
			string ip;
			cin >> ip;
			if (connect(ip))
				return true;
		} else if (command == "x")
		{
			return false;
		} else {
			cout<<"invalid command"<<endl;
		}
	}

}

/**
 * connect to server and start mainLoop 
 */
bool TCPMessengerClient::connect(string ip){
	if(connected) disconnect();
	socket = new TCPSocket(ip,MSNGR_PORT); //port is located in TCPMessengerProtocol
	if (socket == NULL) 
	{
		cout<<"failed to create socket"<<endl;
		return false;
	}
	connected = true;
	return true;
}

/**
 * return server connection status
 */
bool TCPMessengerClient::isConnected(){
	return connected;
}

/**
 * disconnect from server
 */
void TCPMessengerClient::disconnect(){
	if (socket != NULL){
		if(sessionActive) 
			closeActiveSession();
		socket->cclose();
	}
}

/**
 * open new session with address (ip:port)
 * and close opened session
 */
bool TCPMessengerClient::open(string userName){
	if(sessionActive)//delete?
		closeActiveSession();//delete?
	cout<<"send play request to - "<<userName<<endl;
	sendCommand(OPEN_SESSION_WITH_PEER);
	sendCommandData(userName);
	cout<<"waiting for "<< userName << " respond"<<endl;
	int respond = readCommand();
	if (respond == START_SESSION_WITH_PEER)
	{
		cout<<userName << " accept your request"<<endl;
		secPlayerData = readCommandData();

		return true;
	}
	else if (respond == REFUSE)
	{
		cout<<userName << " decline your request"<<endl;

	}else
		cout<<"got unexpected command from server"<<endl;
	return false;

}

bool TCPMessengerClient::openRandom(){
	cout<<"send Random play request"<<endl;
	sendCommand(OPEN_RANDOM_SESSION);
	cout<<"waiting for respond"<<endl;
	int respond = readCommand();
	if (respond == START_SESSION_WITH_PEER)
	{
		cout<<"accept your request"<<endl;
		secPlayerData = readCommandData();

		return true;
	}
	else if (respond == REFUSE)
	{
		cout<<"server failed to find peer"<<endl;

	}else
		cout<<"got unexpected command from server"<<endl;
	return false;

}

void TCPMessengerClient::showAvailableUsers(){
	cout<<"available users:"<<endl;
	sendCommand(LIST_USERS);
	int respond = readCommand();
	if(respond == LIST_USERS)
	{
		string data = readCommandData();
		cout << data << endl;
	}
}
/**
 * return session status
 */
bool TCPMessengerClient::isActiveClientSession(){
	return sessionActive;
}

/**
 * close active session
 */
void TCPMessengerClient::closeActiveSession(){
	sendCommand(DISCONNECT);
	sessionActive = false;
//	sessionAddress = "";
//	return true;
}

/**
 * send message
 */
//bool TCPMessengerClient::send(string msg){
//	if(!sessionActive)
//		return false;
//	sendCommand(SEND_MSG_TO_PEER);
//	sendCommandData(msg);
//	return true;
//}
/**
 * send command
 */
void TCPMessengerClient::sendCommand(int command){
	command = htonl(command);
	socket->send((char*)&command,4);
}

/**
 * send command data
 * in the format: data length (4 byte int) and the data 
 */
void TCPMessengerClient::sendCommandData(string msg){
	int msgLen = msg.length();
	msgLen = htonl(msgLen);
	socket->send((char*)&msgLen,4);
	socket->send(msg.data(),msg.length());
}

/**
 * read incoming command
 */
int TCPMessengerClient::readCommand(){
	int command;
	socket->recv((char*)&command,4);
	command = ntohl(command);
	return command;
}

/**
 * read incoming command data
 */
string TCPMessengerClient::readCommandData(){
	string msg;
	char buff[1500];
	int msgLen;
	socket->recv((char*)&msgLen,4);
	msgLen = ntohl(msgLen);
	int totalrc = 0;
	int rc;
	while (totalrc < msgLen){
		rc = socket->recv((char*)&buff[totalrc],msgLen-totalrc);
		if (rc>0){
			totalrc += rc;
		}else{
			break;
		}
	}
	if (rc > 0 && totalrc == msgLen){
		buff[msgLen] = 0;
		msg = buff;
	}else{
		socket->cclose();
	}
	return msg;
}
