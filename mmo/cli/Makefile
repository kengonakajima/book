#
# 注意： vceと zlibを "-arch i386 -arch x86_64"でユニバーサルにしておかないとSDLビルドできない。 SDLはx86_64でしかspriteが動かない。
#
#


CC=g++

CLISRC=../gmsv/zone.cpp font.cpp kcli.cpp sprite.cpp app.cpp sdlmain.cpp util.cpp ../proto/kproto.cpp
CLIOBJ=$(CLISRC:.cpp=.o)

VCEROOT=../../vce/darwin_special_version/r8644
SDLROOT=../../SDL-1.2.14

LIBS=$(VCEROOT)/libvce.a $(VCEROOT)/libz.a 


LIBFLAGS=-L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa
CFLAGS=-arch x86_64 -g -I. -I../proto -I../../boost/boost_1_41_0 -I$(VCEROOT) -I$(SDLROOT)/include -D_DEBUG 
GEN=$(VCEROOT)/gen

all: cli

sprite: sprite.cpp
	g++ -arch x86_64 -g -O2 -I$(VCEROOT) -I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_THREAD_SAFE -DHAVE_OPENGL -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa sprite.cpp -o sprite $(LIBS)


../proto/kproto.cpp ../proto/kproto.h: ../proto/k.xml
	$(GEN) ../proto/k.xml


%.o:%.cpp
	$(CC) -c $(CFLAGS) $< -o $@


cli: $(CLIOBJ) $(LIBS)
	$(CC) $(CFLAGS) -pg -o cli $(CLIOBJ) $(LIBFLAGS) $(LIBS) 

