// implementation for CatCommand

#include "CatCommand.h"
#include "AbstractCommand.h"
#include "AbstractFileSystem.h"
#include "AbstractFileFactory.h"
#include "AbstractFile.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// basic constructor
CatCommand::CatCommand(AbstractFileSystem* afs)
	: afs_(afs)
{};

// usage message
void CatCommand::displayInfo() {
	cout << "cat concatenates content to an existing file, with -a used to append content instead of overriding. Usage: cat <filename> [-a]" << endl;
}

// execute cat, with possibility of -a
int CatCommand::execute(string args) {
	// parse args to get filename and -a if necessary
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

	bool append = false;

	// check if we have more args
	if (strings.size() == 2 && strings.at(arg_vals::second_input) == "-a")  {
		append = true;
	}

	// if append, naively display file
	if (append) {
		for (char c : file->read()) {
			cout << c;
		};
		cout << endl; // new line
	}

	// prompt user for input repeatedly
	cout << "Input data to append to file, or :q to quit, or :wq to save and quit" << endl;
	vector<char> content;
	while (true) {
		// prompt
		string res;
		getline(cin, res);

		// quit
		if (res == ":q") {
			// close file and return
			return afs_->closeFile(file);
		}
		// save and quit
		else if (res == ":wq") {
			// pop final newline character
			content.pop_back();

			int res;
			if (append) { // append
				// add content
				res = file->append(content);
			}
			else { // not append mode just writes content
				res = file->write(content);
			}

			//handle error
			if (res != 0) {
				return res; 
			}
			// close file and return
			return afs_->closeFile(file);
		}
		// otherwise, add to content vector and continue prompting
		for (char c : res) {
			content.push_back(c);
		}
		content.push_back('\n'); // add newline character cropped by getline
	}

	return afs_->closeFile(file);
}