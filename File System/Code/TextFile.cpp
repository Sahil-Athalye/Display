//  Define the TextFile class here
#include "TextFile.h"
#include<vector>
#include<string>
#include<iostream>

using namespace std;

// basic constructor
TextFile::TextFile(string name) :
	name(name)
{};

// gets size from file
unsigned int TextFile::getSize() {
	return contents.size();
}

// gets name from file
string TextFile::getName() {
	return name;
}

// writes content onto file, returns coded int
int TextFile::write(vector<char> rewrite) {
	// clear vector
	contents.clear();
	// add rewrite content
	for (char c : rewrite) {
		contents.push_back(c);
	}
	return 0;
}

// appends data to file, returns coded int
int TextFile::append(vector<char> a) {
	// append our new input to the end of contents
	for (char c : a) {
		contents.push_back(c);
	}
	return 0;
}

// returns contents of file as vector
vector<char> TextFile::read() {
	return contents;
}

// visitor pattern accept function
void TextFile::accept(AbstractFileVisitor* a) {
	a->visit_TextFile(this);
}

// proxy pattern clone function
AbstractFile* TextFile::clone(string new_name) {
	AbstractFile* new_file = new TextFile(new_name + ".txt"); // instantiate new space in memory
	int res = new_file->write(this->read()); // copy data over to new file
	if (res != 0) { // if error in the above process, return nullptr
		return nullptr;
	}
	return new_file;
}

