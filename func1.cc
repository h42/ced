#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include "list.h"
#include "term.h"
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
    upoff();
}

void ced::bottom() {
    pline();
    zy=ll.size()-zmaxy+2;
    zy = (zy>=0) ? zy : 0;
    disppage(zy);
    zx=0;
    zy=ll.size()-1;
    gline();
}

void ced::bs_char() {
    gline(1);
    // K_BS_CHAR not necessary because del_char handles it
    if (zx>zbufl) {
	left();
	return;
    }
    if (zx>0) {
	zx--;
        del_char(0);
	return;
    }
    if (zy<=0) return;
    pline();
    up(0);
    end(0);
    del_char(0);
}

void ced::btab() {
    gline();
    int x = zx%4;
    if (x>0) zx-=x;
    else if (zx>0) zx-=4;
    upoff();
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
    upoff();
}

void ced::del_char(int pushit) {
    gline(1);
    k_del_char();
    if (zx<zbufl) {
	if (zbufl<=0) return;
	for (int i=zx; i<zbufl-1; i++) zbuf[i]=zbuf[i+1];
        zbuf[--zbufl]=0;
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
    k_del_eol();
    if (zx<zbufl) memset(&zbuf[zx],' ',zbufl-zx-1);
    zbufl=zx;
    dsp.eol();
}

void ced::del_line() {
    if (ll.size()==1) {
        init_k();
	zy=0;
	zedit=zedit2=1;
	zx=zoff=0;
	zbufl=0;
	pline();
	disppage(0);
	return;
    }
    pline();
    k_del_line();
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

void ced::end(int pushit) {
    gline();
    zx=zbufl;
    upoff();
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
    k_enter(); //if zkx2>=zx  and zkh==1 then init_k
    for (i=zx,j=0;i<zbufl;i++) tbuf[j++]=zbuf[i];
    zbufl=zx;
    pline();
    int gotbrace = (zx>0 && zbuf[zx-1]=='{' ) ? 4 : 0; // must come before ins_line
    ins_line(0); // should set zcur=-1
    gline(1);
    if (zindent) {
	ind=firstnb(zbuf,j);
        ind+=gotbrace;
	if (ind) memset(zbuf,' ',ind);
    }
    snprintf(zmsg,sizeof(zmsg),"ind=%d",ind);
    memcpy(zbuf+ind,tbuf,j);
    zbufl=j+ind;
    zx=ind;
    disppage(ztop);
}

void ced::go() {
    char buf[20];
    request("Enter line number: ", buf, sizeof(buf)-1);
    int i = atoi(buf) - 1;
    if (i<0 || i>=ll.size()) return;
    pline();
    zy=i;
    if (zy<ztop || zy>ztop+zmaxy-3) disppage(ztop=zy);
}

void ced::home() {
    zx=0;
    upoff();
}

//
// INS_CHAR
//
bool blankline(char *buf, int bufl) {
    for (int i=0; i<bufl; i++) if (buf[i] != ' ') return false;
    return true;
}

int match_brace(ced &e) {
    int i,rc=0;
    for (i=e.zy-1; i>=0; i--) {
         e.gline2(i);
         if (e.zbufl2>0 && e.zbuf2[e.zbufl2-1] == '{')
             return firstnb(e.zbuf2, e.zbufl2);
    }
    return rc;
}

void ced::ins_char(int c) {
    gline();
    gline(1);
    k_ins_char();
    if ( zindent && c=='}' && blankline(zbuf,zbufl)) {
        zx=match_brace(*this);
        if (zx>0) memset(zbuf,' ',zx);
        zbuf[zx++]='}';
        zbufl=zx;
    }
    else if (zx<=zbufl) {
	for (int i=zbufl;i>zx;i--) zbuf[i]=zbuf[i-1];
	zbuf[zx++]=c;
	zbufl++;
    }
    else {
	memset(&zbuf[zbufl],' ',zx-zbufl);
	zbuf[zx++]=c;
	zbufl=zx;
    }
    zbuf[zbufl]=0;
    upoff();
    displine(zbuf,zy,zbufl);
}

//
// INS_LINE
//
void ced::ins_line(int disp) {
    k_ins_line();
    if (zindent) {
        gline2(zy);
        zx=firstnb(zbuf2,zbufl2);
        if (zbufl2>0 && zbuf2[zbufl2-1]=='{' ) zx+=4;
    }
    else zx=0;
    pline(); zcur=-1;
    zedit=1;
    zy++;
    if (zy>ll.size()) zy=ll.size();
    ll.ins(zy,"",0);
    if (zy-ztop >= zmaxy-2) ztop++;
    if (disp) disppage(ztop);
}

void ced::left() {
    if (zx>0) zx--;
    upoff();
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
    upoff();
}

void ced::scroll(int x) {
    ztop += x;
    if (ztop<0) ztop=0;
    if (ztop>ll.size()-1) ztop=ll.size()-1;
    if (zy<ztop) zy=ztop;
    else if (zy>ztop+zmaxy-2) zy = ztop+zmaxy-2;
    disppage(ztop);
}

void ced::tab() {
    zx+=4+zx%4;
    upoff();
}

void ced::top() {
    pline(); zcur=-1;
    zy=zx=0;
    disppage(zy);
}

void ced::undoer() {
    pline(1); // must update zbuf with pline even if zedit2==0
    zu.pop();
    disppage(ztop);
}

void ced::up(int pushit) {
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
