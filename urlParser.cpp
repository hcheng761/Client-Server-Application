/*Howard Cheng
CSCE 463-600
Spring 2016
*/

#include "urlParser.h"
#include "stdafx.h"

url::url(char* u)
{
	URL = u;
	//setLength();
}

url::~url()
{
	/*delete frag;
	delete request;
	delete scheme;
	delete host;
	delete port;
	delete path;
	delete query;
	delete GET;
	delete HEAD;*/
	//free(host);
	//delete URL;
}

void url::parse()
{
		// [SCHEME]
		char* schemePos = strstr(URL, "://");	//point to ://
		char* URLparse = new char[strlen(URL)]; //no scheme
	
		if (schemePos != NULL)
		{
			strcpy(URLparse, schemePos + 3);

			//locate # [FRAGMENT]
			char *locate = strchr(URLparse, '#');

			if (locate != NULL)
			{
				locate++;
				frag = new char[strlen(locate)];
				int fragStripped = strlen(URLparse) - strlen(locate);
				strncpy(frag, locate, strlen(locate));
				frag[strlen(locate)] = '\0';
				URLparse[fragStripped] = '\0';
			}
			else frag = NULL;

			//locate / [REQUEST]
			locate = strchr(URLparse, '/');		//make substring for request, then separate query
			if (locate != NULL)
			{
				request = new char[strlen(locate)];
				int reqStripped = strlen(URLparse) - strlen(locate);
				strncpy(request, locate, strlen(locate));
				request[strlen(locate)] = '\0';

				// PATH AND QUERY
				locate = strchr(request, '?');
				if (locate != NULL)
				{
					locate++;
					query = new char[strlen(request)];
					int queryStripped = strlen(request) - strlen(locate);
					strncpy(query, locate, strlen(locate));
					query[queryStripped] = '\0';

					path = new char[strlen(request)];
					strcpy(path, request);
					path[strlen(locate)] = '\0';
				}
				else
				{
					query = NULL;
					path = new char[strlen(request)];
					strcpy(path, request);
					path[strlen(request)] = '\0';
				}
				URLparse[reqStripped] = '\0';
			}
			else
			{
				request = "/\0";
				path = NULL;
				query = NULL;
			}

			//locate : [PORT]
			locate = strchr(URLparse, ':');
			if (locate != NULL)
			{
				locate++;
				port = new char[strlen(locate)];
				int portStripped = strlen(URLparse) - strlen(locate);
				strncpy(port, locate, strlen(locate));
				port[strlen(locate)] = '\0';
				URLparse[portStripped - 1] = '\0';

				sscanf(port, "%d", &portNum);
			}
			else
			{
				port = "80\0";
				portNum = 80;
			}
			// HOST is rest of URL
			if (URLparse == NULL)
			{
				return;
			}
			else
				host = URLparse;
		}
}

void url::changeURL(char * arr)
{
	URL = arr;
}

int url::getLength()
{
	return (int) strlen(URL);
}

void url::GETRequest()
{
	if (host == NULL)
	{
		GET = NULL;
		printf("GET Request not created due to missing host.");
		return;
	}
	else
	{
		char format[] = "GET %s HTTP/1.0\r\nHost: %s\r\nUser-agent: %s\r\nConnection: close\r\n\r\n";
		int size = strlen(host) + strlen(request) + strlen(useragent) + strlen(format) - 6;
		GET = new char[size+1];
		sprintf(GET, format, request, host, useragent);
		/*
		strcpy(GET, format);
		strcat(GET, request);
		strcat(GET, host);*/
	}
}

void url::HEADRequest()
{
	if (host == NULL)
	{
		HEAD = NULL;
		printf("HEAD request not created.");
		return;
	}
	else
	{
		char format [] = "HEAD /robots.txt HTTP/1.0\r\nHost: %s\r\nUser-agent: %s\r\nConnection: close\r\n\r\n";
		int size = strlen(host) + strlen(request) + strlen(useragent) + strlen(format) - 6;
		HEAD = new char[size + 1];
		sprintf(HEAD, format, host, useragent);
	}
}
