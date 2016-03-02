/*Howard Cheng
CSCE 463-600
Spring 2016
*/
#include "socket.h"
#include "stdafx.h"

class Parameters {
public:
	HANDLE mutex;
	HANDLE finished;
	HANDLE eventQuit;
};

Socket::Socket()
{
	//printf("Called");
	// allocate this buffer once, then possibly reuse for multiple connections in Part 3
	buf = new char[INITIAL_BUF_SIZE]; // start with 8 KB
	bufSize = 0;
	header = new char[INITIAL_BUF_SIZE];
	//printf("Finished");
}

Socket::~Socket()
{
	delete buf;
	delete header;
}

bool Socket::Read(void)
{
	int fd;
	fd_set readfds;
	struct timeval timeout;
	int ret, sret;

	clock_t start = clock(), diff;

	fd = 0;

	char *oldData;
	curPos = 0;
	allocatedSize = INITIAL_BUF_SIZE;
	// set timeout to 10 seconds
	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);

		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		diff = clock() - start;
		int msec = diff * 1000 / CLOCKS_PER_SEC;
		int sec = diff / CLOCKS_PER_SEC;
		// wait to see if socket has any data (see MSDN)
		sret = select(0, &readfds, NULL, NULL, &timeout);
		if (sret > 0)
		{
			// new data available; now read the next segment
			int bytes = recv(sock, buf + curPos, allocatedSize - curPos - 1, 0);
			bufSize += bytes;
			if (allocatedSize > 20000000 || sec > 10)
			{
				printf("Download too large or taking too long.  Download stopped.");
				break;
			}
			if (bytes == SOCKET_ERROR)
			{
				printf("Error while reading request segment: %s", WSAGetLastError());
				break;
			}
			if (bytes == 0)			// normal completion
			{ 
				//printf("%d\n", curPos);
				printf("Response received in %d milliseconds\n", msec % 1000);
				buf[curPos] = '\0';
				
				char* emptyLine = strstr(buf, "\r\n\r\n");
				for (int i = 0; i < strlen(buf) - strlen(emptyLine); i++)
				{
					header[i] = buf[i];
				}
				header[strlen(buf) - strlen(emptyLine)] = '\n';
				header[strlen(buf) - strlen(emptyLine) + 1] = '\0';
				printf("\n%s",header);
				return true; 
			}
			curPos += bytes; // adjust where the next recv goes
			if ((allocatedSize - curPos) < 8000)
				// double the buffer, copy old chunk there;
			{
				buf = (char*) realloc(buf, 2 * allocatedSize);
				allocatedSize *= 2;
			}
				// you can also use HeapReAlloc() or realloc()
		}
		else if (sret == 0)
		{
			// report timeout
			printf("Timeout while reading: %s\n", WSAGetLastError);
			break;
		}
		else
		{
			//print WSAGetLastError()
			printf("Receive operation could not completed.");
			break;
		}
	}
	//printf("%d\n", curPos);
	//printf("%s", buf);
	return false;
}

bool Socket::checkBot(char * buffer, int header)
{
	int fd;
	fd_set readfds;
	struct timeval timeout;
	int ret, sret;
	int counter = 0;
	clock_t start = clock(), diff;

	fd = 0;
	char *oldData;
	curPos = 0;
	// set timeout to 10 seconds
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	while (curPos < header)	// wait to see if socket has any data (see MSDN)
	{
		sret = select(0, &readfds, NULL, NULL, &timeout);
		if (sret > 0)
		{
			// new data available; now read the next segment
			int bytes = recv(sock, buffer + curPos, header, 0);
			curPos += bytes;
			if (bytes == SOCKET_ERROR)
			{
				printf("Error while reading request segment: %s", WSAGetLastError());
				return false;
			}
			if (curPos > 16000)
			{
				return false;
			}
		}
	}
	return true;
}


void Socket::initSocket(char * host, int port, char* req, url URL)
{
	WSADATA wsaData;

	// Initialize WinSock; once per program run
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		// WSA failure
		WSACleanup();
		return;
	}

	// open a TCP socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		printf("socket() generated error %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// structure used in DNS lookups
	struct hostent *remote;
	//printf("You're here now.");

	// structure for connecting to server
	struct sockaddr_in server;

	DWORD IP = inet_addr(host);
	if (IP == INADDR_NONE)
	{
		// DNS lookup
		remote = gethostbyname(host);	//IP address of local machine
		if (remote == NULL)
		{
			printf("Invalid string: neither FQDN, nor IP address\n");
			return;
		}
		else
		{
			memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
		}
	}
	else
		server.sin_addr.S_un.S_addr = IP;

	// structure for connecting to server
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	printf("Connecting to server...\n");

	clock_t start = clock(), diff;
	// Connect socket to server
	if(connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Connection error: %d\n", WSAGetLastError());
		return;
	}

	diff = clock() - start;
	int msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("Successfully connected to %s (%s) on port %d... Done in %d milliseconds.\n", host, inet_ntoa(server.sin_addr), htons(server.sin_port), msec % 1000);

	// send HTTP requests here
	sendRequest(req);
	
	Read();
	// read into buffer

	// close the socket to this server; open again for the next one
	closesocket(sock);

	char* httpResp = strstr(buf, "HTTP/");
	if (httpResp == NULL)
		printf("HTTP response not found.  No parsing done.\n");

	else if (*(buf + 9) != '2')
	{
		printf("%s\n", header);
		printf("HTML not parsed due to invalid reply.\n");
	}
	else {
		int nLinks;
		HTMLParserBase *parser = new HTMLParserBase;
		char* linkBuffer = parser->Parse(buf, bufSize, URL.getURL(), URL.getLength(), &nLinks);

		printf("Found %d links:\n", nLinks);

		for (int i = 0; i < nLinks; i++)
		{
			printf("%s\n", linkBuffer);
			linkBuffer += strlen(linkBuffer) + 1;
		}

		delete parser;
	}
	// call cleanup when done with everything and ready to exit program
	WSACleanup();
}

