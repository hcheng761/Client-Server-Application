#pragma once
#include "stdafx.h"

class ioObj
{
	char* buf;
	int bytes;

public:
	ioObj(string s);
	~ioObj();

	char* getBuf() { return buf; }
	int getBytes() { return bytes; }
	wstring s2ws(const string& s);
	int readFile(string s);
};