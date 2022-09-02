// implementation for CopyCommand

#include "CopyCommand.h"
#include "AbstractCommand.h"
#include "AbstractFileSystem.h"
#include "AbstractFile.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// basic constructor
CopyCommand::CopyCommand(AbstractFileSystem* afs)
	: afs_(afs)
{};

// usage message
void CopyCommand::displayInfo() {
	cout << "copy copies the file given and makes a new version in the file system under a different name. Usage: cp <file_to_copy> <new_name_no_extension>" << endl;
}

// execute copy
int CopyCommand::execute(string args) {
	// simply parse our args and then call the clone method

	stringstream ss(args); // wrap string into stream
	string temp;
	vector<string> strings;

	// extract while possible, essentially splits string by spaces and puts new strings in vector
	while (ss >> temp) {
		strings.push_back(temp);
	}

	if (strings.size() != 2) { // if we don't have exactly two args => malformed input
		return bad_input;
	}

	string filename = strings[arg_vals::filename]; // first arg is always the filename, improper formatting handled by openFile
	string new_name = strings[arg_vals::second_input]; // second arg is new name

	// get file from name
	AbstractFile* file = afs_->openFile(filename);

	// check if file doesn't exist
	if (file == nullptr) {
		return does_not_exist;
	}

	// make copy
	AbstractFile* new_file = file->clone(new_name);

	// close file
	afs_->closeFile(file);

	// try to add to filesystem, return status
	int res = afs_->addFile(new_file->getName(), new_file);

	if (res != 0) { // if it fails, delete the clone
		delete new_file;
	}

	return res; // return our status
}