// declaration for CopyCommand
#pragma once

#include "AbstractCommand.h"
#include "AbstractFileSystem.h"

class CopyCommand : public AbstractCommand {
private:
	AbstractFileSystem* afs_;
public:
	CopyCommand(AbstractFileSystem*);
	virtual int execute(std::string) override;
	virtual void displayInfo() override;
	virtual ~CopyCommand() = default;
};