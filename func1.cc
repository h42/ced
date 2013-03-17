#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include "list.h"
#include "term.h"
#include "file.h"
#include "ced.h"

void ced::add_char(int c) {
    gline(1);
    dispchar(c,zy,zx);
    zbuf[zx]=c;
    zx++;
    if (zx>zbufl) {
	memset(&zbuf[zbufl],' ',zx-zbufl);
	zbufl=zx;
    }
    if (zx-zoff >= zmaxx|| zx<zoff) upoff();
}

void ced::bottom() {
    pline();
    zy=ll.size()-1;
    zx=0;
    gline();
    disppage(zy);
}

void ced::bs_char() {
    gline();
    if (zx>zbufl) {
	left();
	return;
    }
    if (zx>0) {
	zx--;
	del_char();
	return;
    }
    if (zy<=0) return;
    pline();
    up();
    end();
    del_char();
}

void ced::btab() {
    gline();
    int x = zx%4;
    if (x>0) zx-=x;
    else if (zx>0) zx-=4;
    if (zx-zoff >= zmaxx|| zx<zoff) upoff();
}

int firstnb(char *s, int l) {
    int hit=-1;
    for (int j=0;j<l;j++) {
	if (s[j] != ' ') {
	    hit = j;
	    break;
	}
	if (hit>=0) break;
    }
    return (hit<0) ? 0 : hit;
}

void ced::ctrl_a() {
    gline();
    zx=firstnb(zbuf,zbufl);
    if (zx<zoff) upoff();
}

void ced::del_char() {
    gline(1);
    if (zx<zbufl) {
	if (zbufl<=0) return;
	for (int i=zx; i<zbufl-1; i++) zbuf[i]=zbuf[i+1];
	zbufl--;
	displine(zbuf,zy,zbufl);
	return;
    }
    if (zx>=zbufl && zy>= ll.size()-1) return;
    gline2(zy+1);
    if (zx>zbufl) memset(&zbuf[zbufl],' ',zx-zbufl);
    memcpy(&zbuf[zx],zbuf2,zbufl2);
    zbufl=zx+zbufl2;
    ll.del(zy+1);
    disppage(ztop);
}

void ced::del_eol() {
    gline(1);
    memset(&zbuf[zx],' ',zbufl-zx-1);
    zbufl=zx;
    dsp.eol();
}

void ced::del_line() {
    if (ll.size()==1) {
	zy=0;
	zedit=zedit2=1;
	zx=zoff=0;
	zbufl=0;
	pline();
	disppage(0);
	return;
    }
    pline();
    zedit=1; zcur=-1;
    ll.del(zy);
    if (zy>=ll.size()) {
	zy--;
	if (zy<ztop) ztop=zy;
    }
    disppage(ztop);
}

void ced::down() {
    pline(); zcur=-1;
    if (zy>=ll.size()-1) return;
    zy++;
    if (zy-ztop < zmaxy-2) return;
    disppage(ztop+1);
}

void ced::end() {
    gline();
    zx=zbufl;
    if (zx-zoff >= zmaxx || zx<zoff) upoff();
}

void ced::enter() {
    pline();
    if (!zins) {
	home();
	if (zy<ll.size()-1) zy++;
	if (zy+ztop>=zmaxy-2) disppage(++ztop);
	zcur=-1;
	return;
    }
    gline(1);
    if (zx>=zbufl) {
	ins_line(1);
	return;
    }
    int i,j,ind=0;
    char *tbuf=(char *)alloca(zbufl-zx+2);
    for (i=zx,j=0;i<zbufl;i++) tbuf[j++]=zbuf[i];
    zbufl=zx;
    pline();
    ins_line(0); // should set zcur=-1

    gline(1);
    if (zindent) {
	ind=firstnb(zbuf,j);
	if (ind) memset(zbuf,' ',ind);
    }
    memcpy(zbuf+ind,tbuf,j);
    zbufl=j+ind;
    zx=ind;
    disppage(ztop);
}

void ced::home() {
    zx=0;
    if (zx-zoff >= zmaxx|| zx<zoff) upoff();
}

void ced::ins_char(int c) {
    gline(1);
    if (zx<=zbufl) {
	for (int i=zbufl;i>zx;i--) zbuf[i]=zbuf[i-1];
	zbuf[zx++]=c;
	zbufl++;
    }
    else {
	memset(&zbuf[zbufl],' ',zx-zbufl);
	zbuf[zx++]=c;
	zbufl=zx;
    }
    if (zx-zoff >= zmaxx|| zx<zoff) upoff();
    displine(zbuf,zy,zbufl);
}

void ced::ins_line(int disp) {
    pline(); zcur=-1;
    zedit=1;
    zx=0;
    zy++;
    if (zy>ll.size()) zy=ll.size();
    ll.ins(zy,"",0);
    if (zy-ztop >= zmaxy-2) ztop++;
    if (disp) disppage(ztop);
}

void ced::left() {
    if (zx>0) zx--;
    if (zx-zoff >= zmaxx|| zx<zoff) upoff();
}

void ced::pgup() {
    int y=zy-ztop;
    pline(); zcur=-1;
    if (ztop==0) {
	zy=0;
	return;
    }
    ztop-=zmaxy-2;
    if (ztop<0) ztop=0;
    zy=ztop+y;
    disppage(ztop);
}

void ced::pgdown() {
    int y=zy-ztop;
    pline(); zcur=-1;
    if (ztop>=ll.size()-2) return;
    ztop+=zmaxy-2;
    if (ztop>=ll.size()-2) ztop=ll.size()-2;
    if (ztop<0) ztop=0;
    zy=ztop+y;
    if (zy>=ll.size()) zy=ll.size()-1;
    disppage(ztop);
}

void ced::right() {
    zx++;
    if (zx-zoff >= zmaxx|| zx<zoff) upoff();
}

void ced::tab() {
    zx+=4+zx%4;
    if (zx-zoff >= zmaxx|| zx<zoff) upoff();
}

void ced::top() {
    pline(); zcur=-1;
    zy=zx=0;
    disppage(zy);
}

void ced::undoer() {
    //zu.trace();
    zu.pop();
    disppage(ztop);
}

void ced::up() {
    pline(); zcur=-1;
    if (zy>0) zy--;
    if (zy-ztop<0) disppage(zy);
}

void ced::upoff() {
    if (zx-zoff < zmaxx && zx>=zoff) return;
    zoff = zx - zmaxx / 2;
    if (zoff<0) zoff = 0;
    disppage(ztop);
}
