#include "stdafx.h"
#include "Helper.h"
#include <unordered_set>
#include <string>
#include <algorithm>


using namespace std;

wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

int readFile(string s)
{
	wstring temp = s2ws(s);
	LPCWSTR l = temp.c_str();
	HANDLE hFile = CreateFile(l, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with %d\n", GetLastError());
		return 0;
	}

	// get file size
	LARGE_INTEGER li;
	BOOL bRet = GetFileSizeEx(hFile, &li);
	// process errors
	if (bRet == 0)
	{
		printf("GetFileSizeEx error %d\n", GetLastError());
		return 0;
	}

	// read file into a buffer
	int fileSize = (DWORD)li.QuadPart;			// assumes file size is below 2GB; otherwise, an __int64 is needed
	DWORD bytesRead;
	// allocate buffer
	char *fileBuf = new char[fileSize];
	// read into the buffer
	bRet = ReadFile(hFile, fileBuf, fileSize, &bytesRead, NULL);
	// process errors
	if (bRet == 0 || bytesRead != fileSize)
	{
		printf("ReadFile failed with %d\n", GetLastError());
		return NULL;
	}

	// done with the file
	CloseHandle(hFile);
	return bytesRead;
}

void function2()
{
		string file;// = "URL-input-100.txt";
		cout << "Enter a filename that contains URLs (include extension):" << endl;
		cin >> file;
		ifstream ifile(file);
		if (ifile)
		{
			int lineNum = 0;
			string line;
			/*while (getline(ifile, line))
				++lineNum;*/

			

			ioObj obj(file);
			//cout << "CHECKPOINT" << endl;
			//separately parse this buffer as well
			int bufSize = obj.getBytes();
			int marker = 0;
			for (int i = 0; i < bufSize; i++)
			{
				if (obj.getBuf()[i] == '\r' && obj.getBuf()[i+1] == '\n')
					lineNum++;
			}
			printf("Number of lines found in file: %d\n", lineNum + 1);

			char * bufMarker = new char[strlen(obj.getBuf())];
			strcpy(bufMarker, obj.getBuf());
			printf("Buffer size from reading file: %d\n\n", bufSize);

			int iteration = 0;
			int mark = 0;
			unordered_set<string> IPs;
			unordered_set <char*> hosts;	//elements are unique from each other
			while (strlen(bufMarker) != 0)
			{
				
				char* link, *locate, *marker;
				marker = strrchr(bufMarker + mark, '`');
				if (marker != NULL)
					mark = strlen(bufMarker) - strlen(marker);

				if (iteration < lineNum)
				{
					locate = strchr(bufMarker+mark, '\r\n');		//parse txt file for newlines.  Strips links as locate continues along the buffer
					link = new char[strlen(bufMarker) - strlen(locate)];
					strncpy(link, bufMarker, strlen(bufMarker) - strlen(locate));
					link[strlen(bufMarker) - strlen(locate)] = '\0';
				}
				else
				{
					link = new char[strlen(bufMarker)];
					strcpy(link, bufMarker);
					link[strlen(bufMarker)] = '\0';
				}
				url URL(link);
				URL.parse();
				cout << "Parsing URL . . . Host: " << URL.getHost() << " | Port: " << URL.getPortNum() << endl;

				int prevSize = hosts.size();

				hosts.insert(URL.getHost());
				cout << "Checking for host uniqueness in host . . . ";
				if (hosts.size() > prevSize)
				{	//unique item
					cout << "passed." << endl;

					Socket sock;
					cout << "Performing DNS lookup . . . ";		//modify this later to get DNS without connecting
					sock.findDNS(URL.getHost(), URL.getPortNum());
					if (sock.getDNS() == NULL)
					{
						cout << "Error on DNS Lookup." << endl;
					}
					else
					{
						cout << sock.getDNS() << endl;

						int IPprevSize = IPs.size();
						string DNSstr = sock.getDNS();
						IPs.insert(DNSstr);
						cout << "Checking for IP uniqueness . . . ";
						if (IPs.size() > IPprevSize)
						{
							cout << "passed" << endl;
							URL.HEADRequest();
							URL.GETRequest();
							sock.txtSocket(URL.getHost(), URL.getPortNum(), URL.getGET(), URL.getHEAD(), URL);	//send HEAD and GET request
						}
						else
						{
							cout << "failed." << endl;
							printf("\n--------------------------------------------------------------\n\n");
						}
					}
				}
				else
				{
					cout << "failed." << endl;
					printf("\n--------------------------------------------------------------\n\n");
				}
				if (iteration != lineNum)
				{
					for (int i = mark; i < strlen(bufMarker) - strlen(locate) + 2; i++)
					{
						bufMarker[i] = '`';		//try to find last character in URL with " ` " so program doesn't take a long time or freezes when going through the URLs
					}							//mainly for files with large amounts of lines
					++iteration;
				}
				else break;
			}
			cout << "\n\nProgram finished" << endl;
			return;
		}
		else
		{
			char exit;
			cout << "Invalid file.  Enter any key to quit:" << endl;
			cin >> exit;
			return;
		}
}