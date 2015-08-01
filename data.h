/**
@author Alexander Hillmer
*/
#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>
#include "speech.h"
#include <ctime>
using namespace std;

#define ALERTMODE_DISABLED 1
#define ALERTMODE_ENABLED 2
#define ALERTMODE_IENABLED 3

class Data {
public:

	Data(const char* configPath, const char* address, const char* port);
	~Data();

	void addLookup(string lookup);
	bool isLookup(string query);
	void setAlertMode(int mode) { alertMode = mode; writeConfig(configPath); };
	void setPlayers(vector<string> players);
	void selectSpeechOutput();
	void selectAlertMode();
	bool loadConfig(const char* filepath);
	void writeConfig(const char* filepath);
	
	

	const char* getAddress() { return address; };
	const char* getPort() { return port; };
	void switchLoginMode() { loginMode = !loginMode; };
	int getQueryTime() { return loginMode ? 5000 : queryTime; };
	int getPlayerCount() { return mPlayers.size(); };
	vector<string> getPlayers() { return vector<string>(mPlayers); };
	vector<string> getMessages() { return vector<string>(messages); };
	bool hasNewMessages() { return newMessages; };
	bool isRunning() { return running; };
	bool isWaiting() { return waiting; };
	void exitThread() { running = false; waiting = false; };
	void wait() { waiting = true; };
	void cont() { waiting = false; };
private:
	//data
	vector<string> mPlayers;
	vector<string> mLookups;
	vector<string> messages;

	//objects
	Speech* speech;
	
	//Config
	int queryTime;
	int countTime;
	int alertMode;
	
	
	//Input Vars
	const char* configPath;
	const char* address;
	const char* port;
	

	//States
	bool running;
	bool waiting;
	bool newMessages;
	bool loginMode;
	clock_t countClock;

	void alertPlayerChanges(vector<string> oldPlayers, vector<string> newPlayers);
	void alertServerCount();
};

#endif