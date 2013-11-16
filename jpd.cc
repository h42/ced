#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "term.h"
#include "list.h"
#include "file.h"
#include "glob.h"

using namespace std;

//ccinclude
#ifndef _CED_H
#define _CED_H

#define MAXUNDO 50

typedef unsigned char uchar;


#include "history.h"
#include "re.h"

class ced;

/*
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
*/

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
    void change();
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
    void find(int sensitive=0);
    void gline(int up=0);
    void gline2(int x);
    void help();
    int  hist_put();
    void home();
    void ins_char(int c);
    void ins_line(int disp=1);
    void left();
    void main(int,char **);
    void make();
    int  max(int x,int y) {return x>y ? x : y;}
    int  min(int x,int y) {return x<y ? x : y;}
    void newf();
    void pgup();
    void pgdown();
    void pline();
    void request(const char *, char *, int);
    void rfind();
    void rchange();
    void right();
    void tab();
    void top();
    //void undoer();
    void up();
    void upoff();

    void ctrl_x();
    void ctrl_xx();
    void ctrl_k();
    void ctrl_kb();
    void ctrl_kc();
    void ctrl_kd();
    void ctrl_kk();
    void ctrl_kl();
    void ctrl_kp();
    void ctrl_ks();
    void ctrl_kv();
    int  check_k();
    void disp_k();
    void init_k();
    bool in_k(int x=-1, int y=-1);
    void k_del_line();
    void k_del_eol();
    void k_del_char();
    void k_enter();
    void k_ins_char();
    void k_ins_line();

    // FILE
    void checksave();
    int  loadfile(const char *fn=0);
    int  newfile();
    int  readf(const char *fn);
    int  savefile();
    int  swapfile(int x=0);

//private:
    //undo    zu;
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
    history zhist;

    int     zkx1,zkx2,zky1,zky2,zkh;
    list    zcopyll;

    re      zre; // used for find / change
    bool    zchange;
    char    zchangebuf[128];
};

#endif
//ccinclude

//ced::ced() : zu(this) {
ced::ced() {
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
    zkh=0; zkx1 = zkx2 = zky1 = zky2 = -1;
    zfn[0]=zr0=zoverride=0;
    dsp.setmode(1);
    dsp.fg7();
    dsp.clrscr();
    fsync(1);
    zchange=false;
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

void ced::main(int argc, char **argv) {

    /*
    char buf[256];
    getfn(dsp, ".", "cc,makefile", buf, sizeof(buf));
    dsp.clrscr();
    puts(buf);
    dsp.get();
    return;
    */

    int c=0;
    hist *h;
    char *fn=0;

    newfile();
    zhist.read();
    zmaxx=dsp.cols();
    zmaxy=dsp.rows();

    if (argc && argv[1] && argv[1][0]) fn=argv[1];
    else {
        h = zhist.pop();
        if (h->hfn[0]) fn=h->hfn;
    }
    if (fn) {
        loadfile(fn);
        h=zhist.pop(fn);
        if (h) {
            zx=h->hx;
            zy=h->hy;
            zoff=h->hoff;
            ztop=h->htop;
        }
    }

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
            //else if (c==7) hist_put();
	    else if (c==8) bs_char();
            else if (c==11) ctrl_k();
	    else if (c==13) enter();
	    else if (c==20) top();
	    else if (c==14) ins_line(1);
	    else if (c==9) tab();
            //else if (c==21) undoer();
            else if (c==24) ctrl_x();
	}

	else if (c<256) {
	    if (!zins) add_char(c);
	    else ins_char(c);
	    //putchar(c);
	}

	else if (c >= F1) {
	    if (c == F12) break;
            else if (c==F1)   help();
            else if (c==F5)   rfind();
            else if (c==F6)   rchange();
            else if (c==F9)   swapfile();
            else if (c==F10)  loadfile();
            else if (c==F11)  savefile();
	    else if (c==BTAB) btab();
            else if (c==DEL)  del_char();
	    else if (c==DOWN) down();
            else if (c==END)  end();
	    else if (c==HOME) home();
            else if (c==INS)  zins=zins==0 ? 1 : 0;
            else if (c==LEFT) left();
	    else if (c==PGDOWN) pgdown();
            else if (c==PGUP)  pgup();
	    else if (c==RIGHT) right();
            else if (c==UP)    up();
            else if (c==ALT_M) make();
	}

	//else printf("%d\n",c);
    }
    checksave();
    if (zfn[0]) {
        zhist.push(zfn,zx,zy,zoff,ztop);
        zhist.write();
    }
}

void ced::make() {
    if (savefile()) return;
    dsp.clrscr();
    fflush(stdout);
    char buf[256], *h = getenv("HOME");
    h=0;
    h = h ? h : (char *)".";
    snprintf(buf, sizeof(buf), "make 2>&1 | tee %s/ced_temp", h);
    system(buf);
    dsp.get();
    disppage(ztop);
}

void ced::help() {
    dsp.clrscr();
    puts("CED V0.1");
    dsp.get();
    disppage(ztop);
}

void t1() {
    re r;
    const char *s="xxxhay";
    int l=strlen(s);
    int rc=r.open("ha",0);
    printf("rc=%d\n",rc);
    rc=r.wild((unsigned char *)s,l);
    printf("rc=%d fast=%d\n",rc,r.zfast);
}

int main(int argc, char **argv) {
    ced e1;

    //t1();

    e1.main(argc,argv);
    return 0;
}
