#include "CommandPrompt.h"
#include "AbstractCommand.h"

#include <map>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

CommandPrompt::CommandPrompt()
	: afs_(nullptr), aff_(nullptr)
{};

void CommandPrompt::setFileSystem(AbstractFileSystem* afs) {
	afs_ = afs;
}

void CommandPrompt::setFileFactory(AbstractFileFactory* aff) {
	aff_ = aff;
}

int CommandPrompt::addCommand(string s, AbstractCommand* command) {
	pair<map<string, AbstractCommand*>::iterator, bool> p = commands.insert(pair<string, AbstractCommand*>(s, command));
	if (p.second) {
		return return_vals::success_output;
	}
	else {
		return return_vals::failed_to_insert;
	}
}

void CommandPrompt::listCommands() {
	for (pair<string, AbstractCommand*> p : commands) {
		cout << p.first << endl;
	}
}

string CommandPrompt::prompt() {
	cout << "Enter a command, q to quit, help for a list of commands, or help followed by a command name for more information about that command." << endl;

	cout << "$    ";
	string res;

	getline(cin, res);
	return res;
}

int CommandPrompt::run() {
	while (true) {
		string input = prompt();

		if (input == "q") {
			return quit;
		}

		bool is_space = false;
		for (char c : input) {
			if (c == ' ') {
				is_space = true;
			}
		}

		if (input == "help") {
			listCommands();
		}

		else if (!is_space) {
			map<string, AbstractCommand*>::iterator it = commands.find(input);

			if (it != commands.end()) { //is found
				int res = it->second->execute("");
				if (res != 0) { // is error
					cout << "The command failed." << endl;
				}
			}
			else { // does not exist
				cout << "This command does not exist." << endl;
			}
		}
		else {
			istringstream iss(input);
			string first_word;
			iss >> first_word;

			if (first_word == "help") { // check if it is a help command
				string second_word;
				iss >> second_word;

				// search for command, display info if it exists
				map<string, AbstractCommand*>::iterator it = commands.find(second_word);
				if (it != commands.end()) { //is found
					cout << "test" << endl;
					it->second->displayInfo();
				}
				else { // does not exist
					cout << "This command does not exist" << endl;
				}
			}
			else { // otherwise, it is a command w an argument
				map<string, AbstractCommand*>::iterator it = commands.find(first_word);
				if (it != commands.end()) { //is found
					string rest_of_args;
					getline(iss, rest_of_args);
					int res = it->second->execute(rest_of_args.substr(1, string::npos)); // substring here to crop leading space
					if (res != 0) {
						cout << "Error with the command" << endl;
					}
				}
				else { // does not exist
					cout << "This command does not exist" << endl;
				}
			}

		}
	}
}