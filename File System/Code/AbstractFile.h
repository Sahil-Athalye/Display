#pragma once
// declare AbstractFile here. As all methods are virtual and will not be defined, no .cpp file is necessary
#include<vector>
#include<string>

class AbstractFileVisitor;

enum return_vals {success_output, bad_chars, size_mismatch, not_supported, already_exists, null_ptr, bad_type, file_not_open, 
					does_not_exist, file_open, incorrect_password, failed_to_insert, quit, file_creation_failed, file_add_failed, bad_input,
					failed_to_close
				 };

enum arg_vals {filename, second_input};

class AbstractFile {
public:
	virtual std::vector<char> read() = 0;
	virtual int write(std::vector<char>) = 0;
	virtual int append(std::vector<char>) = 0;
	virtual unsigned int getSize() = 0;
	virtual std::string getName() = 0;
	virtual ~AbstractFile() = default;
	virtual void accept(AbstractFileVisitor*) = 0;
	
	virtual AbstractFile* clone(std::string new_name) = 0;
};