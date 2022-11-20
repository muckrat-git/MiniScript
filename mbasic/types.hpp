#ifndef _MBASIC_TYPES

#define _MBASIC_TYPES

#include "../mbasic.hpp"
#include "exception.hpp"

#ifndef String
#include <iostream>
#include <string>
using namespace std;
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
        LIST
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