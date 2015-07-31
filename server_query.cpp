/**
@author Alexander Hillmer
*/
#include "server_query.h"

#include <iostream>

vector<string> parsePlayers(char* buf, int length, vector<string> &players) {
	char namebuffer[256];
	int n = (int)(buf[5]);
	//skip header
	buf += 6;
	//cout << "Parsing players...";
	for(int i=0; i<n;i++) {
		//skip rank
		buf += 1;
		//Get name
		memset(namebuffer, 0, 256);
		memcpy_s(namebuffer, 256, buf, strlen(buf)+1);
		string name(namebuffer);
		players.push_back(name);
		//Skip name
		buf += strlen(buf)+1;
		//Skip score and sth.
		buf += 8;
		
	}

	return players;
}
bool query(const char* address, const char* port, vector<string> &players) {
	char recvbuf[BUFLEN];
    int recvbuflen = BUFLEN;
	
	
	char *command = "TSource Engine Query";
	
	char* infobuf = new char[4+strlen(command)+1];
	memset(infobuf, 255, 4);
	memcpy_s(infobuf+4,strlen(command)+1, command, strlen(command)+1);
	
	char* challengeReqBuf = new char[4+1+4];
	challengeReqBuf[4] = 0x55;
	memset(challengeReqBuf, 255, 4);
	memset(challengeReqBuf+5, 255, 4);


    // Send an initial buffer
	SOCKET socket = openSocket(address, port);
	if(socket == INVALID_SOCKET) {
		cout << "Couldn't open socket" << endl;
		return false;
	}
	//INFO
	//if(sendToSocket(socket, infobuf, strlen(infobuf)+1)) {
	//	int bytes = recvFromSocket(socket, recvbuf, recvbuflen);
	//	std::cout << "Received " << bytes << " bytes." << std::endl;
	//	return true;
	//} else {
	//	return false;
	//}

	//PLAYERS
	if(sendToSocket(socket, challengeReqBuf, strlen(challengeReqBuf))) {
		int bytes = recvFromSocket(socket, recvbuf, recvbuflen);
		if(bytes > 0) {
			char newSendBuf[9];
			memset(newSendBuf, 255, 4);
			newSendBuf[4] = 'U';
			memcpy_s(newSendBuf+5, 9, recvbuf+5, 4); 
			if(sendToSocket(socket, newSendBuf, 9)) {
				int bytes = recvFromSocket(socket, recvbuf, recvbuflen);
				if(bytes > 0) {
					parsePlayers(recvbuf, bytes, players);
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else {
			return false;
		}

	} else {
		return false;
	}

	delete infobuf;
	delete challengeReqBuf;
	closeSocket(socket);
	return true;
}


SOCKET openSocket(const char* address,const char* port) {
	WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    
    int iResult;
    

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return ConnectSocket;
    }

    ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // Resolve the server address and port
    iResult = getaddrinfo(address, port, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return ConnectSocket;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return ConnectSocket;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
			printf("socket failed with error: %ld\n", WSAGetLastError());
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return ConnectSocket;
    }

	return ConnectSocket;
}

bool sendToSocket(SOCKET socket, char* data, int length) {
    int iResult = send( socket, data, length, 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        WSACleanup();
        return false;
    }

    //printf("Bytes Sent: %ld\n", iResult);
	//print_hex(sendbuf);
    // shutdown the connection since no more data will be sent
    //iResult = shutdown(socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        WSACleanup();
        return false;
    }

	//cout << iResult << " " << length << endl;

	return true;
}

int recvFromSocket(SOCKET socket, char* buffer, int length) {
	int to = TIMEOUT;
	bool res = true;
	setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&to,sizeof(int));
	int iResult = 0;
    do {
		//cout << "Receiving..." << endl;
        iResult = recv(socket, buffer, length, 0);
       // cout << "Done" << endl;
		if ( iResult > 0 ) {
			res = true;
            //printf("Bytes received: %d\n", iResult);
			//print_hex(buffer, iResult);
			//print_string(buffer,iResult);
			break;
		}
        else if ( iResult == 0 ) {
            printf("Connection closed\n");
		}
        else {
			res = false;
            printf("recv failed with error: %d\n", WSAGetLastError());
			break;
		}

    } while( iResult > 0 );

    // cleanup
    

    return iResult;
}


void closeSocket(SOCKET socket) {
	shutdown(socket, SD_SEND);
	closesocket(socket);
    WSACleanup();
}