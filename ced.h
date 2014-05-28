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
