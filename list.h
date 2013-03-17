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
