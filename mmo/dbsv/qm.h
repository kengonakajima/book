#ifndef _QM_H_
#define _QM_H_

#include "vce2.h"

#include <iostream>
#include <sstream>


class QueryMaker
{
    std::ostringstream query;
    bool firstCol;
    bool finished;
 public:    
    QueryMaker( const char *_command_pre, const char *_tableName, const char *_command_suf );
    void confirmFirstComma();
    void add_string( const char *columnName, const char *data );
    void add_char( const char *columnName, const vce::VSint8 data );
    void add_short( const char *columnName, const vce::VSint16 data );    
    void add_int( const char *columnName, const vce::VSint32 data );
    
    void add_byte( const char *columnName, const vce::VUint8 data );    
    void add_word( const char *columnName, const vce::VUint16 data );    
    void add_dword( const char *columnName, const vce::VUint32 data );    
    void add_qword( const char *columnName, const vce::VUint64 data );

    
    void set_first();
    void append( const char *s );
    void finish();
    std::string output();
};


#endif
