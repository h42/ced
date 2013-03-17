#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "term.h"
#include "list.h"
#include "file.h"

using namespace std;

//ccinclude
#ifndef _CED_H
#define _CED_H

#define MAXUNDO 50

typedef unsigned char uchar;

class ced;

class under {
public:
    under() {zused=0; zbuf=0;}
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
};

class undo {
public:
    undo(ced *t) {zt=t; zp1=-1; zcnt=0;}
    ~undo() {};
    void trace();
    void push(int type=0);
    void pop();
    void del(int d1, int dlen=1);
private:
    ced *zt;
    under zunder[MAXUNDO];
    int zp1,zcnt;
};

class ced {
    friend class undo;
    friend class under;
public:
    ced();
    ~ced();
    void add_char(int c);
    void bs_char();
    void btab();
    void bottom();
    void ctrl_a();
    void del_char();
    void del_eol();
    void del_line();
    void dispchar(int c, int y=-1, int x=-1);
    void displine(char *s, int y, int len=-1);
    void disppage(int top);
    void dispstat();
    void down();
    void end();
    void enter();
    void gline(int up=0);
    void gline2(int x);
    void home();
    void ins_char(int c);
    void ins_line(int disp=1);
    void left();
    void main();
    int  max(int x,int y) {return x>y ? x : y;}
    int  min(int x,int y) {return x<y ? x : y;}
    void newf();
    void pgup();
    void pgdown();
    void pline();
    void request(const char *, char *, int);
    void right();
    void tab();
    void top();
    void undoer();
    void up();
    void upoff();

    // FILE
    int  loadfile(const char *fn=0);
    int  readf(const char *fn);
    int  savefile();

private:
    undo    zu;
    term    dsp;
    list    ll;
    //file    zfile;
    char   *zbuf;
    char   *zbuf2;
    int     zbufl;
    int     zbufl2;
    int     zbufsize;
    char   *zfbuf;
    int     zfbufsize;
    int     zindent;
    int     zins;
    int     zcur,zx,zy,ztop,zoff;
    int     zedit,zedit2;
    int     ztabsize;
    int     zmaxx,zmaxy;
    char    zmsg[80];
    char    zfn[256];
    int     zr0,zoverride;
};

#endif
//ccinclude

ced::ced() : zu(this) {
    zbufsize=zfbufsize=4064;
    zbuf=(char *)malloc(zbufsize);
    zbuf2=(char *)malloc(zbufsize);
    zfbuf=(char *)malloc(zfbufsize);
    zbufl=0;
    zins=1;
    zindent=1;
    zx=zy=zoff=ztop=zedit=zedit2=0;
    zcur=-1;
    zmsg[0]=0;
    ztabsize=8;
    zfn[0]=zr0=zoverride=0;
    dsp.setmode(1);
    dsp.fg7();
    dsp.clrscr();
    fsync(1);
}

ced::~ced() {
    return;
}

void ced::gline(int up) {
    int i;
    char c,*llbuf;
    if (up) zedit=zedit2=1;
    if (zy==zcur) return; // ALL FUNCTIONS MUST SET ZY!=ZCUR IF ZBUF NOT VALID
    zcur=zy;
    llbuf=ll.get(zy);
    for (i=zbufl=0;i<zbufsize && zbufl<zbufsize;i++) {
	if (!(c=llbuf[i])) break;
	if (c == 9) {
	    zbuf[zbufl++]=' ';
	    while(zbufl%ztabsize && zbufl<zbufsize) zbuf[zbufl++]=' ';
	}
	else zbuf[zbufl++]=c;
	//if (zbufl+10 >= zbufsize) init_buffers(0);
    }
}

void ced::gline2(int x) {
    int i;
    char c,*cur=ll.get(x);
    for (i=zbufl2=0;i<zbufsize && zbufl2<zbufsize;i++) {
	if (!(c=cur[i])) break;
	if (c == 9) {
	    zbuf2[zbufl2++]=' ';
	    while(zbufl2%ztabsize && zbufl2<zbufsize) zbuf2[zbufl2++]=' ';
	}
	else zbuf2[zbufl2++]=c;
	//if (zbufl2+10 >= zbufsize) init_buffers(0);
    }
}

void ced::pline() {
    int i,j;
    if (!zedit2) return;
    //listgetopt(-1);
    j=zbufl;
    zbufl=0;
    for (i=j-1;i>=0;i--)
	if (zbuf[i]!=' ') {
	    zbufl=i+1;
	    break;
	}
    zbuf[zbufl]=0;
    ll.del(zcur);
    zbuf[zbufl]=0;
    ll.ins(zcur,zbuf,zbufl+1);
    zedit2=0;
}

void ced::newf() {
    ll.reset();
    ll.ins("");
    zx=zy=zoff=ztop=0;
    disppage(0);
}

void ced::main() {
    int c=0;
    //tfile();
    //return;

    zmaxx=dsp.cols();
    zmaxy=dsp.rows();

    readf("temp");
    disppage(0);

    while (1) {
	dispstat();
	zmsg[0]=0;
	dsp.cup(zy+1-ztop,zx+1-zoff);
	fflush(stdout);
	c=dsp.get();
	if (c<=26) {
	    if (c==1) ctrl_a();
	    else if (c==2) bottom();
	    else if (c==4) del_line();
	    else if (c==5) del_eol();
	    else if (c==8) bs_char();
	    else if (c==13) enter();
	    else if (c==20) top();
	    else if (c==14) ins_line(1);
	    else if (c==9) tab();
	    else if (c==21) undoer();
	}

	else if (c<256) {
	    if (!zins) add_char(c);
	    else ins_char(c);
	    //putchar(c);
	}

	else if (c >= F1) {
	    if (c == F12) break;
	    else if (c==F10) loadfile();
	    else if (c==F11) savefile();
	    else if (c==BTAB) btab();
	    else if (c==DEL) del_char();
	    else if (c==DOWN) down();
	    else if (c==END) end();
	    else if (c==HOME) home();
	    else if (c==INS) zins=zins==0 ? 1 : 0;
	    else if (c==LEFT) left();
	    else if (c==PGDOWN) pgdown();
	    else if (c==PGUP) pgup();
	    else if (c==RIGHT) right();
	    else if (c==UP) up();
	}

	//else printf("%d\n",c);
    }
}

int main() {
    ced e1;
    e1.main();
    return 0;
}