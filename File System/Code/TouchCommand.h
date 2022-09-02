#pragma once

#include "AbstractCommand.h"
#include "AbstractFileSystem.h"
#include "AbstractFileFactory.h"

class TouchCommand : public AbstractCommand {
private:
	AbstractFileSystem* afs_;
	AbstractFileFactory* aff_;
public:
	TouchCommand(AbstractFileSystem*, AbstractFileFactory*);
	virtual int execute(std::string) override;
	virtual void displayInfo() override;
	virtual ~TouchCommand() = default;
};