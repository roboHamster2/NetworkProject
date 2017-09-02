#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */
#define MSNGR_PORT 3346

#define CLOSE_SESSION_WITH_PEER 	1
#define OPEN_SESSION_WITH_PEER 		2
#define EXIT						3
#define SEND_REQUEST_TO_PEER		4
#define SESSION_REFUSED				5
#define SESSION_ESTABLISHED			6
#define LOGIN 						7
#define REGISTER					8
#define ACCEPT 						9
#define REFUSE 						10
#define LIST_USERS 					11
#define OPEN_RANDOM_SESSION 		12
#define LOGIN_ACCEPT        		13
#define LOGIN_REFUSED        		14
#define START_SESSION_WITH_PEER		15
#define DISCONNECT  				16
#define DISCONNECT_FROM_SERVER  	17
#define SESSION_ABOUT_TO_CLOSE		18
#define UPDATE_SCORE				19

#define TEST_PEER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer refused, peer might be busy or disconnected, try again later"



#endif /* TCPMESSENGERPROTOCOL_H_ */
