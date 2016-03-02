// Homework 1.cpp : Defines the entry point for the console application.
/*Howard Cheng
CSCE 463-600
Spring 2016
*/

#include "stdafx.h"
#include <unordered_set>
using namespace std;



int main()
{
	clock_t start = clock(), diff;

	//parse
	char input[MAX_REQUEST_LEN];
	int function;
	
	cout << "Enter 1 or 2 to choose a functionality for the program's respective part:" << endl;
	cin >> function;

	if (function == 1)
	{
		printf("Enter the URL to be loaded:\r\n");
		scanf_s("%s", input, MAX_REQUEST_LEN);
		url URL(input);

		URL.parse();
		diff = clock() - start;

		int msec = diff * 1000 / CLOCKS_PER_SEC;
		printf("URL successfully parsed.  Done in %d milliseconds", msec % 1000);

		URL.GETRequest();
		if (URL.getGET() == NULL) return 0;

		printf("\n");
		printf("Host: %s\r\n", URL.getHost());
		printf("Port: %s\r\n", URL.getPort());
		printf("Request: %s\r\n", URL.getRequest());
		printf("Path: %s\r\n", URL.getPath());
		printf("Query: %s\r\n", URL.getQuery());
		printf("Fragment: %s\r\n", URL.getFragment());
		printf("\n");
		printf("GETRequest: %s\r\n", URL.getGET());

		//connect
		Socket sock;
		sock.initSocket(URL.getHost(), URL.getPortNum(), URL.getGET(), URL);
		cout << "\n\nProgram finished.  Press any key to exit." << endl;
		char exit;
		cin >> exit;
		return 0;
	}
	else if (function == 2)
	{
		int t;
		printf("Enter the number threads to process:\n");
		cin >> t;
		if (t != 1)
		{
			char exit;
			cout << "Number of threads exceeds limit.  Press any key to exit the program." << endl;
			cin >> exit;
			return 0;
		}
		else
		{
			thread func2(function2);
			func2.join();
			cout << "Thread is finished." << endl;
		}
		return 0;
	}
	else {
		cout << "Invalid input.  Press any key to exit the program:" << endl;
		char exit;
		cin >> exit;
		return 0;
	}
}

