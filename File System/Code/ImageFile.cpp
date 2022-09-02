// definition of ImageFile class here

#include "ImageFile.h"
#include "AbstractFile.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

ImageFile::ImageFile(string name)
	: name(name), size('0') {};

unsigned int ImageFile::getSize() {
	//int size_int = size - '0';
	int size_int = contents.size();
	return size_int;
}

string ImageFile::getName() {
	return name;
}

int ImageFile::write(vector<char> v) {
	char sizeChar = v[v.size() - 1];
	int trueSize = sizeChar - '0';

	// clear vector
	contents.clear();
	for (char c : v) {
		// check if we're adding the last value, since its not part of the image
		if (c == sizeChar) break;

		// check if we have a malformed input (not space or X)
		if (c != 'X' && c != ' ') {
			//cout << "here" << endl;
			contents.clear();
			//cout << "here" << endl;
			size = '0';
			return bad_chars;
		}

		// otherwise, add to contents
		contents.push_back(c);
	}

	// check if contents has the expected number of values
	if (contents.size() != trueSize * trueSize) {
		//cout << trueSize << endl;
		//cout << contents.size() << endl;
		contents.clear();
		size = '0';
		return size_mismatch;
	}

	// worked, so set our size to the right value
	size = char(trueSize) + '0'; //contents.size(); //char(trueSize * trueSize) + '0';

	return success_output;
}

unsigned int ImageFile::getImageSize() {
	return size - '0';
}

int ImageFile::append(vector<char> v) {
	return not_supported;
}

vector<char> ImageFile::read() {
	return contents;
}

// visitor pattern accept function
void ImageFile::accept(AbstractFileVisitor* a) {
	a->visit_ImageFile(this);
}

// proxy pattern clone function
AbstractFile* ImageFile::clone(string new_name) {
	AbstractFile* new_file = new ImageFile(new_name + ".txt"); // instantiate new space in memory
	int res = new_file->write(this->read()); // copy data over to new file
	if (res != 0) { // if error in the above process, return nullptr
		return nullptr;
	}
	return new_file;
}