
CC=g++
VCEROOT=../../vce/darwin_special_version/r8644

GMSVSRC=movable.cpp floor.cpp sv.cpp zone.cpp gmsvmain.cpp util.cpp ../proto/kproto.cpp ../dbsv/dbproto.cpp
GMSVOBJ=$(GMSVSRC:.cpp=.o)
AUTOCLISRC=climain.cpp ../proto/kproto.cpp util.cpp
AUTOCLIOBJ=$(AUTOCLISRC:.cpp=.o)

LIBS=$(VCEROOT)/libvce.a $(VCEROOT)/libz.a
MYSQLLIBS=./libmysqlclient_5.1.41_darwin_i386.a
CFLAGS=-g -I. -I$(VCEROOT) -I../../boost/boost_1_41_0 -I../dbsv -I../proto -I/opt/local/include/mysql5/mysql  -D_DEBUG -arch x86_64
GEN=$(VCEROOT)/gen

all: gmsv autocli

../proto/kproto.cpp ../proto/kproto.h: ../proto/k.xml
	$(GEN) ../proto/k.xml

%.o:%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

gmsv: $(GMSVOBJ) $(LIBS) 
	$(CC) $(CFLAGS) -pg -o gmsv $(GMSVOBJ) $(LIBS) 

autocli: $(AUTOCLIOBJ) $(LIBS)
	$(CC) $(CFLAGS) -pg -o autocli $(AUTOCLISRC) $(LIBS)

clean:
	rm -f *.o gmsv  *~ ../proto/*.o ../proto/*.h ../proto/*.cpp  
maketable:
	cat create_table.sql | mysql5 -S /tmp/mysql.sock -u storage

depend: $(GMSVSRC) $(AUTOCLISRC)
	mv Makefile Makefile.bak
	sed -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(CC) $(CFLAGS) -MM $(GMSVSRC) >> Makefile

# DO NOT DELETE THIS LINE
movable.o: movable.cpp movable.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h floor.h \
  ../proto/kproto.h ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h common.h sv.h \
  ../dbsv/dbproto.h ../../vce/darwin_special_version/r8644/vce2.h \
  gmsvmain.h id.h util.h
floor.o: floor.cpp floor.h ../proto/kproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h common.h \
  ../../vce/darwin_special_version/r8644/vce2.h sv.h ../dbsv/dbproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h gmsvmain.h id.h util.h \
  movable.h zone.h
sv.o: sv.cpp ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h util.h sv.h \
  ../proto/kproto.h ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h \
  ../dbsv/dbproto.h ../../vce/darwin_special_version/r8644/vce2.h \
  common.h gmsvmain.h id.h floor.h movable.h zone.h
zone.o: zone.cpp zone.h ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h util.h common.h \
  ../proto/kproto.h ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h
gmsvmain.o: gmsvmain.cpp ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h ../proto/kproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h \
  ../dbsv/dbproto.h ../../vce/darwin_special_version/r8644/vce2.h \
  gmsvmain.h id.h util.h sv.h common.h floor.h movable.h zone.h
util.o: util.cpp ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h util.h \
  ../../boost/boost_1_41_0/boost/functional/hash.hpp \
  ../../boost/boost_1_41_0/boost/functional/hash/hash.hpp \
  ../../boost/boost_1_41_0/boost/functional/hash/hash_fwd.hpp \
  ../../boost/boost_1_41_0/boost/config.hpp \
  ../../boost/boost_1_41_0/boost/config/user.hpp \
  ../../boost/boost_1_41_0/boost/config/select_compiler_config.hpp \
  ../../boost/boost_1_41_0/boost/config/compiler/gcc.hpp \
  ../../boost/boost_1_41_0/boost/config/select_stdlib_config.hpp \
  ../../boost/boost_1_41_0/boost/config/no_tr1/utility.hpp \
  ../../boost/boost_1_41_0/boost/config/stdlib/libstdcpp3.hpp \
  ../../boost/boost_1_41_0/boost/config/select_platform_config.hpp \
  ../../boost/boost_1_41_0/boost/config/platform/macos.hpp \
  ../../boost/boost_1_41_0/boost/config/posix_features.hpp \
  ../../boost/boost_1_41_0/boost/config/suffix.hpp \
  ../../boost/boost_1_41_0/boost/detail/workaround.hpp \
  ../../boost/boost_1_41_0/boost/functional/hash/detail/hash_float.hpp \
  ../../boost/boost_1_41_0/boost/functional/hash/detail/float_functions.hpp \
  ../../boost/boost_1_41_0/boost/config/no_tr1/cmath.hpp \
  ../../boost/boost_1_41_0/boost/functional/hash/detail/limits.hpp \
  ../../boost/boost_1_41_0/boost/limits.hpp \
  ../../boost/boost_1_41_0/boost/integer/static_log2.hpp \
  ../../boost/boost_1_41_0/boost/cstdint.hpp \
  ../../boost/boost_1_41_0/boost/assert.hpp \
  ../../boost/boost_1_41_0/boost/functional/hash/detail/hash_float_generic.hpp \
  ../../boost/boost_1_41_0/boost/functional/hash/extensions.hpp \
  ../../boost/boost_1_41_0/boost/detail/container_fwd.hpp
kproto.o: ../proto/kproto.cpp ../proto/kproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h
dbproto.o: ../dbsv/dbproto.cpp ../dbsv/dbproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h
