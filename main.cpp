#include <iostream>
#include <stdio.h>

#include "mbasic.hpp"

using namespace std;

void print(string text) {
	std::cout << text;
}

void println(string text) {
	std::cout << text << '\n';
}

int main() {
	cout << ( token("Hello world! \"Quoted ?\"", ' ', 2).c_str() );
}