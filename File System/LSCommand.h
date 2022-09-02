// declaration for LSCommand
#pragma once

#include "AbstractCommand.h"
#include "AbstractFileSystem.h"

class LSCommand : public AbstractCommand {
private:
	AbstractFileSystem* afs_;
public:
	LSCommand(AbstractFileSystem*);
	virtual int execute(std::string) override;
	virtual void displayInfo() override;
	virtual ~LSCommand() = default;
};