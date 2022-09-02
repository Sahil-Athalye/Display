#pragma once
// Image file class declaration here

#include <string>
#include <vector>

#include "AbstractFile.h"
#include "AbstractFileVisitor.h"

class ImageFile : public AbstractFile {
private:
	std::string name;
	std::vector<char> contents;
	char size;
public:
	ImageFile(std::string);
	virtual unsigned int getSize() override;
	virtual std::string getName() override;
	virtual int write(std::vector<char>) override;
	virtual int append(std::vector<char>) override;
	virtual std::vector<char> read() override;
	virtual void accept(AbstractFileVisitor*) override;
	virtual unsigned int getImageSize();

	virtual AbstractFile* clone(std::string) override;
};