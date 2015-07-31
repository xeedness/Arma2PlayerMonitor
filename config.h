/**
@author Alexander Hillmer
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


std::vector<std::string> &split(const string &s, char delim, vector<std::string> &elems);


std::vector<std::string> split(const string &s, char delim);
class Config {
public:
	static map<string, string> readConfig(const char* filepath);
	static void writeConfig(const char* filepath, map<string,string> map);
};



#endif