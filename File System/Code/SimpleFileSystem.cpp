// define methods of SimpleFileSystem class here

#include "SimpleFileSystem.h"
#include "AbstractFile.h"
#include "TextFile.h"
#include "ImageFile.h"

#include <string>
#include <map>

using namespace std;

int SimpleFileSystem::addFile(string s, AbstractFile* f) {
	auto it = m_.find(s);
	if (it != m_.end()) {
		return return_vals::already_exists;
	}
	
	if (f == nullptr) {
		return return_vals::null_ptr;
	}

	m_.insert(pair<string, AbstractFile*>(s, f));
	return return_vals::success_output;
}

AbstractFile* SimpleFileSystem::openFile(string s) {
	// check if exists
	auto it = m_.find(s);
	if (it == m_.end()) {
		return nullptr;
	}

	// check if open
	AbstractFile* f = it->second;
	if (s_.find(f) != s_.end()) {
		return nullptr;
	}

	// add to open files
	s_.insert(f);

	return f;
}

int SimpleFileSystem::closeFile(AbstractFile* f) {
	// check if file is open
	auto it = s_.find(f);
	if (it == s_.end()) {
		return return_vals::file_not_open;
	}

	// close file
	s_.erase(it);
	return 0;
}

int SimpleFileSystem::deleteFile(string s) {
	// check if file exists
	auto it = m_.find(s);
	if (it == m_.end()) {
		return return_vals::does_not_exist;
	}

	// check if open
	AbstractFile* f = it->second;
	if (s_.find(f) != s_.end()) {
		return return_vals::file_open;
	}

	// remove file from map
	m_.erase(it);

	// delete file
	delete f;

	return 0;
}

set<string> SimpleFileSystem::getFileNames() {
	set<string> names;
	for (pair<string, AbstractFile*> p : m_) {
		names.insert(p.first);
	}
	return names;
}