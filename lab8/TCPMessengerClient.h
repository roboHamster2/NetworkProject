#include <strings.h>
#include <map>
#include <vector>
#include "MThread.h"
#include "TCPSocket.h"
#include "TCPMessengerProtocol.h"


using namespace std;

/**
 * The TCP Messenger client class
 */
class TCPMessengerClient{

	//bool runMainLoop;
	bool connected;
	bool sessionActive;
//		string sessionAddress;
	string firstPlayerUserName;
	string secPlayerData;


public:
	TCPSocket* socket;

	void showConnectMenu();
	void showLoginMenu();
	void showMenu();
	/**
	 * client main loop
	 */
	void handleServerCommand();
	/**
	 * constructor
	 */
	TCPMessengerClient();

	bool connectToServer();
	/**
	 * connect to server and start mainLoop 
	 */
	bool connect(string ip);

	void startGameWithPeer(bool startedByMe);
	/**
	 * login/register
	 */
	bool loginToServer();

	bool authenticate(int authCommand, string user, string password);
	/**
	 * return server connection status
	 */
	bool isConnected();

	/**
	 * disconnect from server
	 */
	void disconnect();

	/**
	 * open new session with address (ip:port)
	 * and close opened session
	 */
	bool open(string address);

	bool openRandom();

	void showAvailableUsers();
	/**
	 * return session status
	 */
	bool isActiveClientSession();

	/**
	 * close active session
	 */
	void closeActiveSession();

	/**
	 * send message
	 */
	bool send(string msg);


private:

	/**
	 * send command
	 */
	void sendCommand(int command);

	/**
	* send command data
	* in the format: data length (4 byte int) and the data 
	*/
	void sendCommandData(string msg);

	/**
	 * read incoming command
	 */
	int readCommand();

	/**
	 * read incoming command data
	 */
	string readCommandData();
};