void Socket::txtSocket(char * host, int port, char * GETreq, char * HEADreq, url URL)
{
	WSADATA wsaData;

	// Initialize WinSock; once per program run
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		// WSA failure
		WSACleanup();
		return;
	}

	// open a TCP socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		printf("socket() generated error %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// structure used in DNS lookups
	struct hostent *remote;
	//printf("You're here now.");

	// structure for connecting to server
	struct sockaddr_in server;

	DWORD IP = inet_addr(host);
	if (IP == INADDR_NONE)
	{
		// DNS lookup
		remote = gethostbyname(host);	//IP address of local machine
		if (remote == NULL)
		{
			printf("Invalid string: neither FQDN, nor IP address\n");
			return;
		}
		else
		{
			memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
		}
	}
	else
		server.sin_addr.S_un.S_addr = IP;

	clock_t start = clock(), diff;

	// structure for connecting to server
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	printf("Connecting on robots . . ."); 
	// Connect socket to server
	if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Failed to connect on robots: %d\n", WSAGetLastError());
		return;
	}
	diff = clock() - start;
	int msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("done in %d milliseconds\n", msec % 1000);
	//printf("Successfully connected to %s (%s) on port %d... Done in %d milliseconds.\n", host, inet_ntoa(server.sin_addr), htons(server.sin_port), msec % 1000);

	
	
	// send HTTP requests here
	if (sendRequest(HEADreq) < 0)
	{
		printf("HEAD request not sent.");
		closesocket(sock);
		return;
	}
	char* botBuffer = new char[20];
	printf("RECV on robots . . . ");
	if (checkBot(botBuffer, 20) == false)
	{
		printf("not received.");
	}
		
	botBuffer[strlen(botBuffer)] = '\0';
	printf("done in %d milliseconds.\n", msec % 1000);
	if (strchr(botBuffer, '4') == NULL)
	{
		printf("Robots found (Non-4xx status code).  No parsing done\n");	
		printf("\n--------------------------------------------------------------\n\n");
	}
	else
	{
		sendRequest(GETreq);
		printf("Connecting on page . . .");
		if (Read() == true)
		{
			printf("done in %d milliseconds\n", msec % 1000);
			// read into buffer
			;

			char* httpResp = strstr(buf, "HTTP/");
			if (httpResp == NULL)
			{
				printf("HTTP response not found.  No parsing done.\n");
				printf("\n--------------------------------------------------------------\n\n");
			}
			else if (*(buf + 9) != '2')
			{
				//printf("%s\n", header);
				printf("HTML not parsed due to invalid reply.\n");
				printf("\n--------------------------------------------------------------\n\n");
			}
			else {
					int nLinks;
					HTMLParserBase *parser = new HTMLParserBase;
					char* linkBuffer = parser->Parse(buf, bufSize, URL.getURL(), URL.getLength(), &nLinks);

					printf("Found %d links:\n\n", nLinks);

					/*	for (int i = 0; i < nLinks; i++)
						{
							printf("%s\n", linkBuffer);
							linkBuffer += strlen(linkBuffer) + 1;
						}*/

					delete parser;
				}
		}
		else
		{
			printf("failed to connect.\n");
		}
	}
	//delete botBuffer;
	// close the socket to this server; open again for the next one
	closesocket(sock);

	// call cleanup when done with everything and ready to exit program
	WSACleanup();
}

void Socket::findDNS( char * host, int port)
{
	
	struct hostent *remoteHost;
	struct in_addr addr;
	WSADATA wsaData;
	int win = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (win != 0)
	{
		WSACleanup();
		return;
	}

	remoteHost = gethostbyname(host);
	if (remoteHost == NULL)
	{
		WSACleanup();
		return;
	}

	clock_t start = clock(), diff;
	diff = clock() - start;
	int msec = diff * 1000 / CLOCKS_PER_SEC;

	addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
	DNS = inet_ntoa(addr);

	cout << DNS << " done in " << msec % 1000 << " milliseconds." << endl;
	// call cleanup when done with everything and ready to exit program
	WSACleanup();
}

int Socket::sendRequest(char * request)
{
	//printf("%s",request);
	if (send(sock, request, strlen(request), 0) == SOCKET_ERROR)
	{
		//printf("Request not sent: %d\n.", WSAGetLastError());
		return -1;
	}
	else
	{
		//printf("Request successfully sent.\n");
		return 0;
	}
}