#include "qm.h"


// MySQLアクセスのためのヘッダ
#include "my_global.h"
#include "mysql.h"

QueryMaker::QueryMaker( const char *_command_pre,
                        const char *_tableName,
                        const char *_command_suf )
{
    query << _command_pre << " " << _tableName << " " << _command_suf << " ";
    firstCol = true;
    finished = false;
}

void QueryMaker::confirmFirstComma(){
    if( !firstCol ){
        query << " , ";
    }
    firstCol = false;
}


void QueryMaker::add_string( const char *columnName, const char *data )
{
    confirmFirstComma();
    char *buf = (char*)malloc(strlen(data)*2+1);
    mysql_escape_string( buf, data, strlen(data));
    query << columnName << "='" << buf << "' ";
    free(buf);
}
void QueryMaker::add_char( const char *columnName, const char data )
{
    confirmFirstComma();
    query << columnName << "=" << data << " ";    
}

void QueryMaker::add_short( const char *columnName, const short data )
{
    confirmFirstComma();
    query << columnName << "=" << data << " ";    
}
void QueryMaker::add_int( const char *columnName, const int data )
{
    confirmFirstComma();
    query << columnName << "=" << data << " ";
}
void QueryMaker::add_byte( const char *columnName, const vce::VUint8 data )
{
    confirmFirstComma();
    query << columnName << "=" << ((int)data) << " ";        
}
void QueryMaker::add_word( const char *columnName, const vce::VUint16 data )
{
    confirmFirstComma();
    query << columnName << "=" << data << " ";    
}
void QueryMaker::add_dword( const char *columnName, const vce::VUint32 data )
{
    confirmFirstComma();
    query << columnName << "=" << data << " ";
}
void QueryMaker::add_qword( const char *columnName, const vce::VUint64 data )
{
    confirmFirstComma();
    query << columnName << "=" << data << " ";
}
void QueryMaker::append( const char *s )
{
    query << s;
}
void QueryMaker::set_first()
{
    firstCol = true;
}

void QueryMaker::finish(){
    query << ";";
    finished = true;
}

std::string QueryMaker::output(){
    assert(finished);
    return query.str();
}

