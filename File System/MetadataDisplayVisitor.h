#pragma once
// declaration of MetadataDisplayVisitor here

#include "AbstractFileVisitor.h"

class MetadataDisplayVisitor : public AbstractFileVisitor {
public:
	virtual void visit_TextFile(TextFile*);
	virtual void visit_ImageFile(ImageFile*);
};