/**
@author Alexander Hillmer
*/
#ifndef SERVER_QUERY_H
#define SERVER_QUERY_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <vector>

#pragma comment(lib,"ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;
#define TIMEOUT 5000
#define BUFLEN 4048

//Socket
SOCKET openSocket(const char* address,const char* port);
bool sendToSocket(SOCKET socket, char* data, int length);
void closeSocket(SOCKET socket);

//Steam
vector<string> parsePlayers(char* buf, int length, vector<string> &players);
bool query(const char* address, const char* port, vector<string> &players);
int recvFromSocket(SOCKET socket, char* buffer, int length);


#endif