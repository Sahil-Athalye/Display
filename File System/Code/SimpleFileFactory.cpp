// studio 18 - simple file factory definitions
#include "SimpleFileFactory.h"
#include "AbstractFile.h"
#include "TextFile.h"
#include "ImageFile.h"
#include <string>

using namespace std;

AbstractFile*  SimpleFileFactory::createFile(std::string s) {
	string ext = s.substr(s.length() - 3);

	if (ext == "txt") {
		TextFile* t = new TextFile(s);
		return t;
	}
	else if (ext == "img") {
		ImageFile* i = new ImageFile(s);
		return i;
	}
	else {
		return nullptr;
	}
}