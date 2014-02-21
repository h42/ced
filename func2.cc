#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <ctype.h>
#include "list.h"
#include "term.h"
#include "ced.h"

#define HIGH  999999

void set_indent(ced *t);

void ced::ctrl_x() {
    int c = dsp.get();
    if (c==6  || c=='f') find();
    else if (c=='F') find(1);
    else if (c==3 || c=='c' || c=='C') change();
    else if (c==9 || c=='i' || c=='I') set_indent(this);
    else if (c==24 || c=='x' || c=='X') ctrl_xx();
}

void set_indent(ced *t) {
    t->zindent = t->zindent ? 0 : 1;
    snprintf(t->zmsg, sizeof(t->zmsg), "zindent %s", t->zindent ? "on" : "off");
}

void ced::ctrl_xx() {
    gline(1);
    char c=zbuf[zx];
    if (islower(c)) c= toupper(c);
    else c=tolower(c);
    add_char(c);
}

void ced::ctrl_k() {
    int c = dsp.get();
    if (c==2  || c=='b' || c=='B') ctrl_kb();
    else if (c==3   || c=='c' || c=='C') ctrl_kc();
    else if (c==4   || c=='d' || c=='D') ctrl_kd();
    else if (c==8   || c=='h' || c=='H') {init_k();}
    else if (c==11  || c=='k' || c=='K') ctrl_kk();
    else if (c==12  || c=='l' || c=='L') ctrl_kl();
    else if (c==16  || c=='p' || c=='P') ctrl_kp();
    else if (c==19  || c=='s' || c=='S') ctrl_ks();
    else if (c==20  || c=='t' || c=='T')
        snprintf(zmsg,sizeof(zmsg),"in_k=%d x1=%d x2=%d",in_k(),zkx1,zkx2);
    else if (c==22   || c=='v' || c=='V') ctrl_kv();
    else if (c=='<' || c=='>') ctrl_k_indent(c=='<' ? -1 : 1);
}

void ced::ctrl_k_indent(int dir) {
    if (!zkh) {
        snprintf(zmsg, sizeof(zmsg)-1, "no block to indent");
        return;
    }
    char buf[80];
    request("Enter number to adjust: ", buf, sizeof(buf)-1);
    int x = atoi(buf);
    if (x==0) return;

    const int IMAX=20;
    if (x>IMAX) x=IMAX;
    pline();
    for (zy=zky1; zy<=zky2; zy++) {
        gline(1);
        if (dir<0) {
            if (x>=zbufl) x=zbufl;
            else memmove(zbuf,zbuf+x,zbufl-x);
            zbuf[(zbufl-=x)]=0;
        }
        else if (x>0) {
            memmove(zbuf+x, zbuf, zbufl);
            memset(zbuf,' ',x);
            zbuf[(zbufl+=x)]=0;
        }
        pline();
    }
    disppage(ztop);
}

int ced::check_k() {
    if (zkx1<0 || zkx2<0) return 0;
    if (zky2>zky1 && zky1>=0) return 2;
    if (zky1==zky2 && zkx1==0 && zkx2==HIGH) return 3;
    if (zky1==zky2 && zkx1<zkx2) return 1;
    return 0;
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
    ll2.reset();
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

void ced::ctrl_kp() {
    char *buf;
    int l;
    if ((l=zcopyll.size())<=0) {
        snprintf(zmsg,sizeof(zmsg),"nothing to paste");
        return;
    }
    pline();
    for (int i=0;i<l;i++) {
        buf=zcopyll.get(i);
        ll.ins(i+zy+1,buf);
    }
    zky1=zy+1;
    zky2=zky1+l-1;
    zkx1=zkx2=0;
    zkh=2;
    zedit=1;
    disppage(ztop);
    snprintf(zmsg,sizeof(zmsg),"block pasted");
}

void ced::ctrl_ks() {
    if (zkh<2) {
        snprintf(zmsg,sizeof(zmsg),"no block to save");
        return;
    }
    char *buf;
    zcopyll.reset();
    for (int i=zky1;i<=zky2;i++) {
        buf=ll.get(i);
        zcopyll.ins(buf);
    }
    snprintf(zmsg,sizeof(zmsg),"block saved");
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
    ll2.reset();
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

bool ced::in_k(int x, int y) {
    if (x<0) x=zx;
    if (y<0) y=zy;
    if (!zkh || y<zky1 || y>zky2) return false;
    if (zkh==1 && (x<zkx1 || x>zkx2)) return false;
    return true;
}

void ced::init_k() {
    zkh=0;
    zkx1=zkx2=zky1=zky2=-1;
    disppage(ztop);
}

//
// K_XXX
//
void ced::k_del_char() {
    if (zkh==0) return;
    if (zy==zky1 && zky1==zky2 && zx==zkx1 && zkx1==zkx2) return init_k();
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
    if (zy<zky1) {
        zky1--;
        zky2--;
    }
    else if (zy<zky2) zky2--;
}

void ced::k_del_eol() {
    if (zkh==0) return;
    if (zkh==1 && zy==zky1) {
        if (zx<=zkx1) init_k();
        else if (zx<zkx2) zkx2=zx-1;
    }
}

void ced::k_del_line() {
    if (zkh==0) return;
    if (zy==zky1 && zky1==zky2) return init_k();
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

void ced::k_enter() {
    if (zkh==1 && zy==zky1 && zkx2>=zx) init_k();
}

void ced::k_ins_char() {
    if (zkh==0) return;
    if (zkh!=1 || zky1 != zy) return;
    if (zx>=zkx1 && zx<=zkx2) zkx2++;
}

void ced::k_ins_line() {
    if (zkh==0) return;
    if (zy<zky1) {
        zky1++;
        zky2++;
        return;
    }
    else if (zkh>1 && zy<zky2) {
        zky2++;
        return;
    }
}



