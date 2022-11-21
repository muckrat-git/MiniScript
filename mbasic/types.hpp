#ifndef _MBASIC_TYPES

#define _MBASIC_TYPES

#include "../mbasic.hpp"
#include "exception.hpp"

#include <iostream>
#include <algorithm>
#include <map>
#include <string>
using namespace std;

#ifndef String
#define String string
#endif

#ifndef byte
#define byte char
#endif

#ifndef to_string
#define to_string std::to_string
#endif

namespace types {

    enum {
        VOID,
        INTEGER,
        DOUBLE,
        STRING,
        CODE,
        BOOLEAN,
        CHAR,
        BYTE,
        LIST,
        FUNC
    };
    String names[] = { // This does waste a bit of memory but should speed up the casting process by quite a bit
        "void",
        "int",
        "double",
        "str",
        "code",
        "bool",
        "char",
        "byte",
        "list",
        "func"
    };

    class type {
        public:
            byte id = VOID;
            String val = {0};
            type(byte type_id, String str_val) {
                val = str_val;
                id = type_id;
            }
    };

    type from_str(String str, MBasic * scope);

    void init_list(String varname, types::type list, int max_items, MBasic * scope);

    type add(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            println((a.val) + "\t" + (b.val));
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }

        switch(a.id) {
            case INTEGER:
                return type(a.id, to_string(stoi(a.val) + stoi(b.val)));
            case DOUBLE:
                return type(a.id, to_string(stod(a.val) + stod(b.val)));
            case BYTE:
                return type(a.id, to_string(a.val[0] + b.val[0]));
            case CHAR:
            case STRING:
                return type(a.id, a.val+b.val);
            default:
                scope->except(exception::INVALID_OPERATION);
                return type(0,"");
        }
    }

    type subtract(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }

        switch(a.id) {
            case INTEGER:
                return type(a.id, to_string(stoi(a.val) - stoi(b.val)));
            case DOUBLE:
                return type(a.id, to_string(stod(a.val) - stod(b.val)));
            case BYTE:
                return type(a.id, to_string(a.val[0] - b.val[0]));
            default:
                scope->except(exception::INVALID_OPERATION);
                return type(0,"");
        }
    }

    type divide(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }

        switch(a.id) {
            case INTEGER:
                return type(a.id, to_string(stoi(a.val) / stoi(b.val)));
            case DOUBLE:
                return type(a.id, to_string(stod(a.val) / stod(b.val)));
            case BYTE:
                return type(a.id, to_string(a.val[0] / b.val[0]));
            default:
                scope->except(exception::INVALID_OPERATION);
                return type(0,"");
        }
    }

    type multiply(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }

        switch(a.id) {
            case INTEGER:
                return type(a.id, to_string(stoi(a.val) * stoi(b.val)));
            case DOUBLE:
                return type(a.id, to_string(stod(a.val) * stod(b.val)));
            case BYTE:
                return type(a.id, to_string(a.val[0] * b.val[0]));
            default:
                scope->except(exception::INVALID_OPERATION);
                return type(0,"");
        }
    }

    type compare(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }
        return type(BOOLEAN, a.val==b.val ? "true" : "false");
    }

    type invert(type a, MBasic * scope) {
        if(a.id != BOOLEAN)
            return type(BOOLEAN, a.val=="false" ? "true" : "false");
        else {
            scope->except(exception::INVALID_DATA_TYPE);
            return type(0,"");
        }
    }

    type bigger_than(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }
        if(a.id == DOUBLE)
            return type(BYTE, stod(a.val)>stod(b.val) ? "true" : "false");
        else if(a.id == INTEGER)
            return type(BYTE, stoi(a.val)>stoi(b.val) ? "true" : "false");
        else if(a.id == BYTE)
            return type(BYTE, a.val[0]>b.val[0] ? "true" : "false");
        else {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }
    }

    type smaller_than(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }
        if(a.id == DOUBLE)
            return type(BYTE, stod(a.val)<stod(b.val) ? "true" : "false");
        else if(a.id == INTEGER)
            return type(BYTE, stoi(a.val)<stoi(b.val) ? "true" : "false");
        else if(a.id == BYTE)
            return type(BYTE, a.val[0]<b.val[0] ? "true" : "false");
        else {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }
    }

    type bool_and(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }
        if(!a.id == BOOLEAN) {
            scope->except(exception::INVALID_DATA_TYPE);
            return type(0,"");
        }

        return type(BOOLEAN, a.val=="true"&&b.val=="true" ? "true" : "false");
    }

    type bool_or(type a, type b, MBasic * scope) {
        if(a.id != b.id) {
            scope->except(exception::INCOMPATIBLE_TYPES);
            return type(0,"");
        }
        if(!a.id == BOOLEAN) {
            scope->except(exception::INVALID_DATA_TYPE);
            return type(0,"");
        }

        return type(BOOLEAN, a.val=="true"||b.val=="true" ? "true" : "false");
    }

    type cast(type a, int t, MBasic * scope) {
        switch(t) {
            case INTEGER:
                // Casts to an integer
                if(a.id == CHAR || a.id == BYTE)
                    return type(INTEGER, to_string(a.val[0]));
                if(a.id == DOUBLE)
                    return type(INTEGER, to_string((int)stod(a.val)));
                if(a.id == INTEGER)
                    return a;
                if(a.id == BOOLEAN)
                    return type(INTEGER, a.val=="true" ? "1" : "0");
                if(a.id == STRING) {
                    for(int i = 0;i<a.val.length();++i) {
                        if(!(i==0 && a.val[i] == '-')) {
                            if(!isdigit(a.val[i])) {
                                scope->except(exception::INVALID_CAST);
                                return type(0,"");
                            }
                        }
                    }
                    return type(INTEGER, a.val);
                }
            case DOUBLE:
                if(a.id == CHAR || a.id == BYTE)
                    return type(DOUBLE, to_string(a.val[0]));
                if(a.id == DOUBLE || a.id == INTEGER)
                    return type(DOUBLE, a.val);
                if(a.id == BOOLEAN)
                    return type(DOUBLE, a.val=="true" ? "1" : "0");
                if(a.id == STRING) {
                    bool dec = false;
                    for(int i = 0;i<a.val.length();++i) {
                        if(!(i==0 && a.val[i] == '-')) {
                            if(!isdigit(a.val[i])) {
                                if(a.val[i] == '.' && !dec) {
                                    dec = true;
                                }
                                else {
                                    scope->except(exception::INVALID_CAST);
                                    return type(0,"");
                                }
                            }
                        }
                    }
                    return type(DOUBLE, a.val);
                }
            case BOOLEAN:
                if(a.id == CHAR || a.id == BYTE)
                    return type(BOOLEAN, a.val[0] == 0 ? "false" : "true");
                if(a.id == DOUBLE)
                    return type(BOOLEAN, stod(a.val) == 0 ? "false" : "true");
                if(a.id == INTEGER)
                    return type(BOOLEAN, stoi(a.val) == 0 ? "false" : "true");
                if(a.id == BOOLEAN)
                    return a;
            case BYTE:
                if(a.id == CHAR || a.id == BYTE)
                    return type(BYTE, a.val);
                if(a.id == INTEGER || a.id == DOUBLE) {
                    int val = (int)stod(a.val);
                    if(val>255 || val<0) {
			            scope->except(exception::DATA_OVERFLOW);
			            return type(VOID, "");
		            }
                    return type(BYTE, {(char)val, '\0'});
                }
                if(a.id == BOOLEAN)
                    return type(BYTE, {(char)(a.val=="true" ? 1 : 0), '\0'});
                if(a.id == STRING) {
                    for(int i = 0;i<a.val.length();++i) {
                        if(!(i==0 && a.val[i] == '-')) {
                            if(!isdigit(a.val[i])) {
                                scope->except(exception::INVALID_CAST);
                                return type(0,"");
                            }
                        }
                    }
                    int val = (int)stod(a.val);
                    if(val>255 || val<0) {
			            scope->except(exception::DATA_OVERFLOW);
			            return type(VOID, "");
		            }
                    return type(BYTE, {(char)val, '\0'});
                }
            case CHAR:
                if(a.id == CHAR || a.id == BYTE)
                    return type(CHAR, a.val);
                if(a.id == INTEGER || a.id == DOUBLE) {
                    int val = (int)stod(a.val);
                    if(val>255 || val<0) {
			            scope->except(exception::DATA_OVERFLOW);
			            return type(VOID, "");
		            }
                    return type(CHAR, {(char)val, '\0'});
                }
                if(a.id == BOOLEAN)
                    return type(CHAR, {(char)(a.val=="true" ? 1 : 0), '\0'});
                if(a.id == STRING) {
                    for(int i = 0;i<a.val.length();++i) {
                        if(!(i==0 && a.val[i] == '-')) {
                            if(!isdigit(a.val[i])) {
                                scope->except(exception::INVALID_CAST);
                                return type(0,"");
                            }
                        }
                    }
                    int val = (int)stod(a.val);
                    if(val>255 || val<0) {
			            scope->except(exception::DATA_OVERFLOW);
			            return type(VOID, "");
		            }
                    return type(BYTE, {(char)val, '\0'});
                }
            case STRING:
                if(a.id == BYTE)
                    return type(STRING, to_string(a.val[0]));
                return type(STRING, a.val);
            case CODE:
                if(a.id == STRING)
                    return type(CODE, a.val);
            default:
                scope->except(exception::INVALID_CAST);
                return type(0,"");
        }
    }

}

#endif