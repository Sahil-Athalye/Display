// definitions of basic display visitor here

#include "BasicDisplayVisitor.h"
#include "TextFile.h"
#include "ImageFile.h"

#include <iostream>
#include <vector>

using namespace std;

int BasicDisplayVisitor::toIndex(int size_int, int X, int Y) {
	return Y * size_int + X;
}

void BasicDisplayVisitor::visit_TextFile(TextFile* tf) {
	for (char c : tf->read()) {
		cout << c;
	}
	cout << endl;
};

void BasicDisplayVisitor::visit_ImageFile(ImageFile* ifi) {
	int size = ifi->getImageSize();
	vector<char> contents = ifi->read();
	for (int i = size - 1; i >= 0; --i) {
		for (int j = 0; j < size; j++) {
			int idx = toIndex(size, j, i);
			cout << contents[idx];
		}
		cout << endl;
	}
};