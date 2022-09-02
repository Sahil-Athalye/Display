// implementation for LSCommand

#include "LSCommand.h"
#include "AbstractCommand.h"
#include "AbstractFileSystem.h"
#include "AbstractFileFactory.h"
#include "AbstractFile.h"
#include "AbstractFileVisitor.h"
#include "MetadataDisplayVisitor.h"

#include <iostream>
#include <set>
#include <string>

using namespace std;

// basic constructor
LSCommand::LSCommand(AbstractFileSystem* afs)
	: afs_(afs)
{};

// usage message
void LSCommand::displayInfo() {
	cout << "ls lists the files in the filesystem. it can also be used with -m to list metadata" << endl;
}

// execute ls, with possibility of -m

int LSCommand::execute(string args) {
	// ls
	if (args == "") {
		string output; // create string to handle formatting and printing
		for (string file : afs_->getFileNames()) { // loop over files
			if (output.length() < 20) { // new line
				output += file;
				string spaces(20 - file.length(), ' ');
				output += spaces; // fill out to 20 long
			}
			else { // tack on file name and cout it
				output += file;
				cout << output << endl;
				output = "";
			}
		}
		// cout remaining output, also gives us a new line if there are no files
		cout << output << endl;
	}
	// ls -m
	else if (args == "-m") {
		// loop over files
		for (string filename : afs_->getFileNames()) {
			// get file from filename
			AbstractFile* file = afs_->openFile(filename);

			// get data from file with visitor
			AbstractFileVisitor* metadata_visitor = new MetadataDisplayVisitor;
			file->accept(metadata_visitor); // print out the metadata

			// close file to tie up loose ends
			int res = afs_->closeFile(file);
			if (res != 0) {
				return failed_to_close;
			}
		}
	}
	// malformed command
	else {
		return bad_input;
	}

	return success_output;
}