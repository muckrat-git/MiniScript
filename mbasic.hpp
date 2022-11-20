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

	int depth=0;

	// Exec variables
	int line;
	String current;
	int max_lines = 255;
	bool debug = false;

	void except(unsigned short e);
	struct exception::exception exec(String code);
	struct exception::exception run(String code);
};

#endif