CPPFLAGS=-g -Wall -static
#CC=g++

PROGS=jpd mat term_t
OBJS=term.o list.o file.o func1.o func2.o display.o history.o

all:history.h file.h term.h ced.h $(PROGS) #getkb.o

ced.h:jpd.cc
	awkinc <jpd.cc >ced.h

file.h:file.cc
	awkinc <file.cc >file.h

term.h:term.cc
	awkinc <term.cc >term.h

history.h:history.cc
	awkinc <history.cc >history.h

nlist:nlist.cc
	g++ -Wall -g nlist.cc -onlist

jpd:jpd.cc $(OBJS)
	g++ $(CPPFLAGS) jpd.cc $(OBJS) -ojpd

func1.o :func1.cc ced.h
	g++ -c $(CPPFLAGS) func1.cc

func2.o :func2.cc ced.h
	g++ -c $(CPPFLAGS) func2.cc

undo.o undo.h:undo.cc ced.h
	g++ -c $(CPPFLAGS) undo.cc

display.o :display.cc ced.h
	g++ -c $(CPPFLAGS) display.cc

file.o :file.cc
	g++ -c $(CPPFLAGS) file.cc

history.o:history.cc
	g++ -c $(CPPFLAGS) history.cc

term.o:term.cc
	g++ -c $(CPPFLAGS) term.cc

term_t:term_t.cc term.o

list.o:list.cc
	g++ -c $(CPPFLAGS) list.cc

list_t:list_t.cc list.o

install:jpd
	install jpd /usr/local/bin/ced

clean:
	-rm *.o $(PROGS)
