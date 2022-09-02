#pragma once
#include "AbstractFileFactory.h"
// Studio 18 - simplefilefactory class declaration goes here
class SimpleFileFactory : public AbstractFileFactory {
public:
	virtual AbstractFile* createFile(std::string) override;
};