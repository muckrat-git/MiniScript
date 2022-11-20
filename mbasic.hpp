#ifndef _MBASIC_H

#define _MBASIC_H

#ifndef String
#include <iostream>
#include <string>
#define String std::string
#endif

// Prototype MBasic class
class MBasic {
	unsigned short exception = 0;

	public:
	String variables;

	MBasic();

	void except(unsigned short e);
	unsigned short exec(String code);
};

enum {
	SUCCESS,
	EXPECTED_KEYWORD,
	EXPECTED_NAME,
	EXPECTED_ARGUMENT,
	INCOMPATIBLE_TYPES,
	INVALID_OPERATION,
	INVALID_DATA
};

#endif