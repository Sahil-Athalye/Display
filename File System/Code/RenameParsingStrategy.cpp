#include "RenameParsingStrategy.h"
#include "AbstractFile.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

vector<string> RenameParsingStrategy::parse(string args) {
	// parse our args into a preliminary vector
	stringstream ss(args); // wrap string into stream
	string temp;
	vector<string> strings;

	// extract while possible, essentially splits string by spaces and puts new strings in vector
	while (ss >> temp) {
		strings.push_back(temp);
	}

	if (strings.size() != expected_size) {
		vector<string> def;
		return def;
	}

	vector<string> parsed;
	parsed.push_back(strings[arg_vals::filename] + " " + strings[arg_vals::second_input]); // add for copy
	parsed.push_back(strings[arg_vals::filename]); // add for remove

	return parsed;
}