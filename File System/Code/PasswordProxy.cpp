#include "PasswordProxy.h"
#include "AbstractFile.h"
#include "AbstractFileVisitor.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

PasswordProxy::PasswordProxy(AbstractFile* f, string password)
	: file(f), password(password)
{};

PasswordProxy::~PasswordProxy() {
	file = nullptr;
}

string PasswordProxy::passwordPrompt() {
	cout << "Please input a password: " << endl;
	string pass;
	cin >> pass;
	return pass;
}

bool PasswordProxy::matchesPassword(string s) {
	return s == password;
}

vector<char> PasswordProxy::read() {
	string pass = passwordPrompt();
	if (matchesPassword(pass)) {
		return file->read();
	}
	return vector<char>();
}

int PasswordProxy::write(vector<char> v) {
	string pass = passwordPrompt();
	if (matchesPassword(pass)) {
		return file->write(v);
	}
	return return_vals::incorrect_password;
}

int PasswordProxy::append(vector<char> v) {
	string pass = passwordPrompt();
	if (matchesPassword(pass)) {
		return file->append(v);
	}
	return return_vals::incorrect_password;
}

unsigned int PasswordProxy::getSize() {
	return file->getSize();
}

string PasswordProxy::getName() {
	return file->getName();
}

void PasswordProxy::accept(AbstractFileVisitor* afs) {
	string pass = passwordPrompt();
	if (matchesPassword(pass)) {
		return file->accept(afs);
	}
}

// proxy pattern clone function
AbstractFile* PasswordProxy::clone(string new_name) {
	// clone underlying file
	AbstractFile* new_file = file->clone(new_name);

	// if the clone here fails, it all fails
	if (new_file == nullptr) {
		return nullptr;
	}

	// otherwise, we're good to clone a new password proxy as well
	AbstractFile* new_proxy = new PasswordProxy(new_file, this->password); // instantiate new space in memory
	return new_proxy;
}