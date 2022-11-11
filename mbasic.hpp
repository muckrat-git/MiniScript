#ifndef _MBASIC_H

#ifndef String
#include <iostream>
#include <string>
#define String std::string
#endif

String token(String raw, char delim, int index) {
	bool quote = false;
	String word;
	for(int i;i<raw.length();++i) {
		if(raw.at(i) == '"') {
			quote = !quote;
		}
		
		if(raw.at(i) == delim && !quote) {
			if(index == 0) {
				return word + "\0";
			}
			word = "";
			--index;
		}
		else {
			word = word + raw.at(i);
		}
	}
}

class MBasic {
	MBasic() {}
	
	void exec(String code) {
		// MBasic code is made of three parts
		// keyword name args
		// E.G run print "Hello world!"
		String keyword; // Tells the interpreter what to read the following code as
		String name;    // The name of whatever is being used/set
		String args;    // Arguments to tell what to do

		
	}
};

#define _MBASIC_H
#endif