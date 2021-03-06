#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include "term.h"
#include "list.h"
#include "glob.h"

using namespace std;

//ccinclude
#ifndef _CED_H
#define _CED_H

#define MAXUNDO 50

typedef unsigned char uchar;

#include "undo.h"
#include "history.h"
#include "re.h"
#include "log.h"

class ced;

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
    void go();
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
    void pgup();
    void pgdown();
    void pline(int rollback=0);
    void request(const char *, char *, int);
    void rfind();
    void rchange();
    void recent();
    void right();
    void scroll(int);
    void tab();
    void top();
    void undoer();
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
    void ctrl_k_indent(int);
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
    int  checksave();
    int  loadfile(const char *fn=0);
    int  newfile();
    int  readf(const char *fn);
    int  savefile();
    int  swapfile(int x=1);

//private:
    undo    zu;
    term    dsp;
    list    zl;
    //file    zfile;
    char   *zbuf;
    char   *zbuf2;
    int     zbufl;
    int     zbufl2;
    int     zbufsize;
    char   *zfbuf;
    int     zcur;
    int     zfbufsize;
    int     zindent;
    int     zins;
    int     zedit,zedit2;
    int     ztabsize;
    int     ztabcomp;
    int     zmaxx,zmaxy;
    char    zmsg[80];
    char    zfn[256];
    int     zr0,zoverride;
    history zhist;
    int     zx,zy,ztop,zoff;

    int     zkx1,zkx2,zky1,zky2,zkh;
    list    zcopyll;

    re      zre; // used for find / change
    bool    zchange;
    char    zchangebuf[128];

    log     zlog;
};

#endif
//ccinclude

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
    ztabcomp=0;
    zkh=0; zkx1 = zkx2 = zky1 = zky2 = -1;
    zfn[0]=zr0=zoverride=0;
    dsp.setmode(1);
    dsp.fg7();
    dsp.clrscr();
    fsync(1);
    zu.init(this);
    zchange=false;
}

ced::~ced() {
    return;
}

void ced::gline(int up) {
    int i;
    char c,*llbuf;
    //zlog.put("gline up=%d zy=%d zcur=%d", up,zy,zcur);
    if (up) zedit=zedit2=1;
    if (zy==zcur) return; // ALL FUNCTIONS MUST SET ZY!=ZCUR IF ZBUF NOT VALID
    zcur=zy;
    llbuf=zl.get(zy);
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
    //zlog.put("gline2 x=%d", x);
    char c,*cur=zl.get(x);
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

#define TAB 8
#define tabstop(x) ((x)+(TAB-(x)%TAB))
void ced::pline(int rollback) {
    int i,j,k,state,c;
    //zlog.put("pline zedit2=%d", zedit2);
    if (!zedit2 && !rollback) return;
    //listgetopt(-1);

    j=zbufl;
    zbufl=0;
    for (i=j-1;i>=0;i--)
	if (zbuf[i]!=' ') {
	    zbufl=i+1;
	    break;
	}
    zbuf[zbufl]=0;

    if (ztabcomp) {
        for (i=j=k=state=0; i<zbufl; i++) {
            c=zbuf[i];
            if (state==0) {
                if (c==' ') {
                    k++;
                    if (k==TAB) {
                        zbuf[j++]=9;
                        k=0;
                    }
                }
                else {
                    state=1;
                    if (k) {
                        memset(&zbuf[j],' ',k);
                        j+=k;
                    }
                    zbuf[j++]=c;
                }
            }
            else zbuf[j++]=c;
        }
        zbuf[j]=0;
        zbufl=j;
    }

    zl.del(zcur);
    zbuf[zbufl]=0;
    zl.ins(zcur,zbuf,zbufl+1);
    zedit2=0;
}

static int waitio(int timeout) {
    int rc;
    struct pollfd pfd;
    pfd.fd=0;
    pfd.events=POLLIN;
    pfd.revents=0;
    rc=poll(&pfd,1,timeout);
    if (rc>0) {
        if (pfd.revents&POLLIN) rc=0;
        else rc=-1;
    }
    else rc=-1;
    return rc;
}

static int zresize=0;
static void gotsigwinch(int x) {
    if (x==SIGWINCH) zresize=1;
}

static int getkb(ced *ced) {
    int c;
    while (1) {
        if (waitio(5000)==0) {
             while (!(c=ced->dsp.get()));
             break;
        }
        if (zresize) {
            zresize=0;
            ced->dsp.resize();
            ced->zmaxy=ced->dsp.rows();
            ced->zmaxx=ced->dsp.cols();
            ced->disppage(ced->ztop);
        }
    }
    return c;
}

void ced::main(int argc, char **argv) {

    int c=0;
    hist *h;
    char *fn=0;

    //zlog.open("xxx");

    newfile();
    zhist.read();
    dsp.resize();
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
        //zlog.put("----------------");

        c=getkb(this);

	if (c<=26) {
	    if (c==1) ctrl_a();
            else if (c==2) {bottom();}
	    else if (c==4) del_line();
	    else if (c==5) del_eol();
            else if (c==6) scroll(1);
            else if (c==7) go();
	    else if (c==8) bs_char();
	    else if (c==9) tab();
            else if (c==11) ctrl_k();
	    else if (c==13) enter();
	    else if (c==14) ins_line(1);
            else if (c==18) scroll(-1);
            else if (c==20) {top();}
            else if (c==21) undoer();
            else if (c==24) ctrl_x();
	}

	else if (c<256) {
	    if (!zins) add_char(c);
	    else ins_char(c);
	    //putchar(c);
	}

	else if (c >= F1) {
            if (c == F12) {
                if (!checksave()) break;
            }
            else if (c==F1)   help();
            else if (c==F5)   rfind();
            else if (c==F6)   rchange();
            else if (c==F8)   swapfile(2);
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
            else if (c==ALT_N) newfile();
            else if (c==ALT_R) recent();
	}

	//else printf("%d\n",c);
    }

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
    h = h ? h : (char *)".";
    snprintf(buf, sizeof(buf), "make 2>&1 | tee %s/.ced/ced_temp", h);
    system(buf);
    snprintf(buf, sizeof(buf), "%s/.ced/ced_temp", h);
    loadfile(buf);
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

    signal(SIGWINCH,gotsigwinch);

    e1.main(argc,argv);
    return 0;
}
