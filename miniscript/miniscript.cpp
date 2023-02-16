#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#define String std::string

String compile_line(String line) {
    String parts[sizeof(strings)/sizeof(strings[0])];
    String set_type = "\0";
	for(int i = 0;i<sizeof(strings)/sizeof(strings[0]);++i) {
        if(strings[i]=="\0")
            break;
        if(strings[i]=="=" || strings[i]=="+=" || strings[i]=="-=" || strings[i]=="*=" || strings[i]=="/=")
            set_type = strings[i];
		parts[i] = strings[i];
	} 

    if(set_type != "\0") {
        
    }
    if(parts[0] == "var") {

    }
    if(parts[0] == "def") {

    }

}