#pragma once
// declaration of SimpleFileSystem class goes here

#include "AbstractFileSystem.h"
#include <string>
#include <map>
#include <set>

class SimpleFileSystem : public AbstractFileSystem {
private:
	std::map<std::string, AbstractFile*> m_;
	std::set<AbstractFile*> s_;
public:
	virtual int addFile(std::string, AbstractFile*) override;
	virtual int deleteFile(std::string) override;
	virtual AbstractFile* openFile(std::string) override;
	virtual int closeFile(AbstractFile*) override;

	virtual std::set<std::string> getFileNames() override;
};