CPPFLAGS=-g -Wall -static -std=c++11
#CC=g++

PROGS=jpd list_t # mat term_t
OBJS=term.o list.o file.o func1.o func2.o find.o display.o history.o re.o\
     str.o glob.o undo.o log.o
INCS=ced.h file.h glob.h history.h  list.h re.h str.h term.h vec.h log.h

%.h : %.cc
	awkinc < $< > $@

all:$(INCS) $(PROGS) #getkb.o

ced.h:jpd.cc
	awkinc <jpd.cc >ced.h

nlist:nlist.cc

jpd:jpd.cc $(OBJS) $(INCS)
	g++ $(CPPFLAGS) jpd.cc $(OBJS) -ojpd

func1.o :func1.cc ced.h
func2.o :func2.cc ced.h
find.o :find.cc ced.h
undo.o undo.h:undo.cc ced.h
display.o :display.cc ced.h
file.o :file.cc ced.h
history.o:history.cc
term.o:term.cc
term_t:term_t.cc term.o
list.o:list.cc
list_t:list_t.cc list.o
re.o:re.cc
glob.o:glob.cc
vec.o:vec.cc

install:jpd
	install jpd /usr/local/bin/ced

clean:
	-rm *.o $(PROGS)
