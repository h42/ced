#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include "list.h"
#include "term.h"
#include "file.h"
#include "ced.h"

#define HIGH  999999
void ced::ctrl_k() {
    int c = dsp.get();
    if (c==2  || c=='b' || c=='B') ctrl_kb();
    else if (c==3   || c=='c' || c=='C') ctrl_kc();
    else if (c==11  || c=='k' || c=='K') ctrl_kk();
    else if (c==12  || c=='l' || c=='L') ctrl_kl();
    else if (c==20  || c=='t' || c=='T')
        snprintf(zmsg,sizeof(zmsg),"in_k=%d",in_k());
}

int ced::check_k() {
    if (zkx1<0 || zkx2<0) return 0;
    if (zky2>zky1 && zky1>=0) return 2;
    if (zky1==zky2 && zkx1==0 && zkx2==HIGH) return 3;
    if (zky1==zky2 && zkx1<zkx2) return 1;
    return 0;
}

bool ced::in_k(int x, int y) {
    if (x<0) x=zx;
    if (y<0) y=zy;
    if (!zkh || y<zky1 || y>zky2) return false;
    if (zkh==1 && (x<zkx1 || x>zkx2)) return false;
    return true;
}

void ced::ctrl_kb() {
    zkx1=zx;
    zky1=zy;
    zkh=check_k();
    disppage(ztop);
    disp_k();
}

void ced::ctrl_kc() {
    int l;
    if (zkh<1 || (zkh==1 && in_k() && zx != zkx1) ) {
        snprintf(zmsg,sizeof(zmsg),"nothing copied");
        return;
    }
    pline();
    if (zkh==1) {
        gline2(zky1);
        l=zkx2-zkx1+1;
        char *cp=(char *)alloca(l);
        memcpy(cp,&zbuf2[zkx1],l);
	gline(1);
	memcpy(zbuf2,zbuf,zbufl);
        memcpy(&zbuf[zx],cp,l);
	memcpy(&zbuf[zx+l],&zbuf2[zx],zbufl-zx);
	zbufl+=l;
        zkx1=zx; zkx2=zkx1+l-1; zky1=zky2=zy;
	disppage(ztop);
    }
}

void ced::ctrl_kk() {
    zkx2=zx;
    zky2=zy;
    zkh=check_k();
    disppage(ztop);
    disp_k();
}

void ced::ctrl_kl() {
    zkx1=0;
    zky1=zy;
    zkx2=HIGH;
    zky2=zy;
    if ((zkh=check_k())>0) disppage(ztop);
    disp_k();
}

void ced::disp_k() {
    snprintf(zmsg,sizeof(zmsg),"%d (%d,%d) (%d,%d)",
        zkh,zkx1,zky1,zkx2,zky2);
}

