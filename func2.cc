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
    else if (c==4   || c=='d' || c=='D') ctrl_kd();
    else if (c==8   || c=='h' || c=='H') {if (zkh) zkh=0; else zkh=check_k();}
    else if (c==11  || c=='k' || c=='K') ctrl_kk();
    else if (c==12  || c=='l' || c=='L') ctrl_kl();
    else if (c==20  || c=='t' || c=='T')
        snprintf(zmsg,sizeof(zmsg),"in_k=%d",in_k());
    else if (c==22   || c=='v' || c=='V') ctrl_kv();
}

int ced::check_k() {
    if (zkx1<0 || zkx2<0) return 0;
    if (zky2>zky1 && zky1>=0) return 2;
    if (zky1==zky2 && zkx1==0 && zkx2==HIGH) return 3;
    if (zky1==zky2 && zkx1<zkx2) return 1;
    return 0;
}

void ced::k_del() {
    int k=check_k();
    if (k==0 || (k!=2 && zy==zky1)) return init_k();
    if (zky1>zy) {
        --zky1;
        --zky2;
        return;
    }
    if (zy>=zky1 && zy<=zky2) {
        zky2--;
        if (zky1==zky2) {
            zkh=3;
            zkx1=0;
            zkx2=HIGH;
        }
    }
}

void ced::k_del_char() {
    if (zx<zbufl) {
        if (zkh==1 && zy==zky1) {
            if (zx<zkx1) {
                zkx1--;
                zkx2--;
            }
            else if (zx<=zkx2) zkx2--;
        }
        return;
    }
    if (zkh==1 && zy==zky1-1) {
        zky1=zky2=zy;
        zkx1+=zx;
        zkx2+=zx;
        return;
    }
    if (zkh<2) return;
    if (zy<zky1) {
        zky1--;
        zky2--;
    }
    else if (zy<zky2) zky2--;
}

void ced::k_ins_char() {
    if (zkh!=1 || zky1 != zy) return;
    if (zx>=zkx1 && zx<=zkx2) zkx2++;
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
    pline();
    if (zkh<1 || (zkh==1 && in_k() && zx != zkx1) || (zkh>1 && in_k() && zy != zky2)) {
        snprintf(zmsg,sizeof(zmsg),"nothing copied");
        return;
    }
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
        return;
    }
    list ll2;
    int i;
    char *buf;
    zedit=1;
    l=zky2-zky1+1;
    ll2.init();
    for (i=zky1;i<=zky2;i++) {
        buf=ll.get(i);
        ll2.ins(buf);
    }
    for (i=0;i<l;i++) {
        buf=ll2.get(i);
        ll.ins(i+zy+1,buf);
    }
    zky1=zy+1;zky2=zky1+l-1;
    disppage(ztop);
}

void ced::ctrl_kd() {
    if (zkh==0) {
        snprintf(zmsg,sizeof(zmsg),"nothing deleted");
        return;
    }
    pline();
    if (zkh==1) {
        int i, l=zkx2-zkx1+1, oy=zy;
        zy=zky1;
        gline(1);
        for (i=zkx1; i<zbufl-l; i++) zbuf[i]=zbuf[i+l];
        memset(&zbuf[zbufl-l],' ',l);
        zbufl-=l;
        zkh=0;
        if (zy==oy) {
            if (zx>zkx2) zx-=l;
            else if (zx>zkx1) zx = zkx1;
        }
        pline();
        zy=oy;
        disppage(ztop);
        return;
    }
    int l=zky2-zky1+1;
    for (int i=0;i<l;i++) ll.del(zky1);
    zkh=0;
    if (zy>zky1 && zy<=zky2) zy=zky1;
    if (zy>zky2) zy-=l;
    disppage(ztop);
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

void ced::ctrl_kv() {
    int l;
    pline();
    if (zkh<1 || (in_k() || (zky1>1 && zy==zky1-1)) ) {
        snprintf(zmsg,sizeof(zmsg),"nothing moved");
        return;
    }
    if (zkh==1) {
        int i,yy=zy;
        zy=zky1;
        gline(1);
        if (zkx2>=zbufl) {
            zy=yy;
	    snprintf(zmsg,sizeof(zmsg),"nothing moved");
	    return;
	}
        l=zkx2-zkx1+1;
        char *cp=(char *)alloca(l);
        memcpy(cp,&zbuf[zkx1],l);
        for (i=zkx1;i<zbufl-l;i++) zbuf[i]=zbuf[i+l];
        zbufl-=l;
        memset(&zbuf[zbufl],' ',l);
        pline();
        zy=yy;
        gline(1);
        if (zx>=zbufl) {
            memset(&zbufl,' ',zx-zbufl);
            zbufl+=zx-zbufl;
        }
        for (i=zbufl+l-1; i>=zx+l; i--) zbuf[i]=zbuf[i-l];
        memcpy(&zbuf[zx],cp,l);
	zbufl+=l;
        zkx1=zx; zkx2=zkx1+l-1; zky1=zky2=zy;
        disppage(ztop);
        return;
    }

    list ll2;
    int i;
    char *buf;
    zedit=1;
    l=zky2-zky1+1;
    ll2.init();
    for (i=zky1;i<=zky2;i++) {
        buf=ll.get(i);
        ll2.ins(buf);
    }
    for (i=0;i<l;i++) {
        buf=ll2.get(i);
        ll.ins(i+zy+1,buf);
    }
    if (zy<zky1) {
        for (i=0;i<l;i++) ll.del(zky1 + l);
        zky1=zy+1; zky2=zky1+l-1;
    }
    else {
        for (i=0;i<l;i++) ll.del(zky1);
        zky1=zy+1-l; zky2=zky1+l-1;
    }
    disppage(ztop);
}

void ced::disp_k() {
    snprintf(zmsg,sizeof(zmsg),"%d (%d,%d) (%d,%d)",
        zkh,zkx1,zky1,zkx2,zky2);
}

void ced::init_k() {
    zkh=0;
    zkx1=zkx2=zky1=zky2=-1;
}