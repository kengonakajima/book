/*
  generated code
 */

#include "vce2.h"
#include "dbproto.h"
#include "dbclmain.h"

#include "util.h"

#include <map>



void random_str( char *out, size_t len )
{
    for(int i=0;i<len;i++){
        out[i] = 'A' + ( random() % 25 );
    }
    out[len-1]='\0';
}

int main( int argc, char **argv )
{
    vce::VCE *vceobj;    
    assert(vce::VCEInitialize());
    vceobj = vce::VCECreate();
    vceobj->ReUseAddress(true);
    vceobj->SetPollingWait(true);
    vceobj->SetSelectAlgorithmCallback(MySelectAlgorithmCallback);

    // 通常のクライアントのテスト用
    DBClient *dbcli = new DBClient();
    if( !vceobj->Connect( dbcli, "localhost", 10010 )){
        std::cerr << "fatal error: cannot connect to dbsv" << std::endl;
        return 1;
    }
    dbcli->log=&std::cerr;
    

    
    while(true){


        if( dbcli->GetState() == vce::VCE_STATE_ESTABLISHED
            && dbcli->getLatency() < 3000 ){
            //{% for t in db.tables %} 
            // 構造体を作る
            db_proto::{{ t.struct_name}} tmp_{{t.struct_name}};
            //  {% for qf in t.fields %}
            //    {% if qf.is_string %}
            random_str( tmp_{{t.struct_name}}.{{qf.name}},  {{ qf.size }} );
            //    {% else %}
            tmp_{{t.struct_name}}.{{qf.name}} = ({{qf.type_in_cpp}})(random()%100);
            //    {% endif %}
            //  {% endfor %}

            // クエリ送信
            dbcli->send_put_{{t.name}}( dbcli->uID, tmp_{{t.struct_name}} );
            
            //  {% for f in t.fields %}
            //    {% if f.index or f.primary %}
            dbcli->send_get_{{t.name}}_by_{{f.name}}( dbcli->uID, tmp_{{t.struct_name}}.{{f.name}} );
            dbcli->send_get_{{t.name}}_ranking_by_{{f.name}}( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            //    {% endif %}
            //    {% if f.conditional %}
            // set_if関連

            {
                db_proto::{{t.struct_name}} tmp_if_{{t.struct_name}} = tmp_{{t.struct_name}};
                //{% if f.is_string %}
                vce::strcpy_s( tmp_{{t.struct_name}}.{{f.name}},
                               sizeof(tmp_{{t.struct_name}}.{{f.name}}), "before" );                
                vce::strcpy_s( tmp_if_{{t.struct_name}}.{{f.name}},
                               sizeof(tmp_if_{{t.struct_name}}.{{f.name}}), "after" );
                dbcli->send_put_{{t.name}}( dbcli->uID, tmp_{{t.struct_name}});
                dbcli->send_put_{{t.name}}_if_{{f.name}}( dbcli->uID, tmp_if_{{t.struct_name}}, "before" );
                vce::strcpy_s( tmp_if_{{t.struct_name}}.{{f.name}},
                               sizeof(tmp_if_{{t.struct_name}}.{{f.name}}), "after" );                
                dbcli->send_put_{{t.name}}_if_{{f.name}}( dbcli->uID, tmp_if_{{t.struct_name}}, "after" );
                //{% else %}
                tmp_{{t.struct_name}}.{{f.name}} = 0;
                tmp_if_{{t.struct_name}}.{{f.name}} = 1;
                dbcli->send_put_{{t.name}}( dbcli->uID, tmp_{{t.struct_name}});                
                dbcli->send_put_{{t.name}}_if_{{f.name}}( dbcli->uID, tmp_if_{{t.struct_name}}, 0);
                tmp_if_{{t.struct_name}}.{{f.name}} = 1;                
                dbcli->send_put_{{t.name}}_if_{{f.name}}( dbcli->uID, tmp_if_{{t.struct_name}}, 1);
                //{% endif %}

            }
            
            //    {% endif %}

            //  {% endfor %}
            //{% endfor %}
            dbcli->send_getNewID( db_proto::MAXIDSET );
            dbcli->send_ping( vce::GetTime());
            dbcli->lastPingSentAt = vce::GetTime();
        }
        vceobj->Poll();        
        std::cerr << ".";        

    }

    return 0;
}
//{% for t in db.tables %}
void DBClient::recv_put_{{t.name}}_result( vce::VUint32 sessionID, db_proto_client::ResultCode result, {{t.struct_name}} data )
{
    std::cerr << "recv: {{t.name}} " << std::endl;    
    assert( result == db_proto::SUCCESS || result == db_proto::DUPENTRY );
    std::cerr << "sid:" << sessionID << " uid:" << this->uID << std::endl;
    assert( sessionID == this->uID );
}
//  {% for f in t.fields %}
//    {% if f.index or f.primary %}
void DBClient::recv_get_{{t.name}}_by_{{f.name}}_result( vce::VUint32 sessionID, ResultCode result, const {{t.struct_name}} *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: {{f.name}} " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_{{t.name}}_ranking_by_{{f.name}}_result( vce::VUint32 sessionID, ResultCode result, const {{t.struct_name}} *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: {{f.name}} r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
//    {% endif %}
//    {% if f.conditional %}
void DBClient::recv_put_{{t.name}}_if_{{f.name}}_result( vce::VUint32 sessionID, ResultCode result, const {{t.struct_name}} data )
{
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );
    {% if f.is_string %}
    assert( strcmp( data.{{f.name}}, "after" ) == 0 );
    {% else %}
    assert( data.{{f.name}} == 1 );
    {% endif %}

}

//    {% endif %}
//  {% endfor %}
//{% endfor %}



void DBClient::recv_ping( vce::VUint64 timestamp )
{
    lastPingRecvFromServer = timestamp;
        
    std::cerr << "received ping from dbsv:" << timestamp << std::endl;
}

void DBClient::recv_getNewIDResult(const vce::VUint64 *idarray,vce::VUint32 idarray_qt)
{
    std::cerr << "recv_getNewIDResult: get " << idarray_qt << " ids" << std::endl;
    for(int i=0;i<idarray_qt;i++){
        std::cerr << idarray[i] << ":";
    }
    std::cerr << std::endl;        
}

              
