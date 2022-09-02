// declaration for DisplayCommand
#pragma once

#include "AbstractCommand.h"
#include "AbstractFileSystem.h"

class DisplayCommand : public AbstractCommand {
private:
	AbstractFileSystem* afs_;
public:
	DisplayCommand(AbstractFileSystem*);
	virtual int execute(std::string) override;
	virtual void displayInfo() override;
	virtual ~DisplayCommand() = default;
};