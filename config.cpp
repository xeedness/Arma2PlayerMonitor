/**
@author Alexander Hillmer
*/
#include "config.h"
#include <iostream>
#include <fstream>


std::vector<std::string> &split(const string &s, char delim, vector<std::string> &elems) {
		stringstream sstr(s);
		string item;
		while (getline(sstr, item, delim)) {
			elems.push_back(item);
		}
		return elems;
};


std::vector<std::string> split(const string &s, char delim) {
	vector<std::string> elems;
	split(s, delim, elems);
	return elems;
};

map<string, string> Config::readConfig(const char* filepath) {
	string line;
	ifstream file (filepath);
	map<string,string> map;
	if (file.is_open()) {
		while ( getline (file,line) ) {
			vector<string> spl = split(line, ':');
			if(spl.size() > 1) 
				map.insert(pair<string,string>(spl.at(0), spl.at(1)));
			else if(spl.size() == 1) {
				map.insert(pair<string,string>(spl.at(0), string("")));
			}
		}
		file.close();
	}
	//else cout << "Unable to open file"; 

	return map;
}
void Config::writeConfig(const char* filepath, map<string,string> map) {
	ofstream file (filepath);
	if (file.is_open()) {
		for(auto entry = map.begin(); entry != map.end() ; entry++) {
			file << entry->first << ":" << entry->second << endl;		
		}
		file.close();
	} else {
		cout << "An error occured on writing to file " << filepath << endl;
	}
}


