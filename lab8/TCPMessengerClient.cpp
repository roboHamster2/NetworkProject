#include "TCPMessengerClient.h"
#include "TCPMessengerProtocol.h"
#include "TCPSocket.h"
#include "UDPGame.h"
#include "stdlib.h"
#include "iostream"
#include "sstream"
#include "string"

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ")
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
	cout<<"su - show score and position"<<endl;
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
					secPlayerData = readCommandData();
					startGameWithPeer(false);
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

void TCPMessengerClient::startGameWithPeer(bool startedByMe){//change to UDPSOCKET
	cout<<"GameStarted"<<endl;
	vector<string> tokens;
	Tokenize(secPlayerData, tokens, ":");
	string destIP = tokens[0];
	int destPort = atoi(tokens[1].c_str());
	int myPort = atoi(tokens[2].c_str());
	UDPGame* game = new UDPGame(destIP,destPort, myPort);
	int score=game->startGame();
	cout << "the score is : " << score << endl;
	//Updating the server
	string resultToServer = to_string(score);
	sendCommand(CLOSE_SESSION_WITH_PEER);
	sendCommandData("400");
	cout<<"ffff "<<endl;
//	closeActiveSession();
	//close the game in server

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
				cout<< "register successfully"<<endl;
			else
				cout<<"failed to register"<<endl;
		} else if(command == "l")
		{
			string user;
			string password;
			cin >> user;
			cin >> password;
			if(authenticate(LOGIN,user, password)){
				firstPlayerUserName = user;
				return true;
			}else
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
	if (socket->isConnected() == false)
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
//	if(sessionActive)//delete?
//		closeActiveSession();//delete?
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
		string error = readCommandData();
		cout<< error <<endl;

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

void TCPMessengerClient::showPosiotionAndScore(){
	cout<<"Scores:"<<endl;
	sendCommand(SHOW_SCORE);
	int respond = readCommand();
	if(respond == SHOW_SCORE)
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
