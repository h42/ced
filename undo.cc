#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec.h"
#include "list.h"
#include "term.h"
#include "ced.h"

//ccinclude
#ifndef _UNDO_H
#define _UNDO_H

#define MAXUNDO 50

class ced;

class under {
public:
    under() {zused=0; zbuf=(char *)0; zbufl=0;}
    ~under() {}
    void get(ced&);
    void put(ced&,int type);
    void del(ced&, int d1, int d2);
//private:
    int     zused;
    char   *zbuf;
    int     zbufl;
    int     zbufsize;
    int     zcur;
    int     zdel1,zdlen;
    int     zedit,zedit2;
    int     zindent;
    int     zins;
    int     zx,zy,ztop,zoff;
    int     zkx1,zkx2,zky1,zky2,zkh;
};

class undo {
public:
    void init(ced *t) {zt=t; zp1=0; zcnt=0;}
    ~undo() {};
    void trace();
    void push(int type=0);
    void pop();
    void del(int d1, int dlen=1);
    void reset() {zp1=zcnt=0;}
private:
    ced *zt;
    under zunder[MAXUNDO];
    int zp1,zcnt;
};

#endif
//ccinclude

void undo::trace() {
    zt->dsp.clrscr();
    for (int i=0;i<zcnt;i++) {
	printf("i=%d zx=%d zy=%d\n",i,zunder[i].zx,zunder[i].zy);
    }
    getchar();
}

void undo::push(int type) {
    zt->dsp.clrscr();
    zt->zl.set_log_ptr(zp1);
    zunder[zp1].put(*zt, type);
    zp1++;
    if (zcnt<MAXUNDO) zcnt++;
    if (zp1>=MAXUNDO) zp1=0;
}

void undo::pop() {
    //zt->dsp.clrscr();
    if (zcnt<=0) return;
    zp1--;
    if (zp1<0) zp1=MAXUNDO - 1;
    zt->zl.rollback(zp1);
    zunder[zp1].get(*zt);
    zcnt--;
}

void under::put(ced &t, int type) {
    zdel1=zdlen=-1;
    zcur=t.zcur;
    zx=t.zx;
    zy=t.zy;
    ztop=t.ztop;
    zoff=t.zoff;
    zedit=t.zedit;
    zedit2=t.zedit2;
    zindent=t.zindent;
    zkh=t.zkh;
    zkx1=t.zkx1;
    zkx2=t.zkx2;
    zky1=t.zky1;
    zky2=t.zky2;

    if (1) {
        // we never shrink buffer even if undo state has smaller buffer for efficiency
        if (zbufsize<t.zbufsize) {
	    if (zbuf) zbuf=(char *)realloc(zbuf,t.zbufsize);
	    else zbuf=(char *)malloc(t.zbufsize);
	    zbufsize=t.zbufsize;
	}
        memcpy(zbuf,t.zbuf,t.zbufsize);
	zbufl=t.zbufl;
    }
    zused |= 1;
}

void under::get(ced &t) {
    t.zcur=zcur;
    t.zx=zx;
    t.zy=zy;
    t.ztop=ztop;
    t.zoff=zoff;
    t.zedit=zedit;
    t.zedit2=zedit2;
    t.zindent=zindent;
    t.zkh=zkh;
    t.zkx1=zkx1;
    t.zkx2=zkx2;
    t.zky1=zky1;
    t.zky2=zky2;
    if (zbuf) memcpy(t.zbuf,zbuf,zbufsize);
    t.zbufl=zbufl;
}
