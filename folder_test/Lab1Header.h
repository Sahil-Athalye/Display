#pragma once
#include <string>
#include <vector>
using namespace std;

enum basics {
		programname,
		inputfile,
		numarguments,
	};
	
enum cases {good, cannotopen, emptyfile, wrongnum};

int parse(vector<string>& apple, char* banana);

int useful(char* pear);


