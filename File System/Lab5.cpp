// Lab5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../../SharedCode/AbstractCommand.h"
#include "../../SharedCode/MacroCommand.h"
#include "../../SharedCode/AbstractParsingStrategy.h"
#include "../../SharedCode/RenameParsingStrategy.h"
#include "../../SharedCode/CopyCommand.h"
#include "../../SharedCode/RemoveCommand.h"
#include "../../SharedCode/SimpleFileSystem.h"
#include "../../SharedCode/AbstractFileSystem.h"
#include "../../SharedCode/AbstractFileFactory.h"
#include "../../SharedCode/SimpleFileFactory.h"
#include "../../SharedCode/CommandPrompt.h"
#include "../../SharedCode/DisplayCommand.h"
#include "../../SharedCode/CatCommand.h"
#include "../../SharedCode/TouchCommand.h"
#include "../../SharedCode/LSCommand.h"
#include "RenameParsingStrategy2.h"
#include "../../SharedCode/TextFile.h"


int main()
{	
	// Set up basic filesystem, factory, etc
	AbstractFileSystem* afs = new SimpleFileSystem();
	AbstractFileFactory* aff = new SimpleFileFactory();

	// Command prompt
	CommandPrompt* prompt = new CommandPrompt();
	prompt->setFileFactory(aff);
	prompt->setFileSystem(afs);

	// Q8:
	// Define macrocommand with rename parsing strategy to create rename command rn
	MacroCommand* macro = new MacroCommand(afs);

	// Define macrocommand with rename parsing strategy to create rename command rn
	AbstractCommand* cp = new CopyCommand(afs);
	AbstractCommand* rm = new RemoveCommand(afs);

	// add to command prompt
	prompt->addCommand("cp", cp);
	prompt->addCommand("rm", rm);

	AbstractParsingStrategy* strat = new RenameParsingStrategy();

	// configure
	macro->addCommand(cp);
	macro->addCommand(rm);

	macro->setParseStrategy(strat);

	// add to command prompt
	prompt->addCommand("rn", macro);

	//LS command
	AbstractCommand* ls = new LSCommand(afs); 
	prompt->addCommand("ls", ls);

	//Cat command
	AbstractCommand* cat = new CatCommand(afs);
	prompt->addCommand("cat", cat);

	//Ds command
	AbstractCommand* ds = new DisplayCommand(afs);
	prompt->addCommand("ds", ds);

	//Touch command
	AbstractCommand* touch = new TouchCommand(afs, aff);
	prompt->addCommand("touch", touch);



	// Q9:Cat+ds command
	MacroCommand* macro2 = new MacroCommand(afs);
	macro2->addCommand(cat);
	macro2->addCommand(ds);
	AbstractParsingStrategy* strat2 = new RenameParsingStrategy2();
	macro2->setParseStrategy(strat2);
	prompt->addCommand("cat+ds", macro2);

	prompt->run();

	return 0;
}


