/**
@author Alexander Hillmer
*/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <windows.h>
#include "speech.h"
#include "data.h"
#include "server_query.h"
#include <ctime>


using namespace std;
void printToConsole();
void print_hex(const char *s, int length);
void writeRed(string str);
void print_string(const char *s, int length);


DWORD WINAPI ServerQueryThread( LPVOID lpParam );



#define INPUTMSG  "Wrong number of arguments. configPath serverAddress port required"

#define SEL_QUIT 0
#define SEL_SELAUDIO 1
#define SEL_AUDIOMODE 2
#define SEL_LOGIN 3



Data* data;
int main(int argc, char** argv) {
	DWORD   dwThreadId;
	HANDLE  hThread;

	int i;
	
	if(argc < 4) {
		cout << INPUTMSG << endl;
		cin >> i;
		return 1;
	}


	data = new Data(argv[1], argv[2], argv[3]);

	hThread = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ServerQueryThread,       // thread function name
            NULL,          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadId);   // returns the thread identifier 


	while(data->isRunning()) {
		int i=-1;
		while(i < 0 || i > 3) {
			cin >> i;
			if(i < 0 || i > 3) {
				cout << "Invalid Input." << endl;
				cin.clear();
				cin.ignore(10000, '\n');
			}
		}
		switch(i) {
		case(SEL_QUIT):
			data->exitThread();
			break;
		case(SEL_SELAUDIO):
			data->wait();
			data->selectSpeechOutput();
			data->cont();
			break;
		case(SEL_AUDIOMODE):
			data->wait();
			data->selectAlertMode();
			data->cont();
			break;
		case(SEL_LOGIN):
			data->switchLoginMode();
			break;
		}
	}
	WaitForSingleObject(hThread, INFINITE);


	
	delete data;
	return 0;
}


DWORD WINAPI ServerQueryThread( LPVOID lpParam ) 
{ 
	//cout << "Starting requests at " << data->getAddress() << ":" << data->getPort() << endl;
	vector<string> players;
	while(data->isRunning()) {
		players.clear();
		if(query(data->getAddress(), data->getPort(), players)) {
			//cout << "Query... Sucess" << endl;
			data->setPlayers(players);
		}
		else {
			cout << "An error occured." << endl;
		}
		if(data->hasNewMessages()) {
			system("cls");
			printToConsole();
		}
		
		clock_t start = clock();
		clock_t end = clock();
		while(data->isRunning() && ((end-start)*1000)/CLOCKS_PER_SEC < data->getQueryTime()) {
			Sleep(500);
			end = clock();
		}

		while(data->isWaiting()) {
			Sleep(500);
		}
	}

	return 0;
}

void printToConsole() {
	vector<string> players = data->getPlayers();
	cout << "Currently are " << data->getPlayerCount() << " players on " << data->getAddress() << "." << endl;
	int n=0;

	cout << left << setw(25);
	for (auto player = players.begin(); player != players.end(); player++ ) {
		n++;
		if(data->isLookup(*player)) writeRed(*player);
		else cout << *player;
		cout << setw(25);
		if(n%3 == 0) {
			cout << endl;
			cout << left << setw(25);
		}
	}
	cout << endl << endl;
	vector<string> messages = data->getMessages();
	for(auto message = messages.begin(); message != messages.end(); message++) {
		cout << *message << endl;
	}

	cout << endl;

	cout << "Enter a number" << endl << "quit(0), selectAudio(1), selectAudioMode(2), loginMode(3)" << endl;
}

void writeRed(string str) {
	HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hstdout, &csbi );

	SetConsoleTextAttribute( hstdout, 0x0C );

	cout << str;

	SetConsoleTextAttribute( hstdout, csbi.wAttributes );
}
void print_hex(const char *s, int length)
{
  for(int i=0; i<length; i++) {
    printf("%02x ", (unsigned int) *s++);
  }
  //printf("%02x ", (unsigned int) *s++);
  printf("\n");
}

void print_string(const char *s, int length)
{
  for(int i=0; i<length; i++) {
    printf("%c", (unsigned int) *s++);
  }
  printf("\n");
}


