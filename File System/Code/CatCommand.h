// declaration for CatCommand
#pragma once

#include "AbstractCommand.h"
#include "AbstractFileSystem.h"

class CatCommand : public AbstractCommand {
private:
	AbstractFileSystem* afs_;
public:
	CatCommand(AbstractFileSystem*);
	virtual int execute(std::string) override;
	virtual void displayInfo() override;
	virtual ~CatCommand() = default;
};