#include <iostream>

#include "mbasic.hpp"

using namespace std;

void print(string text) {
	std::cout << text;
}

void println(string text) {
	std::cout << text << '\n';
}

int main() {
  std::cout << "Hello World!\n";
}