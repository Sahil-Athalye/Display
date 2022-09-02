#include "TouchCommand.h"
#include "AbstractCommand.h"
#include "AbstractFileSystem.h"
#include "AbstractFileFactory.h"
#include "AbstractFile.h"
#include "PasswordProxy.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

TouchCommand::TouchCommand(AbstractFileSystem* afs, AbstractFileFactory* aff)
	: afs_(afs), aff_(aff) 
{};

void TouchCommand::displayInfo() {
	cout << "touch creates a file (optionally password protected), touch can be invoked with the command: touch <filename> [-p]" << endl;
}

int TouchCommand::execute(string args) {
	// parse args to get filename and -p if necessary
	stringstream ss(args); // wrap string into stream
	string temp;
	vector<string> strings;

	// extract while possible, essentially splits string by spaces and puts new strings in vector
	while (ss >> temp) {
		strings.push_back(temp);
	}

	string filename = strings[arg_vals::filename]; // first arg is always the filename, improper formatting handled by addFile

	// create file
	AbstractFile* file = aff_->createFile(filename);

	// check if -p argument was added
	if (strings.size() > 1) {
		if (strings.at(arg_vals::second_input) == "-p") {
			// p added, so wrap in password proxy
			cout << "Please input a password: " << endl;
			string pass;
			cin >> pass;
			file = new PasswordProxy(file, pass);
		}
		else {
			return bad_input;
		}
	}

	// if successful, add to filesystem, checking if things went wrong in the meantime
	if (file != nullptr) {
		int res = afs_->addFile(filename, file);
		if (res != 0) {
			delete file;
			return file_add_failed;
		}
	}
	else {
		return file_creation_failed;
	}

	return success_output;
}
