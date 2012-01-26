DEF=k_table_def.py
OUTSQL=k_table_create.sql
OUTXML=dbproto.xml
OUTSVMAINSRC=dbsvmain.cpp dbsvmain.h
OUTCLMAINSRC=dbclmain.cpp dbclmain.h
OUTPROTOSRC=dbproto.cpp dbproto.h
CC=g++

VCEROOT=../../vce/darwin_special_version/r8644
GEN=$(VCEROOT)/gen
BOOST=../../boost/boost_1_41_0

DBSVSRC=$(OUTSVMAINSRC) $(OUTPROTOSRC) util.cpp qm.cpp
DBSVOBJ=$(DBSVSRC:.cpp=.o)
DBCLSRC=$(OUTCLMAINSRC) $(OUTPROTOSRC) util.cpp qm.cpp 
DBCLOBJ=$(DBCLSRC:.cpp=.o)

LIBS=$(VCEROOT)/libvce.a $(VCEROOT)/libz.a
MYSQLLIBS=./libmysqlclient_5.1.41_darwin_i386.a
MYSQLINC=/opt/local/include/mysql5/mysql
CFLAGS=-g -I. -I$(VCEROOT)  -I$(MYSQLINC) -I$(BOOST) -D_DEBUG -arch x86_64


all: dbsv dbcl

$(OUTSQL): $(DEF) dbgen.py template.sql
	python $(DEF)

$(OUTXML) : $(DEF) dbgen.py template.xml 
	python $(DEF)

$(OUTSVMAINSRC) $(OUTCLMAINSRC): $(DEF) dbgen.py template.xml template.sql svtemplate.cpp svtemplate.h cltemplate.h cltemplate.cpp
	python $(DEF)

$(OUTPROTOSRC) : dbproto.xml $(DEF)
	$(GEN) dbproto.xml

%.o:%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

dbsv: $(DBSVOBJ) $(LIBS)
	$(CC) $(CFLAGS) -pg -o dbsv $(DBSVOBJ) $(LIBS)  $(MYSQLLIBS)

dbcl: $(DBCLOBJ) $(LIBS)
	$(CC) $(CFLAGS) -pg -o dbcl $(DBCLOBJ) $(LIBS)  $(MYSQLLIBS)

tbl: $(OUTSQL)
	cat $(OUTSQL) | mysql5 -u storage

clean:
	rm -f *.o dbsv *~ *.o $(OUTPROTOSRC) *.pyc dbsvmain.cpp dbsvmain.h dbclmain.cpp dbclmain.h

depend: $(DBSVSRC) $(DBCLSRC)
	mv Makefile Makefile.bak
	sed -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(CC) $(CFLAGS) -MM $(GMSVSRC) $(AUTHSVSRC) $(DBSVSRC) $(LOGSVSRC) $(CLISRC) >> Makefile

# DO NOT DELETE THIS LINE
dbsvmain.o: dbsvmain.cpp ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h dbproto.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h \
  /opt/local/include/mysql5/mysql/my_global.h \
  /opt/local/include/mysql5/mysql/my_config.h \
  /opt/local/include/mysql5/mysql/my_attribute.h \
  /opt/local/include/mysql5/mysql/my_dbug.h \
  /opt/local/include/mysql5/mysql/mysql.h \
  /opt/local/include/mysql5/mysql/mysql_version.h \
  /opt/local/include/mysql5/mysql/mysql_com.h \
  /opt/local/include/mysql5/mysql/mysql_time.h \
  /opt/local/include/mysql5/mysql/my_list.h \
  /opt/local/include/mysql5/mysql/typelib.h \
  /opt/local/include/mysql5/mysql/my_alloc.h \
  /opt/local/include/mysql5/mysql/mysqld_error.h dbsvmain.h qm.h util.h
dbsvmain.o: dbsvmain.h dbproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h
dbproto.o: dbproto.cpp dbproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h
dbproto.o: dbproto.h ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h
util.o: util.cpp ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h util.h
qm.o: qm.cpp qm.h ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h \
  /opt/local/include/mysql5/mysql/my_global.h \
  /opt/local/include/mysql5/mysql/my_config.h \
  /opt/local/include/mysql5/mysql/my_attribute.h \
  /opt/local/include/mysql5/mysql/my_dbug.h \
  /opt/local/include/mysql5/mysql/mysql.h \
  /opt/local/include/mysql5/mysql/mysql_version.h \
  /opt/local/include/mysql5/mysql/mysql_com.h \
  /opt/local/include/mysql5/mysql/mysql_time.h \
  /opt/local/include/mysql5/mysql/my_list.h \
  /opt/local/include/mysql5/mysql/typelib.h \
  /opt/local/include/mysql5/mysql/my_alloc.h
