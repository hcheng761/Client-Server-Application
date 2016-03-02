/*Howard Cheng
CSCE 463-600
Spring 2016
*/

#pragma once
#include "winsock2.h"
#define INITIAL_BUF_SIZE 8000
using namespace std;

class Socket {
	SOCKET sock; // socket handle
	char *buf; // current buffer
	char * header;
	int allocatedSize; // bytes allocated for buf
	int curPos; // current position in buffer
	 // extra stuff as needed
	int bufSize;
	char* DNS;

public:
	Socket();
	~Socket();
	bool Read(void);
	bool checkBot(char * buffer, int header);
	//void servSocket();
	void initSocket(char* host, int port, char* req, url URL);
	void txtSocket(char * host, int port, char * GETreq, char * HEADreq, url URL);
	void findDNS( char* host, int port);
	int sendRequest(char *request);
	char* getBuffer() { return buf; };
	char* getDNS() { return DNS; };
};