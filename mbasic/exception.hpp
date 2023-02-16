#ifndef _MBASIC_EXCEPTION
#define _MBASIC_EXCEPTION

#include <string>
#define String std::string

namespace exception {
    enum {
	    SUCCESS,
	    EXPECTED_KEYWORD,
	    EXPECTED_NAME,
	    EXPECTED_ARGUMENT,
	    INCOMPATIBLE_TYPES,
	    INVALID_OPERATION,
	    INVALID_DATA,
	    INVALID_LINE_NUMBER,
	    NO_INBUILD_METHOD,
	    INVALID_DATA_TYPE,
        DATA_OVERFLOW,
        INVALID_CAST,
        VAR_NOT_EXIST
    };

    struct exception {
        unsigned short id = 0;
        String term = "";
        int line = 0;
    }success;
};

#endif