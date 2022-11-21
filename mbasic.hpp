#ifndef _MBASIC_H
#define _MBASIC_H

#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#define String std::string

#include "mbasic/exception.hpp"

// Prototype MBasic class
class MBasic {
	struct exception::exception exception = exception::success;

	public:
	struct var;

	String return_val;
	int return_type;
	void _return(String val, int type) {
		return_type = type;
		return_val = val;
	}

	std::map<String, var> variables;

	MBasic();

	// Execution data
	int depth=0; // The depth of the code being executed
	int line; // The line of code being executed	
	String current; // The current line being executed
	bool exit = false; // Whether to exit after the current code line is done

	// Execution config
	int max_lines = 255; // The maximum lines of code to be read
	int max_list_items = 255; // The maximum amount of elements in a list
	bool debug = false; // Whether to print debug messages
	

	void except(unsigned short e);
	struct exception::exception exec(String code);
	struct exception::exception run(String code);
};

#endif