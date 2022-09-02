#pragma once
// declaration of the file visitor interface here

#include "TextFile.h"
#include "ImageFile.h"

class BasicDisplayVisitor : public AbstractFileVisitor {
private:
	int toIndex(int, int, int);
public:
	virtual void visit_TextFile(TextFile*);
	virtual void visit_ImageFile(ImageFile*);
};