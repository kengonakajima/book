# -*- coding: utf-8; -*-

import os
os.environ['DJANGO_SETTINGS_MODULE'] = "settings" 

#
# Django settings
#
from django.conf import settings
from django.template import Template, Context
from django.utils.html import escape
from django.utils.safestring import SafeData, EscapeData




def prettify(s):
    out = ""
    for l in s.split("\n"):
        orig = l
        omitted = l.strip()
        if( omitted == "//" or omitted == "" ): continue
        out += orig+"\n"
    return out
    

class Database:
    name=None
    version=None
    tables=[]
    def __init__(self,name,version):
        self.name=name
        self.version = version

    def add(self,tbl):
        if tbl.get_primary() == None: raise "primary key is neccessary"
        self.tables.append(tbl)
        
    def create_sql_source(self, drop=False, create=True ):
        t = Template( open("template.sql" ).read() )
        return prettify( t.render( Context( {
                        "db": self,
                        "drop" : drop,
                        "create" : create
                        } )) )
    def create_proto_xml(self):
        t = Template( open("template.xml" ).read() )
        return prettify( t.render( Context( { "db": self } ) ))

    def create_sv_cpp(self):
        t = Template( open("svtemplate.cpp" ).read() )
        return prettify( t.render( Context( { "db" : self } ) ))

    def create_sv_h(self):
        t = Template( open("svtemplate.h" ).read() )
        return prettify(t.render( Context( { "db" : self } ) ))

    def create_cl_h(self):
        t = Template( open("cltemplate.h" ).read() )
        return prettify(t.render( Context( { "db" : self } ) ))

    def create_cl_cpp(self):
        t = Template( open("cltemplate.cpp" ).read() )
        return prettify(t.render( Context( { "db" : self } ) ))
        
class Field:

    def __init__(self,name,type,primary=False,auto_increment=False, size=None, index=False, unique=False, conditional=False ):
        self.name=name
        self.type=type
        
        self.primary=primary
        self.auto_increment=auto_increment
        self.size=size
        self.index=index
        self.unique=unique
        self.conditional=conditional

    # FIXME: 型情報は表にすると見やすいだろう
    def type_in_sql(self):
        gen_to_sql={
            "char" : "tinyint",
            "byte" : "tinyint unsigned",
            "short" : "smallint",
            "word" : "smallint unsigned",        
            "int" : "int",
            "dword" : "int unsigned",
            "qword" : "bigint",        
            "string" : "char",
            }
        return gen_to_sql[self.type]

    def type_in_cpp(self):
        gen_to_cpp={
            "char" : "vce::VSint8",
            "byte" : "vce::VUint8",
            "short" : "vce::VSint16",
            "word" : "vce::VUint16",
            "int" : "vce::VSint32",
            "dword" : "vce::VUint32",
            "qword" : "vce::VUint64",
            "string" : "const char *",
            }
        return gen_to_cpp[self.type]
    def converter(self):
        convs={
            "char" : "atoi",
            "byte" : "atoi",
            "short" : "atoi",
            "word" : "atoi",
            "int" : "atoi",
            "dword" : "atoi",
            "qword" : "atoll",
            "string" : "",
            }
        return convs[self.type]
    def is_string(self):
        isstr={
            "char" : False,
            "byte" : False,
            "short" : False,
            "word" : False,
            "int" : False,
            "dword" :False,
            "qword" :False,
            "string" : True,
            }
        return isstr[self.type]
        
    
class Table:
    def __init__(self,name):
        self.name=name
        self.fields=[]
    def add(self, f ):
        self.fields.append(f)
    def struct_name(self):
        out=""
        for c in self.name:
            if out=="":
                out = c.upper()
            else:
                out += c
        return out
    def get_primary(self):
        for f in self.fields:
            if f.primary : return f
        return None
    
