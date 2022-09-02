// definitions of metadata visitor here

#include "MetadataDisplayVisitor.h"
#include "TextFile.h"
#include "ImageFile.h"

#include <iostream>

using namespace std;

void MetadataDisplayVisitor::visit_TextFile(TextFile* tf) {
	// display file name, size and type
	string type = "text";
	string spaces_name(20 - tf->getName().length(), ' ');
	string spaces_type(20 - type.length(), ' ');
	cout << tf->getName() << spaces_name << type << spaces_type<< tf->getSize() << endl;
};

void MetadataDisplayVisitor::visit_ImageFile(ImageFile* ifi) {
	// display file name, size and type
	string type = "image";
	string spaces_name(20 - ifi->getName().length(), ' ');
	string spaces_type(20 - type.length(), ' ');
	cout << ifi->getName() << spaces_name << type << spaces_type << ifi->getSize() << endl;
};