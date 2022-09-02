// declaration for RemoveCommand
#pragma once

#include "AbstractCommand.h"
#include "AbstractFileSystem.h"

class RemoveCommand : public AbstractCommand {
private:
	AbstractFileSystem* afs_;
public:
	RemoveCommand(AbstractFileSystem*);
	virtual int execute(std::string) override;
	virtual void displayInfo() override;
	virtual ~RemoveCommand() = default;
};