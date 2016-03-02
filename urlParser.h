/*Howard Cheng
CSCE 463-600
Spring 2016
*/

#include "stdafx.h"

class url	{
private:
	char* URL;
	char* scheme,* host,* port, *path, *query, *request, *frag;
	int portNum;
	char* GET;
	char * HEAD;
	int length;
	void* parser;
	void* buffer;
	bool dup;

public:
	url(char* u);
	~url();
	void parse();
	void changeURL(char* arr);
	char* getURL() { return URL; };
	char* getScheme() {return scheme;};
	char* getHost() { return host; };
	char* getPort() { return port; };
	char* getPath() { return path; };
	char* getQuery() { return query; };
	char* getRequest() { return request; };
	char* getFragment() { return frag; };
	char* getGET() { return GET; };
	char* getHEAD() { return HEAD; };
	int getPortNum() { return portNum; };
	int getLength();
	bool getDup() { return dup; };
	void GETRequest();
	void HEADRequest();

};

