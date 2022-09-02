// declaration for RenameParsingStrategy

#pragma once

#include "AbstractParsingStrategy.h"

#include <vector>
#include <string>

class RenameParsingStrategy : public AbstractParsingStrategy {
private:
	int expected_size = 2;
public:
	virtual std::vector<std::string> parse(std::string) override;
};
