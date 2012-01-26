# Makefile to build the SDL tests

srcdir  = .

CC      = gcc
EXE	= 
CFLAGS  = -arch x86_64 -I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_THREAD_SAFE -DHAVE_OPENGL
LIBS	=  -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa

TARGETS = checkkeys$(EXE) graywin$(EXE) loopwave$(EXE) testalpha$(EXE) testbitmap$(EXE) testblitspeed$(EXE) testcdrom$(EXE) testcursor$(EXE) testdyngl$(EXE) testerror$(EXE) testfile$(EXE) testgamma$(EXE) testgl$(EXE) testhread$(EXE) testiconv$(EXE) testjoystick$(EXE) testkeys$(EXE) testlock$(EXE) testoverlay2$(EXE) testoverlay$(EXE) testpalette$(EXE) testplatform$(EXE) testsem$(EXE) testsprite$(EXE) testtimer$(EXE) testver$(EXE) testvidinfo$(EXE) testwin$(EXE) testwm$(EXE) threadwin$(EXE) torturethread$(EXE) testloadso$(EXE)

all: $(TARGETS)

checkkeys$(EXE): $(srcdir)/checkkeys.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

graywin$(EXE): $(srcdir)/graywin.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

loopwave$(EXE): $(srcdir)/loopwave.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testalpha$(EXE): $(srcdir)/testalpha.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS) 

testbitmap$(EXE): $(srcdir)/testbitmap.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testblitspeed$(EXE): $(srcdir)/testblitspeed.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testcdrom$(EXE): $(srcdir)/testcdrom.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testcursor$(EXE): $(srcdir)/testcursor.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testdyngl$(EXE): $(srcdir)/testdyngl.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testerror$(EXE): $(srcdir)/testerror.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testfile$(EXE): $(srcdir)/testfile.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testgamma$(EXE): $(srcdir)/testgamma.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS) 

testgl$(EXE): $(srcdir)/testgl.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS) -L/usr/X11/lib -Wl,-framework,OpenGL 

testhread$(EXE): $(srcdir)/testhread.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testiconv$(EXE): $(srcdir)/testiconv.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testjoystick$(EXE): $(srcdir)/testjoystick.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testkeys$(EXE): $(srcdir)/testkeys.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testlock$(EXE): $(srcdir)/testlock.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testoverlay2$(EXE): $(srcdir)/testoverlay2.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testoverlay$(EXE): $(srcdir)/testoverlay.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testpalette$(EXE): $(srcdir)/testpalette.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS) 

testplatform$(EXE): $(srcdir)/testplatform.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testsem$(EXE): $(srcdir)/testsem.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testsprite$(EXE): $(srcdir)/testsprite.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS) 

testtimer$(EXE): $(srcdir)/testtimer.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testver$(EXE): $(srcdir)/testver.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testvidinfo$(EXE): $(srcdir)/testvidinfo.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testwin$(EXE): $(srcdir)/testwin.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testwm$(EXE): $(srcdir)/testwm.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

threadwin$(EXE): $(srcdir)/threadwin.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

torturethread$(EXE): $(srcdir)/torturethread.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)

testloadso$(EXE): $(srcdir)/testloadso.c
	$(CC) -o $@ $? $(CFLAGS) $(LIBS)


clean:
	rm -f $(TARGETS)

distclean: clean
	rm -f Makefile
	rm -f config.status config.cache config.log
	rm -rf $(srcdir)/autom4te*
