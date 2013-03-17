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