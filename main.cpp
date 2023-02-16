#include <iostream>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream

// Define platform specific methods
#ifdef _WIN32
#include <Windows.h>
void usleep(__int64 usec) { 
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}
#else
#include <unistd.h>
#include <termios.h>
#include <thread>

// No, I did not make this function, however, I have no clue who did ¯\_(ツ)_/¯
char getch(void) {
   char buf = 0;
   struct termios old = {0};
   fflush(stdout);
   if(tcgetattr(0, &old) < 0)
       perror("tcsetattr()");
   old.c_lflag &= ~ICANON;
   old.c_lflag &= ~ECHO;
   old.c_cc[VMIN] = 1;
   old.c_cc[VTIME] = 0;
   if(tcsetattr(0, TCSANOW, &old) < 0)
       perror("tcsetattr ICANON");
   if(read(0, &buf, 1) < 0)
       perror("read()");
   old.c_lflag |= ICANON;
   old.c_lflag |= ECHO;
   if(tcsetattr(0, TCSADRAIN, &old) < 0)
       perror("tcsetattr ~ICANON");
   return buf;
}
#endif

using namespace std;

void print(string text) {
	std::cout << text;
}

void println(string text) {
	std::cout << text << '\n';
}

char getkey() {
	char key = getch();
	return key;
}

string open(string filename) {
	ifstream f;
    f.open(filename.c_str());
    stringstream stream; stream << f.rdbuf();
    return stream.str();
}

string input() {
	string ret;
	cin >> ret;
	return ret;
}

string strings[8]; //Max strings returned by split  
  
//Mirrors the python function "len" 
int len(string str)  
{  
    int length = 0;  
    for (int i = 0; str[i] != '\0'; i++) {  
        length++;     
    }  
    return length;     
}  
int len(string strs[])  
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
string * split(string str, char seperator, string avoid, string unavoid) {  
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
			if(str[i] == avoid[ai] && !quote[ai]) {
				quote[ai] = true;
			}
			else if(str[i] == unavoid[ai]) {
				quote[ai] = false;
			}

			if(quote[ai]) {
				quoted = true;
			}
		}

        if ((str[i] == seperator && !quoted) || i == len(str)) {  
            endIndex = i;  
            string subStr = "";  
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

int split_p(string str, char seperator, string avoid, string unavoid, string * buf) {  
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
			if(str[i] == avoid[ai] && !quote[ai]) {
				quote[ai] = true;
			}
			else if(str[i] == unavoid[ai]) {
				quote[ai] = false;
			}

			if(quote[ai]) {
				quoted = true;
			}
		}

        if ((str[i] == seperator && !quoted) || i == len(str)) {  
            endIndex = i;  
            string subStr = "";  
            subStr.append(str, startIndex, endIndex - startIndex);  
            buf[currIndex] = subStr;  
            currIndex += 1;  
            startIndex = endIndex + 1;
        }  
        i++;  
    }
	buf[currIndex] = "\0";  
	return currIndex;
}

#include "mbasic/mbasic.cpp"
#include "miniscript/miniscript.cpp"

void raise(struct exception::exception status) {
	println(" " + to_string(status.line) + " | " + status.term);
	print("     ");
	switch(status.id) {
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
		case 7:
			println("[E] Invalid jump position");
			break;
		case 8:
			println("[E] Inbuilt method not found");
			break;
		case 9:
			println("[E] Invalid data type");
			break;
		case 10:
			println("[E] Data too large for data type");
			break;
		case 11:
			println("[E] Source type cannot be casted to chosen data type");
			break;
		case 12:
			println("[E] Variable does not exist");
			break;
	}
}

int main(int argc, char** argv) {
	MBasic mbasic;
	mbasic.debug = false;

	struct exception::exception status = mbasic.run(
R"""(
inb print "MBasic Shell 1.0-dev"
inb while true,{
	inb out "> "
	var exec (inb input)
	inb while true,{
		inb print "test"
	}
}
)""");
	if(status.id)
		raise(status);
}