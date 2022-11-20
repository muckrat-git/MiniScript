#include "mbasic.hpp"

#include "mbasic/types.hpp"

String strings[8]; //Max strings returned by split  
  
//Mirrors the python function "len" 
int len(String str)  
{  
    int length = 0;  
    for (int i = 0; str[i] != '\0'; i++) {  
        length++;     
    }  
    return length;     
}  
int len(String strs[])  
{  
    int length = 0; 
	bool end = false; 
    while(!end){
		try {
			strs[length];
		}
		catch(const std::exception& e) {
			end = true;
		}
        length++;  
    }  
    return length;     
}  
  
//Mirrors the python function "split"
String * split(String str, char seperator, String avoid) {  
    int currIndex = 0, i = 0;  
    int startIndex = 0, endIndex = 0;
	bool quote[avoid.length()];
	for(int ai = 0;ai<avoid.length();++ai) {
		quote[ai] = false;
	}
	bool quoted = false;
    while (i <= len(str)) {
		quoted = false;
		for(int ai = 0;ai<avoid.length();++ai) {
			if(str[i] == avoid[ai]) {
				quote[ai] = !quote[ai];
			}

			if(quote[ai]) {
				quoted = true;
			}
		}

        if ((str[i] == seperator && !quoted) || i == len(str)) {  
            endIndex = i;  
            String subStr = "";  
            subStr.append(str, startIndex, endIndex - startIndex);  
            strings[currIndex] = subStr;  
            currIndex += 1;  
            startIndex = endIndex + 1;
        }  
        i++;  
    }
	strings[currIndex] = "\0";  
	return strings;
}

MBasic::MBasic() {}

void MBasic::except(unsigned short e) {
	exception = e;
}

unsigned short MBasic::exec(String code) {
	// MBasic code is made of three parts
	// keyword name args
	// E.G run print "Hello world!"
	String keyword; // Tells the interpreter what to read the following code as
	String name;    // The name of whatever is being used/set
	String args;    // Arguments to tell what to do

	String * tokens = split(code, ' ', "\"{}[]()");

	keyword = tokens[0];
	if(keyword.length() == 0) {
		return EXPECTED_KEYWORD;
	}
	name = tokens[1];
	if(name.length() == 0) {
		return EXPECTED_NAME;
	}
	args = tokens[2];
	if(args.length() == 0) {
		return EXPECTED_ARGUMENT;
	}

	// Check keyword
	if(keyword == "run") {}

	return SUCCESS;
}