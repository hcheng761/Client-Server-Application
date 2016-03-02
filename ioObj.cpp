#include "ioObj.h"
#include "stdafx.h"

ioObj::ioObj(string s)
{
	readFile(s);
}

ioObj::~ioObj()
{
}

wstring ioObj::s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buffer = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buffer, len);
	std::wstring r(buffer);
	delete[] buffer;
	return r;
}

int ioObj::readFile(string s)
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

	buf = new char[fileSize];
	strcpy(buf, fileBuf);
	buf[strlen(buf)-4] = '\0';
	bytes = bytesRead;

	// done with the file
	delete fileBuf;
	CloseHandle(hFile);
	return bytesRead;
}
