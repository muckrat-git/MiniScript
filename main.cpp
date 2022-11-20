#include <iostream>
#include <stdio.h>

#include "mbasic.cpp"

using namespace std;

void print(string text) {
	std::cout << text;
}

void println(string text) {
	std::cout << text << '\n';
}

int main() {
	MBasic mbasic;

	unsigned short status = mbasic.exec("run print \"Hello World!\"");
	switch(status) {
		case 1:
			println("[E] Expected a keyword");
			break;
		case 2:
			println("[E] Expected a name");
			break;
		case 3:
			println("[E] Expected an argument");
			break;
		case 4:
			println("[E] Incompatible data types");
			break;
		case 5:
			println("[E] Invalid operation for data type");
			break;
		case 6:
			println("[E] Unknown data type");
			break;
		default:
			break;
	}
}