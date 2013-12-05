#include <stdlib.h>
#include <string.h>
#include <malloc.h>

//ccinclude
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
//ccinclude

//
// LOG FUNCS
//

void list::set_log_ptr(int x) {
    zp1=x;
    vec<logrec> *v=&zlog[zp1];
    v->sp(0);
}

void list::upd_log(OP iop, int ipos, char *ibuf, int ilen) {
    if (zlogit) {
        if (zp1<0 || zp1>=LISTMAXUNDO) return;
        vec<logrec> *v=&zlog[zp1];
        int sp = (*v).sp();
        if (v->size()<=sp) v->grow();
        if (iop==DELETE) (*v)[sp].insert(DELETE,ipos,ibuf,ilen);
        else if (iop==INSERT) (*v)[sp].insert(INSERT,ipos,ibuf,ilen);
        (*v).sp(sp+1);
    }
}

void list::rollback(int p1) {
    vec<logrec> *v=&zlog[p1];
    int save=zlogit;
    for (int i=(*v).sp()-1; i>=0; i--) {
        logrec *rb= &(*v)[i];
        if (rb->op==INSERT) ins(rb->pos, rb->buf, rb->bufl);
        else if (rb->op==DELETE) del(rb->pos);
    }
    (*v).sp(0);
    zlogit=save;
};

logrec::~logrec() {
    if (buf) free(buf);
}

void logrec::insert(OP iop, int ipos, char *ibuf, int ilen) {
    op=iop; // HANDLES COMMIT CASE
    if (iop==DELETE) pos=ipos;
    else if (iop==INSERT) {
        pos=ipos;
        if (ilen<=0) ilen=strlen(ibuf)+1;
        if (ilen>bufl) {
            if (!buf) buf=(char *)malloc(ilen);
            else buf=(char *)realloc(buf,ilen);
            bufl=ilen;
        }
        memcpy(buf,ibuf,bufl);
    }
}


//
// LIST FUNCS
//

node *newNode(int l) {
    node *n;
    n = (node *)malloc(sizeof(node)+l+1);
    return n;
}

char *list::get(int x) {
    int i;
    node *cur;
    if (x<0 || x>=zsize) return 0;
    int d1=x, d2=zsize-x, d3=abs(zpos-x);
    //d3=9999999;
    //d1<d3 if zpos<0
    if (d1<=d2 && d1<=d3) {
	for (i=0,cur=zroot.fp; i<=x; i++,cur=cur->fp) zcur=cur;
    }
    else if (d2<=d1 && d2<=d3) {
	for (i=zsize-1,cur=zroot.bp; i>=x; i--,cur=cur->bp) zcur=cur;
    }
    else if (zpos<=x) {
	for (i=zpos,cur=zcur; i<=x; i++,cur=cur->fp) zcur=cur;
    }
    else {
	for (i=zpos,cur=zcur; i>=x; i--,cur=cur->bp) zcur=cur;
    }
    zpos=x;
    return zcur->buf;
}

void list::del(int x) {
    node *cur;
    if (x<0 || x>=zsize) return;
    char *buf=get(x);
    if (zlogit && buf) upd_log(INSERT,x,buf,0);
    cur = zcur;
    cur->bp->fp = cur->fp;
    cur->fp->bp = cur->bp;
    zsize--;
    zcur = cur->fp;
    free(cur);
    if (zsize<1 || x >= zsize) zpos=-1;
    else zpos = x;
}

void list::ins(int x, const char *s, int len) {
    node *cur;
    if (x<0) x=zsize;
    if (x>0) get(x-1);
    else zcur = &zroot;
    if (zlogit) upd_log(DELETE,x,0,0);
    cur=zcur;
    if (len<=0) len=strlen((char *)s);
    node *n1 = newNode(len);
    strcpy((char *)n1->buf,(char *)s);
    n1->fp = cur->fp;
    n1->bp = cur;
    n1->fp->bp = n1;
    cur->fp = n1;
    zsize++;
    zcur=n1;
    zpos=x;
}

void list::reset() {
    int n=zsize;
    for (int i=0; i<n; i++) del(0);
    init();
}
void list::init() {
    zsize=0;
    zpos=-1;
    zcur=&zroot;
    zcur->buf[0]=0;
    zroot.fp=zcur;
    zroot.bp=zcur;
    zlogit=0;
    zp1=zcnt=0;
    return;
}

list::list() {
    init();
}

list::~list() {
    reset();
}
