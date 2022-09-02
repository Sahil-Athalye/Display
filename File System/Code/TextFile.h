#pragma once
// TextFile declaration goes here

#include "AbstractFile.h"
#include "AbstractFileVisitor.h"

#include<vector>
#include<string>

class TextFile : public AbstractFile {
private:
	std::vector<char> contents;
	std::string name;
public:
	TextFile(std::string);
	virtual std::vector<char> read() override;
	virtual int write(std::vector<char>) override;
	virtual int append(std::vector<char>) override;
	virtual unsigned int getSize() override;
	virtual std::string getName() override;
	virtual void accept(AbstractFileVisitor*) override;

	virtual AbstractFile* clone(std::string) override;
};