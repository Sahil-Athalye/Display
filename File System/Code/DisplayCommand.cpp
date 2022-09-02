// implementation for DisplayCommand

#include "DisplayCommand.h"
#include "AbstractCommand.h"
#include "AbstractFileSystem.h"
#include "AbstractFile.h"
#include "AbstractFileVisitor.h"
#include "BasicDisplayVisitor.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// basic constructor
DisplayCommand::DisplayCommand(AbstractFileSystem* afs)
	: afs_(afs)
{};

// usage message
void DisplayCommand::displayInfo() {
	cout << "ds displays the file content, by default with formatting. Usage: ds <filename> [-d]" << endl;
}

// execute ds, with possibility of -d
int DisplayCommand::execute(string args) {
	// parse args to get filename and -d if necessary
	stringstream ss(args); // wrap string into stream
	string temp;
	vector<string> strings;

	// extract while possible, essentially splits string by spaces and puts new strings in vector
	while (ss >> temp) {
		strings.push_back(temp);
	}

	string filename = strings[arg_vals::filename]; // first arg is always the filename, improper formatting handled by openFile

	// get file from name
	AbstractFile* file = afs_->openFile(filename);

	// check if file doesn't exist
	if (file == nullptr) {
		return does_not_exist;
	}

	vector<char> contents = file->read();

	// check for second arg
	if (strings.size() == 2) {
		if (strings.at(arg_vals::second_input) == "-d") {
			// unformatted mode
			for (char c : contents) {
				cout << c;
			}
			cout << endl;
		}
	}
	else {
		// formatted mode, use display visitor
		AbstractFileVisitor* v = new BasicDisplayVisitor();
		file->accept(v); // handles formatting
	}

	return afs_->closeFile(file);
}