/**
@author Alexander Hillmer
*/
#include "data.h"
#include <iostream>
#include <sstream>

#include <algorithm>

#include "config.h"
using namespace std;

Data::Data(const char* configPath, const char* address, const char* port) {
	queryTime = 60000;
	countTime = 180000;
	countClock = -countTime*2;
	alertMode = ALERTMODE_ENABLED;
	running = true;
	waiting = false;
	newMessages = false;
	loginMode = false;
	this->address = address;
	this->port = port;
	this->configPath = configPath;
	speech = new Speech();

	if(!loadConfig(configPath)) {
		speech->selectOutputDevice();
		mLookups.push_back("ExamplePlayer1");
		mLookups.push_back("ExamplePlayer2");
		writeConfig(configPath);
	}
}

Data::~Data() {
	delete speech;
}


bool Data::loadConfig(const char* filepath) {
	map<string,string> map = Config::readConfig(filepath);
	if(map.size() == 0) return false;
	for(auto entry = map.begin(); entry != map.end() ; entry++) {
		if(strcmp("queryTime", entry->first.c_str()) == 0) {
			queryTime = stoi(entry->second);
		} else if(strcmp("countTime", entry->first.c_str()) == 0) {
			countTime = stoi(entry->second);
		} else if(strcmp("alertMode", entry->first.c_str()) == 0) {
			alertMode = stoi(entry->second);
		} else if(strcmp("speechOutput", entry->first.c_str()) == 0) {
			speech->setOutputDevice(entry->second.c_str());
		} else if(strcmp("players", entry->first.c_str()) == 0) {
			mLookups = split(entry->second, ',');
		}	
	}
	return true;
}
void Data::writeConfig(const char* filepath) {
	map<string,string> map;
	map.insert(pair<string,string>("queryTime",to_string((long long)queryTime)));
	map.insert(pair<string,string>("countTime",to_string((long long)countTime)));
	map.insert(pair<string,string>("alertMode",to_string((long long)alertMode)));

	size_t cSize = wcslen(speech->getOutputDevice())+1;
	char* str = new char[cSize];
	size_t ret;
	wcstombs_s(&ret, str, cSize, speech->getOutputDevice(), cSize);

	map.insert(pair<string,string>("speechOutput",str));
	
	stringstream sstr;
	if(mLookups.size() > 0) {
		for(unsigned int i=0; i<mLookups.size()-1; i++) {
			sstr << mLookups.at(i) << ",";
		}
		sstr << mLookups.at(mLookups.size()-1);
	}

	map.insert(pair<string,string>("players",sstr.str()));

	Config::writeConfig(filepath, map);
}

void Data::selectSpeechOutput() {
	speech->selectOutputDevice();
	writeConfig(configPath);
}

void Data::selectAlertMode() {
	int i=0;
	while(i < 1 || i > 3) {
		cout << "Select the audio output level:" << endl;
		cout << "(" << ALERTMODE_DISABLED << ") Disable Audio" << endl;
		cout << "(" << ALERTMODE_ENABLED << ") Enable Audio" << endl;
		cout << "(" << ALERTMODE_IENABLED << ") Enable important Audio" << endl;
	
		cin >> i;
		if(i < 1 || i > 3) {
			cout << "Invalid Input." << endl;
			cin.clear();
			cin.ignore(10000, '\n');
		}
	}
	alertMode = i;
}
void Data::addLookup(string lookup) {
	this->mLookups.push_back(lookup);
	
	cout << "Looking for player: " << lookup << endl;
}

void Data::setPlayers(vector<string> players) {
	messages.clear();
	alertPlayerChanges(mPlayers, players);
	
	mPlayers.clear();
	mPlayers.insert(mPlayers.end(), players.begin(), players.end());

	alertServerCount();
}


void Data::alertPlayerChanges(vector<string> oldPlayers, vector<string> newPlayers) {
	

	//Check for new Players
	for(unsigned int i=0; i<newPlayers.size();i++) {
		if(find(oldPlayers.begin(), oldPlayers.end(), newPlayers.at(i)) != oldPlayers.end()) {
		} else {
			if(find(mLookups.begin(), mLookups.end(), newPlayers.at(i)) != mLookups.end()) {
				stringstream sstr;
				sstr <<  "Important player joined " << newPlayers.at(i);
				messages.push_back(sstr.str());
				
				if(alertMode == ALERTMODE_IENABLED && oldPlayers.size() != 0) {
					speech->speechOutput(sstr.str().c_str());
				}
			} else {
				stringstream sstr;
				sstr <<  "Player joined " << newPlayers.at(i);
				messages.push_back(sstr.str());
				if(alertMode == ALERTMODE_ENABLED && oldPlayers.size() != 0) {
					speech->speechOutput(sstr.str().c_str());
				}
			}
			
			
		}
	}

	//Check for leaving players
	for(unsigned int i=0; i<oldPlayers.size();i++) {
		if(find(newPlayers.begin(), newPlayers.end(), oldPlayers.at(i)) != newPlayers.end()) {
		} else {
			if(find(mLookups.begin(), mLookups.end(), oldPlayers.at(i)) != mLookups.end()) {
				stringstream sstr;
				sstr <<  "Important player left " << oldPlayers.at(i);
				messages.push_back(sstr.str());
				if(alertMode == ALERTMODE_IENABLED) {
					speech->speechOutput(sstr.str().c_str());
				}
			} else {
				stringstream sstr;
				sstr <<  "Player left " << oldPlayers.at(i);
				messages.push_back(sstr.str());
				if(alertMode == ALERTMODE_ENABLED) {
					speech->speechOutput(sstr.str().c_str());
				}
			}
		}
	}
	if(messages.size() > 0) newMessages = true;
}

void Data::alertServerCount() {
	clock_t current = clock();
	
	long ellapsed = ((current-countClock)*1000)/CLOCKS_PER_SEC;
	if(ellapsed > countTime) {
		countClock = current;
		stringstream sstr;
		sstr << getPlayerCount() << " players online.";
		messages.push_back(sstr.str());
		if(alertMode != ALERTMODE_DISABLED)
			speech->speechOutput(sstr.str().c_str());
	}
}


	