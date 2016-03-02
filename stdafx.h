/*Howard Cheng
CSCE 463-500
Spring 2016
*/

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma comment (lib, "ws2_32.lib")
#include "targetver.h"

#define useragent "hcCrawler/1.0"

#include <stdio.h>
#include <tchar.h>
#include <string.h>
//#include <vld.h>
#include <winsock2.h>
#include <set>
#include <thread>
#include <time.h>
//#include <windows.h>

#include "HTMLParserBase.h"
#include "urlParser.h"
#include "socket.h"
#include <stdlib.h>
#include <string>
#include "Helper.h"
#include "ioObj.h"
#include <iostream>
#include <fstream>
// TODO: reference additional headers your program requires here
