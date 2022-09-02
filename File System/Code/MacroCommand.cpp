// MacroCommand definition

#include "MacroCommand.h"
#include "AbstractCommand.h"
#include "AbstractFileSystem.h"
#include "AbstractFile.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

MacroCommand::MacroCommand(AbstractFileSystem* afs)
	: afs_(afs) {};

void MacroCommand::displayInfo() {
	cout << "Executes a macrocommand specified by user. " << endl;
}

// adds given command to commands vector
void MacroCommand::addCommand(AbstractCommand* c) {
	commands.push_back(c);
}

// sets parsing strategy
void MacroCommand::setParseStrategy(AbstractParsingStrategy* ps) {
	strategy = ps;
}

// execute our macrocommand
int MacroCommand::execute(string s) {
	// get parsed input from strategy
	vector<string> parsed_input = strategy->parse(s);

	// execute commands with input
	for (int i = 0; i < parsed_input.size(); ++i) {
		int res = commands[i]->execute(parsed_input[i]);
		if (res != 0) {
			// if commands fails, return error
			return res;
		}
	}

	// no fail => success
	return success_output;
}