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
String * split(String str, char seperator, String avoid, String unavoid) {  
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
int split_p(String str, char seperator, String avoid, String unavoid, String * buf) {  
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
            String subStr = "";  
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

String trim(String str) {
   if (str.empty()) return str;
   String::size_type i,j;
   i=0;
   while (i<str.size() && isspace(str[i])) ++i;
   if (i == str.size())
      return String(); // empty string
   j = str.size() - 1;
   //while (j>0 && isspace(str[j])) --j; // the j>0 check is not needed
   while (isspace(str[j])) --j;
   return str.substr(i, j-i+1);
}

MBasic::MBasic() {}

struct MBasic::var {
	String name;
	types::type type;
};
void MBasic::except(unsigned short e) {
	exception.id = e;
	exception.line = this->line;
	exception.term = this->current;
}

types::type types::from_str(String str, MBasic * scope) {
	if(str == "void") {
		return type(VOID, "");
	}
	if(str == "true") {
		return type(BOOLEAN, "true");
	}
	if(str == "false") {
		return type(BOOLEAN, "false");
	}
    if(str[0] == '"' && str[str.length()-1] == '"') {
        return type(STRING, str.substr(1, str.length()-2));
    }
    if(str[0] == '{' && str[str.length()-1] == '}') {
        return type(CODE, str.substr(1, str.length()-2));
    }
	if(str[0] == '(' && str[str.length()-1] == ')') {
		scope->exec(str.substr(1, str.length()-2));
        return type(scope->return_type, scope->return_val);
    }
    // Check double or integer
    bool dub = false;
    bool integer = true;
	bool b = false;
    for(int i = 0;i<str.length();++i) {
        if(!(i==0 && str[i] == '-')) {
            if(!isdigit(str[i])) {
                if(str[i]=='.' && !dub) {
                    integer = false;
                    dub = true;
                }
				else if(i==str.length()-1 && str[i] == 'b') {
					b = true;
					integer = false;
					dub = false;
				}
                else {
                    dub = false;
                    integer = false;
                    break;
                }
			}
        }
    }

	if(b && str[0] != 'b') {
		int val = stoi(str.substr(0, str.length()-1));
		if(val>255 || val<0) {
			scope->except(exception::DATA_OVERFLOW);
			return type(VOID, "");
		}
		return type(BYTE, {(char)val, '\0'});
	}
    if(integer) {
        return type(INTEGER, str);
    }
    if(dub) {
        return type(DOUBLE, str);
    }

    if(scope->variables.count(str)) {
        return scope->variables.at(str).type;
    }

    scope->except(exception::INVALID_DATA);
    return type(VOID, "");
}

struct exception::exception MBasic::exec(String code) {
	_return("", types::VOID);

	// Skip if whitespace
	if(trim(code) == "" || trim(code)[0] == '#')
		return exception::success;

	// MBasic code is made of three parts
	// keyword name args
	// E.G run print "Hello world!"
	String keyword; // Tells the interpreter what to read the following code as
	String name;    // The name of whatever is being used/set

	String * tokens = split(trim(code), ' ', "\"{[(", "\"}])");

	keyword = trim(tokens[0]);
	if(keyword.length() == 0) {
		except(exception::EXPECTED_KEYWORD);
		return this->exception;
	}
	name = trim(tokens[1]);
	if(name.length() == 0) {
		except(exception::EXPECTED_NAME);
		return this->exception;
	}
	String * args = split(trim(tokens[2]), ',', "\"{[(", "\"}])");

	// Keywords
	if (keyword == "var") { // Declare a variable
		this->variables.insert(pair<String, MBasic::var>(name, { name, types::from_str(args[0], this) } ));
	}
	else if (keyword == "set") { // Set the value of a variable
		if(this->variables.count(name)) {
			std::map<String, var>::iterator it = this->variables.find(name); 
			if (it != this->variables.end())
    			it->second = { name, types::from_str(args[0], this) };
		}
		else {
			except(exception::VAR_NOT_EXIST);
		}
	}
	else if (keyword == "ret") {
		types::type ret = types::from_str(name, this);
		_return(ret.val, ret.id);
	}
	else if (keyword == "imp") { // Import code from a file
		types::type filename = types::from_str(name, this);
		if(filename.id != types::STRING) {
			except(exception::INVALID_DATA_TYPE);
			return this->exception;
		}
		if(this->exception.id)
			return this->exception;
		
		String code = open(filename.val);
		_return(code, types::CODE);
		++depth;
		this->exception = run(code);
		--depth;
	}
	else if (keyword == "run") { // User defined methods 
		types::type func = types::from_str(name, this);
		if(func.id != types::CODE) {
			except(exception::INVALID_DATA_TYPE);
			return this->exception;
		}
		if(this->exception.id)
			return this->exception;
		
		if(debug) 
			println("");
		++depth;
		this->exception = run(func.val);
		--depth;
	}
	else if (keyword == "inb") { // Inbuilt interpreter methods
		// Lots of "if"s, probably not very good but it works
		if (name == "print") {
			types::type t = types::from_str(args[0], this);
			switch(t.id) { // For type specific print options
				case types::BYTE:
					println(to_string(t.val[0]));
					break;
				default:
					println(t.val);
					break;
			}
		}
		else if (name == "type") {
			switch(types::from_str(args[0], this).id) {
				case types::INTEGER:
					_return("integer", types::STRING);
					break;
				case types::DOUBLE:
					_return("double", types::STRING);
					break;
				case types::STRING:
					_return("string", types::STRING);
					break;
				case types::CODE:
					_return("function", types::STRING);
					break;
				case types::BOOLEAN:
					_return("boolean", types::STRING);
					break;
				case types::CHAR:
					_return("char", types::STRING);
					break;
				case types::BYTE:
					_return("byte", types::STRING);
					break;
				default:
					_return("void", types::STRING);
					break;
			}
		}
		else if (name == "cast") {
			
		}
		else if (name == "jmp") {
			if(types::from_str(args[0], this).id == types::INTEGER) {
				this->line = stoi(types::from_str(args[0], this).val)-2;
				if(this->line<0) {
					except(exception::INVALID_LINE_NUMBER);
					return this->exception;
				}
			}
			else {
				except(exception::INCOMPATIBLE_TYPES);
				return this->exception;
			}
		}
		else if(name == "sleep") {
			if(types::from_str(args[0], this).id == types::DOUBLE) {
				usleep(stod(types::from_str(args[0], this).val)*1000000);
			}
			else {
				except(exception::INCOMPATIBLE_TYPES);
				return this->exception;
			}
		}
		else if(name == "repeat") {
			types::type times = types::from_str(args[0], this);
			if(this->exception.id)
				return this->exception;
			types::type code = types::from_str(args[1], this);
			if(this->exception.id)
				return this->exception;
			if(times.id!=types::INTEGER || code.id!=types::CODE) {
				except(exception::INVALID_DATA_TYPE);
				return this->exception;
			}
			
			for(int i = 0;i<stoi(times.val);++i) {
				++depth;
				this->exception = run(code.val);
				--depth;
				if(this->exception.id)
					return this->exception;
			}
		}
		else if(name == "while") {
			String cond_str = args[0];
			types::type condition = types::from_str(cond_str, this);
			if(this->exception.id)
				return this->exception;
			types::type code = types::from_str(args[1], this);
			if(this->exception.id)
				return this->exception;
			if(condition.id!=types::BOOLEAN || code.id!=types::CODE) {
				except(exception::INVALID_DATA_TYPE);
				return this->exception;
			}

			while(condition.val == "true") {
				++depth;
				this->exception = run(code.val);
				--depth;
				if(this->exception.id)
					return this->exception;
				condition = types::from_str(cond_str, this);
				if(this->exception.id)
					return this->exception;
			}
		}
		else if(name == "input") {
			_return(input(), types::STRING);
		}
		else if(name == "getkey") {
			_return({getkey(), '\0'}, types::CHAR);
		}
		else {
			except(exception::NO_INBUILD_METHOD);
			return this->exception;
		}
	}
	// Math keywords
	else if(keyword == "add") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::add(a, b, this);
		_return(ret.val, ret.id);
	}
	else if(keyword == "sub") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::subtract(a, b, this);
		_return(ret.val, ret.id);
	}
	else if(keyword == "mul") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::multiply(a, b, this);
		_return(ret.val, ret.id);
	}
	else if(keyword == "div") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::divide(a, b, this);
		_return(ret.val, ret.id);
	}
	// Logic statements

	return this->exception;
}

struct exception::exception MBasic::run(String code) {
	String lines[max_lines];
	int len = split_p(code, '\n', "\"{[(", "\"}])", lines);
	struct exception::exception ret;
	for(line=0;line < len;++line) {
		this->current = lines[line];

		// Debug log
		if(this->debug) {
			for(int i=0;i<depth;++i)
				print("\t");
			print("[" + to_string(line+1) + "/" + to_string(len) + "] " + this->current + " {");
		}

		ret = this->exec(trim(this->current));

		// Status
		if(this->debug)
			println("} -> " + (this->return_type!=0 ? this->return_val : "VOID") + "(" + to_string(ret.id) + ")");

		if(ret.id)
			return ret;
	}
	return ret;
}