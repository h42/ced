CPPFLAGS=-g -Wall -static
#CC=g++

PROGS=ccinc jpd mat term_t
OBJS= undo.o term.o list.o file.o func1.o display.o history.o

all:ced.h $(PROGS) #getkb.o

nlist:nlist.cc
	g++ -Wall -g nlist.cc -onlist

jpd:jpd.cc $(OBJS)
	g++ $(CPPFLAGS) jpd.cc $(OBJS) -ojpd

ced.h:jpd.cc
	ccinc <jpd.cc >ced.h

func1.o :func1.cc ced.h
	g++ -c $(CPPFLAGS) func1.cc

undo.o undo.h:undo.cc ced.h
	g++ -c $(CPPFLAGS) undo.cc

display.o :display.cc ced.h
	g++ -c $(CPPFLAGS) display.cc

file.o file.h:file.cc
	g++ -c $(CPPFLAGS) file.cc
	ccinc <file.cc >file.h

term.o term.h:term.cc
	g++ -c $(CPPFLAGS) term.cc
	ccinc <term.cc >term.h

term_t:term_t.cc term.o

list.o:list.cc
	g++ -c $(CPPFLAGS) list.cc
	ccinc <list.cc >list.h

list_t:list_t.cc list.o

clean:
	-rm *.o $(PROGS)
