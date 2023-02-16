#include "mbasic.hpp"

#include "types.hpp"

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
	if(str[0] == '[' && str[str.length()-1] == ']') {
		return type(LIST, str.substr(1, str.length()-2));
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

void types::init_list(String varname, types::type list, int max_items, MBasic * scope) {
    String lines[max_items];
	int len = split_p(list.val, ',', "\"{[(", "\"}])", lines);
    String name;
    for(int i=0;i < len;++i) {
        name = varname + "[" + to_string(i) + "]";
        scope->variables.insert(pair<String, MBasic::var>(name, { name, types::from_str(trim(lines[i]), scope) } ));
    }
    scope->variables.insert(pair<String, MBasic::var>(varname + ".size", { varname + ".size", type(types::INTEGER, to_string(len)) } ));
}

String process_brackets(String in, MBasic * scope) {
	if(in[0] == '[' && in[in.length()-1] == ']')
		return in;
	if(in[0] == '{' && in[in.length()-1] == '}')
		return in;
	if(in[0] == '(' && in[in.length()-1] == ')')
		return in;
	String out = "";
	int brace = 0;
	String term = "";
	for(int i=0;i<in.length();++i) {
		if(in[i] == '[') {
			if(brace==0)
				term = "";
			++brace;
		}
		else if(in[i] == ']'){
			--brace;
			if(brace==0)
				out = out + "[" + types::from_str(term, scope).val + "]";
		}
		
		else if(brace==0)
			out += in[i];
		else
			term += in[i];
	}
	return out;
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
	name = process_brackets(name, this);

	split(trim(tokens[2]), ',', "\"{[(", "\"}])");
	String args[sizeof(strings)/sizeof(strings[0])];
	for(int i = 0;i<sizeof(strings)/sizeof(strings[0]);++i) {
		args[i] = strings[i];
	} 

	for(int i = 0;args[i]!="\0";++i) {
		args[i] = process_brackets(args[i], this);
	}

	// Keywords
	if (keyword == "var") { // Declare a variable
		types::type var = types::from_str(args[0], this);
		this->variables.insert(pair<String, MBasic::var>(name, { name, var } ));
		if(var.id == types::LIST)
			types::init_list(name, var, this->max_list_items, this);
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
	else if (keyword == "def") { // Define a function
		// Functions follow the general structure of [args ...],{code ...}
		types::type var = types::type(types::FUNC, args[0]+","+args[1]);
		this->variables.insert(pair<String, MBasic::var>(name, { name, var } ));
	}
	else if (keyword == "run") { // User defined methods 
		types::type func = types::from_str(name, this);
		if(func.id != types::CODE && func.id != types::FUNC) {
			except(exception::INVALID_DATA_TYPE);
			return this->exception;
		}
		if(this->exception.id)
			return this->exception;
		
		types::type code = types::type(types::CODE, "");
		if(func.id == types::FUNC) {
			String * parts = split(trim(func.val), ',', "\"{[(", "\"}])");
			String rawargs = parts[0];
			String rawcode = parts[1];

			String * argnames = split(trim(rawargs.substr(1, rawargs.length()-2)), ',', "", "");
			
			code = types::from_str(rawcode, this);
			for(int i = 0;argnames[i]!="\0";++i) {
				String varname = (argnames[i]);
				types::type var = types::from_str(args[i], this);
				
				this->variables.insert(pair<String, MBasic::var>(varname, { varname, var } ));
			}
		}
		else 
			code = func;

		if(debug) 
			println("");
		++depth;
		this->exception = run(code.val);
		--depth;
	}
	else if (keyword == "inb") { // Inbuilt interpreter methods
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
		else if (name == "out") {
			types::type t = types::from_str(args[0], this);
			switch(t.id) { // For type specific print options
				case types::BYTE:
					print(to_string(t.val[0]));
					break;
				default:
					print(t.val);
					break;
			}
		}
		else if (name == "type") {
			_return(types::names[types::from_str(args[0], this).id], types::STRING);
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
			
			for(int i = 0;i<stoi(times.val) && !this->exit;++i) {
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

			while(condition.val == "true" && !this->exit) {
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
		else if(name == "try") {
			String code_str = args[0];
			types::type code = types::from_str(code_str, this);
			if(this->exception.id)
				return this->exception;
			if(code.id != types::CODE) {
				except(exception::INVALID_DATA_TYPE);
				return this->exception;
			}

			++depth;
			run(code.val);
			--depth;
			if(this->exception.id) {
				exception.id = 0;
				String exc_str = args[1];
				if(exc_str!="") {
					types::type except_code = types::from_str(exc_str, this);
					if(this->exception.id)
						return this->exception;
					if(except_code.id != types::CODE) {
						except(exception::INVALID_DATA_TYPE);
						return this->exception;
					}
					++depth;
					this->exception = run(except_code.val);
					--depth;
				}
			}
		}
		else if(name == "if") {
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

			if(condition.val == "true") {
				++depth;
				this->exception = run(code.val);
				--depth;
			}
		}
		else if(name == "ifelse") {
			String cond_str = args[0];
			types::type condition = types::from_str(cond_str, this);
			if(this->exception.id)
				return this->exception;
			types::type code = types::from_str(args[1], this);
			if(this->exception.id)
				return this->exception;
			types::type elsecode = types::from_str(args[2], this);
			if(this->exception.id)
				return this->exception;
			if(condition.id!=types::BOOLEAN || code.id!=types::CODE) {
				except(exception::INVALID_DATA_TYPE);
				return this->exception;
			}

			if(condition.val == "true") {
				++depth;
				this->exception = run(code.val);
				--depth;
			}
			else {
				++depth;
				this->exception = run(elsecode.val);
				--depth;
			}
		}
		else if(name == "input") {
			_return(input(), types::STRING);
		}
		else if(name == "getkey") {
			char key = getkey();
			switch(key) {
				case 27:
					this->exit = true;
					return exception::success;
			};
			_return({key, '\0'}, types::CHAR);
		}
		else {
			// Variable casts
			for(int i=0;i<sizeof(types::names)/sizeof(types::names[0]);++i) {
				if (types::names[i] == name) {
					String varname = args[0];
					if(varname=="\0") {
						if(i==types::INTEGER||i==types::DOUBLE)
							_return("0", i);
						if(i==types::BOOLEAN)
							_return("false", i);
						else
							_return({0}, i);
					}
					else {
						types::type ret = types::cast(types::from_str(varname, this), i, this);
						_return(ret.val, i);
					}
					return this->exception;
				}
			}
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
	else if(keyword == "cmp") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::compare(a, b, this);
		_return(ret.val, ret.id);
	}
	else if(keyword == "not") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::invert(a, this);
		_return(ret.val, ret.id);
	}
	else if(keyword == "big") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::bigger_than(a, b, this);
		_return(ret.val, ret.id);
	}
	else if(keyword == "les") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::smaller_than(a, b, this);
		_return(ret.val, ret.id);
	}
	else if(keyword == "and") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::bool_and(a, b, this);
		_return(ret.val, ret.id);
	}
	else if(keyword == "or") {
		types::type a = types::from_str(name, this);
		if(this->exception.id)
			return this->exception;
		types::type b = types::from_str(args[0], this);
		if(this->exception.id)
			return this->exception;
		types::type ret = types::bool_or(a, b, this);
		_return(ret.val, ret.id);
	}

	return this->exception;
}

struct exception::exception MBasic::run(String code) {
	String lines[max_lines];
	int len = split_p(code, '\n', "\"{[(", "\"}])", lines);
	struct exception::exception ret;
	for(line=0;line < len && !this->exit;++line) {
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