// definition for RemoveCommand

#include "RemoveCommand.h"
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
RemoveCommand::RemoveCommand(AbstractFileSystem* afs)
	: afs_(afs)
{};

// usage message
void RemoveCommand::displayInfo() {
	cout << "rm removes a file from the filesystem. usage: rm <filename>" << endl;
}

// execute rm
int RemoveCommand::execute(string filename) {
	return afs_->deleteFile(filename);
}