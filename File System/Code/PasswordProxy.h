#pragma once

#include "AbstractFile.h"
#include "AbstractFileVisitor.h"

#include <string>

class PasswordProxy : public AbstractFile {
private:
	AbstractFile* file;
	std::string password;
protected:
	std::string passwordPrompt();
	bool matchesPassword(std::string);
public:
	PasswordProxy(AbstractFile*, std::string password);
	~PasswordProxy();
	virtual std::vector<char> read() override;
	virtual int write(std::vector<char>) override;
	virtual int append(std::vector<char>) override;
	virtual unsigned int getSize() override;
	virtual std::string getName() override;
	virtual void accept(AbstractFileVisitor*) override;

	virtual AbstractFile* clone(std::string) override;
};