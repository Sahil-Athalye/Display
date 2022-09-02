// declaration for MacroCommand
#pragma once

#include "AbstractCommand.h"
#include "AbstractFileSystem.h"
#include "AbstractParsingStrategy.h"

#include <vector>

class MacroCommand : public AbstractCommand {
private:
	std::vector<AbstractCommand*> commands;
	AbstractParsingStrategy* strategy;
	AbstractFileSystem* afs_;
public:
	MacroCommand(AbstractFileSystem*);
	virtual int execute(std::string) override;
	virtual void displayInfo() override;
	virtual ~MacroCommand() = default;

	void addCommand(AbstractCommand*);
	void setParseStrategy(AbstractParsingStrategy*);
};