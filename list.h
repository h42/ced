#ifndef _LIST_H
#define _LIST_H

#include "vec.h"

enum OP {INSERT,DELETE};

const int LISTMAXUNDO = 50;

struct logrec {
    OP op;
    int pos;
    int bufl;
    char *buf;
    logrec() {bufl=0; buf=0;}
    ~logrec();
    void insert(OP, int, char *, int l=0);
};

struct node {
    node *fp;
    node *bp;
    char buf[];
};

class list {
public:
    list();
    ~list();
    void init();
    void reset();
    void del(int x);
    void ins(int x, const char *s, int len=0);
    void ins(const char *s, int len=0) {ins(-1,s,0);}
    void upd(int x, const char *s, int len=0); // NOT IMPLEMENTED
    char *get(int n);
    int  size() {return zsize;}
    void upd_log(OP iop, int ipos, char *ibuf, int ilen);
    void set_log_ptr(int x);
    void rollback(int p1);
    void log_on() {zlogit=1;}
    void log_off() {zlogit=0;}
private:
    int zsize,zpos;
    node *zcur;
    node zroot;
    int  zlogit;
    vec<logrec> zlog[LISTMAXUNDO];
    int zp1,zcnt;
};

#endif
