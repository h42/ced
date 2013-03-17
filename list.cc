#include <stdlib.h>
#include <string.h>
#include <malloc.h>
//#include <unistd.h>

//ccinclude
#ifndef _LIST_H
#define _LIST_H

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
    void upd(int x, const char *s, int len=0);
    char *get(int n);
    int  size() {return zsize;}
private:
    int zsize,zpos;
    node *zcur;
    node zroot;
};

#endif
//ccinclude

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
    get(x);
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
    return;
}

list::list() {
    init();
}

list::~list() {
    reset();
}