clean:
	rm -f *.o cli *~ ../proto/*.o ../proto/*.h ../proto/*.cpp

maketable:
	cat create_table.sql | mysql5 -S /tmp/mysql.sock -u storage

depend: $(CLISRC)
	mv Makefile Makefile.bak
	sed -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(CC) $(CFLAGS) -MM $(CLISRC) >> Makefile

# DO NOT DELETE THIS LINE
zone.o: ../gmsv/zone.cpp ../gmsv/zone.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h ../gmsv/util.h \
  ../gmsv/common.h ../proto/kproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h
font.o: font.cpp font.h ../../SDL-1.2.14/include/SDL.h \
  ../../SDL-1.2.14/include/SDL_main.h \
  ../../SDL-1.2.14/include/SDL_stdinc.h \
  ../../SDL-1.2.14/include/SDL_config.h \
  ../../SDL-1.2.14/include/SDL_platform.h \
  ../../SDL-1.2.14/include/begin_code.h \
  ../../SDL-1.2.14/include/close_code.h \
  ../../SDL-1.2.14/include/SDL_audio.h \
  ../../SDL-1.2.14/include/SDL_error.h \
  ../../SDL-1.2.14/include/SDL_endian.h \
  ../../SDL-1.2.14/include/SDL_mutex.h \
  ../../SDL-1.2.14/include/SDL_thread.h \
  ../../SDL-1.2.14/include/SDL_rwops.h \
  ../../SDL-1.2.14/include/SDL_cdrom.h \
  ../../SDL-1.2.14/include/SDL_cpuinfo.h \
  ../../SDL-1.2.14/include/SDL_events.h \
  ../../SDL-1.2.14/include/SDL_active.h \
  ../../SDL-1.2.14/include/SDL_keyboard.h \
  ../../SDL-1.2.14/include/SDL_keysym.h \
  ../../SDL-1.2.14/include/SDL_mouse.h \
  ../../SDL-1.2.14/include/SDL_video.h \
  ../../SDL-1.2.14/include/SDL_joystick.h \
  ../../SDL-1.2.14/include/SDL_quit.h \
  ../../SDL-1.2.14/include/SDL_loadso.h \
  ../../SDL-1.2.14/include/SDL_timer.h \
  ../../SDL-1.2.14/include/SDL_version.h
kcli.o: kcli.cpp ../gmsv/common.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h ../proto/kproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h kcli.h app.h \
  sprite.h ../../SDL-1.2.14/include/SDL.h \
  ../../SDL-1.2.14/include/SDL_main.h \
  ../../SDL-1.2.14/include/SDL_stdinc.h \
  ../../SDL-1.2.14/include/SDL_config.h \
  ../../SDL-1.2.14/include/SDL_platform.h \
  ../../SDL-1.2.14/include/begin_code.h \
  ../../SDL-1.2.14/include/close_code.h \
  ../../SDL-1.2.14/include/SDL_audio.h \
  ../../SDL-1.2.14/include/SDL_error.h \
  ../../SDL-1.2.14/include/SDL_endian.h \
  ../../SDL-1.2.14/include/SDL_mutex.h \
  ../../SDL-1.2.14/include/SDL_thread.h \
  ../../SDL-1.2.14/include/SDL_rwops.h \
  ../../SDL-1.2.14/include/SDL_cdrom.h \
  ../../SDL-1.2.14/include/SDL_cpuinfo.h \
  ../../SDL-1.2.14/include/SDL_events.h \
  ../../SDL-1.2.14/include/SDL_active.h \
  ../../SDL-1.2.14/include/SDL_keyboard.h \
  ../../SDL-1.2.14/include/SDL_keysym.h \
  ../../SDL-1.2.14/include/SDL_mouse.h \
  ../../SDL-1.2.14/include/SDL_video.h \
  ../../SDL-1.2.14/include/SDL_joystick.h \
  ../../SDL-1.2.14/include/SDL_quit.h \
  ../../SDL-1.2.14/include/SDL_loadso.h \
  ../../SDL-1.2.14/include/SDL_timer.h \
  ../../SDL-1.2.14/include/SDL_version.h font.h ../gmsv/zone.h \
  ../gmsv/util.h ../gmsv/common.h
sprite.o: sprite.cpp sprite.h ../gmsv/common.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h ../proto/kproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h \
  ../../SDL-1.2.14/include/SDL.h ../../SDL-1.2.14/include/SDL_main.h \
  ../../SDL-1.2.14/include/SDL_stdinc.h \
  ../../SDL-1.2.14/include/SDL_config.h \
  ../../SDL-1.2.14/include/SDL_platform.h \
  ../../SDL-1.2.14/include/begin_code.h \
  ../../SDL-1.2.14/include/close_code.h \
  ../../SDL-1.2.14/include/SDL_audio.h \
  ../../SDL-1.2.14/include/SDL_error.h \
  ../../SDL-1.2.14/include/SDL_endian.h \
  ../../SDL-1.2.14/include/SDL_mutex.h \
  ../../SDL-1.2.14/include/SDL_thread.h \
  ../../SDL-1.2.14/include/SDL_rwops.h \
  ../../SDL-1.2.14/include/SDL_cdrom.h \
  ../../SDL-1.2.14/include/SDL_cpuinfo.h \
  ../../SDL-1.2.14/include/SDL_events.h \
  ../../SDL-1.2.14/include/SDL_active.h \
  ../../SDL-1.2.14/include/SDL_keyboard.h \
  ../../SDL-1.2.14/include/SDL_keysym.h \
  ../../SDL-1.2.14/include/SDL_mouse.h \
  ../../SDL-1.2.14/include/SDL_video.h \
  ../../SDL-1.2.14/include/SDL_joystick.h \
  ../../SDL-1.2.14/include/SDL_quit.h \
  ../../SDL-1.2.14/include/SDL_loadso.h \
  ../../SDL-1.2.14/include/SDL_timer.h \
  ../../SDL-1.2.14/include/SDL_version.h
app.o: app.cpp ../../SDL-1.2.14/include/SDL.h \
  ../../SDL-1.2.14/include/SDL_main.h \
  ../../SDL-1.2.14/include/SDL_stdinc.h \
  ../../SDL-1.2.14/include/SDL_config.h \
  ../../SDL-1.2.14/include/SDL_platform.h \
  ../../SDL-1.2.14/include/begin_code.h \
  ../../SDL-1.2.14/include/close_code.h \
  ../../SDL-1.2.14/include/SDL_audio.h \
  ../../SDL-1.2.14/include/SDL_error.h \
  ../../SDL-1.2.14/include/SDL_endian.h \
  ../../SDL-1.2.14/include/SDL_mutex.h \
  ../../SDL-1.2.14/include/SDL_thread.h \
  ../../SDL-1.2.14/include/SDL_rwops.h \
  ../../SDL-1.2.14/include/SDL_cdrom.h \
  ../../SDL-1.2.14/include/SDL_cpuinfo.h \
  ../../SDL-1.2.14/include/SDL_events.h \
  ../../SDL-1.2.14/include/SDL_active.h \
  ../../SDL-1.2.14/include/SDL_keyboard.h \
  ../../SDL-1.2.14/include/SDL_keysym.h \
  ../../SDL-1.2.14/include/SDL_mouse.h \
  ../../SDL-1.2.14/include/SDL_video.h \
  ../../SDL-1.2.14/include/SDL_joystick.h \
  ../../SDL-1.2.14/include/SDL_quit.h \
  ../../SDL-1.2.14/include/SDL_loadso.h \
  ../../SDL-1.2.14/include/SDL_timer.h \
  ../../SDL-1.2.14/include/SDL_version.h ../proto/kproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h ../gmsv/common.h \
  ../../vce/darwin_special_version/r8644/vce2.h kcli.h \
  ../../vce/darwin_special_version/r8644/vce2.h util.h app.h sprite.h \
  font.h ../gmsv/zone.h ../gmsv/util.h ../gmsv/common.h
sdlmain.o: sdlmain.cpp ../../SDL-1.2.14/include/SDL.h \
  ../../SDL-1.2.14/include/SDL_main.h \
  ../../SDL-1.2.14/include/SDL_stdinc.h \
  ../../SDL-1.2.14/include/SDL_config.h \
  ../../SDL-1.2.14/include/SDL_platform.h \
  ../../SDL-1.2.14/include/begin_code.h \
  ../../SDL-1.2.14/include/close_code.h \
  ../../SDL-1.2.14/include/SDL_audio.h \
  ../../SDL-1.2.14/include/SDL_error.h \
  ../../SDL-1.2.14/include/SDL_endian.h \
  ../../SDL-1.2.14/include/SDL_mutex.h \
  ../../SDL-1.2.14/include/SDL_thread.h \
  ../../SDL-1.2.14/include/SDL_rwops.h \
  ../../SDL-1.2.14/include/SDL_cdrom.h \
  ../../SDL-1.2.14/include/SDL_cpuinfo.h \
  ../../SDL-1.2.14/include/SDL_events.h \
  ../../SDL-1.2.14/include/SDL_active.h \
  ../../SDL-1.2.14/include/SDL_keyboard.h \
  ../../SDL-1.2.14/include/SDL_keysym.h \
  ../../SDL-1.2.14/include/SDL_mouse.h \
  ../../SDL-1.2.14/include/SDL_video.h \
  ../../SDL-1.2.14/include/SDL_joystick.h \
  ../../SDL-1.2.14/include/SDL_quit.h \
  ../../SDL-1.2.14/include/SDL_loadso.h \
  ../../SDL-1.2.14/include/SDL_timer.h \
  ../../SDL-1.2.14/include/SDL_version.h app.h sprite.h ../gmsv/common.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2config.h \
  ../../vce/darwin_special_version/r8644/vce2types.h \
  ../../vce/darwin_special_version/r8644/vce2wchar.h \
  ../../vce/darwin_special_version/r8644/vce2defs.h \
  ../../vce/darwin_special_version/r8644/vce2util.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2log.h ../proto/kproto.h \
  ../../vce/darwin_special_version/r8644/vce2.h \
  ../../vce/darwin_special_version/r8644/vce2serialize.h font.h kcli.h \
  ../gmsv/zone.h ../gmsv/util.h ../gmsv/common.h
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
