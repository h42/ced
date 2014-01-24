#ifndef _TERM_H
#define _TERM_H
#ifndef _TERMIOS_H
#include <termios.h>
#endif

enum FKEY {
    F1=1000,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,F13,F14,F15,F16,F17,F18,
    F19,F20,F21,F22,F23,F24,INS,DEL,HOME,HOME2,END,END2,PGUP,PGDOWN,
    BTAB,UP,DOWN,RIGHT,LEFT,ZERO,ERROR,ALT_M,ALT_N,ALT_R
};

struct funckey {
    FKEY kval;
    const char *kstr;
    int status;
};

struct Curses {
    int rows,cols;
    const char *clrscr;
    const char *cup;
    const char *eol;
    const char *eos;
    const char *del_line;
    const char *ins_line;
    const char *clr_scr;
    const char *bg;
    const char *fg;
    const char *fg0;
    const char *fg1;
    const char *fg2;
    const char *fg3;
    const char *fg4;
    const char *fg5;
    const char *fg6;
    const char *fg7;
    const char *reverse;
    const char *bold;
    const char *underline;
    const char *blink;
    const char *exit;
};

class term {
public:
    void open_kb();
    void close_kb();
    int get();
    void puts(const char *s, int len=0, int row=-1, int col=-1);
    int setmode(int x=0);
private:
    termios termios1,termios2;
    int zmode;
    int zstate;
    int zp1,zp2;
    int zfuncs;
    funckey *zfunckeys;
    int zkeycnt;
    char zbuf[20];
    long zerrno;
public:
    term();
    ~term();
    void tattr(const char *s) {printf(s);}
    void clrscr() {tattr(curses.clrscr);}
    void cup(int y, int x)    {printf(curses.cup,y,x);}
    void eol()    {tattr(curses.eol);}
    void eos()    {tattr(curses.eos);}
    void del_line() {tattr(curses.del_line);}
    void ins_line() {tattr(curses.ins_line);}
    void clr_scr() {tattr(curses.clr_scr);}
    void bg() {tattr(curses.bg);}
    void fg() {tattr(curses.fg);}
    void fg0() {tattr(curses.fg0);}
    void fg1() {tattr(curses.fg1);}
    void fg2() {tattr(curses.fg2);}
    void fg3() {tattr(curses.fg3);}
    void fg4() {tattr(curses.fg4);}
    void fg5() {tattr(curses.fg5);}
    void fg6() {tattr(curses.fg6);}
    void fg7() {tattr(curses.fg7);}
    void reverse() {tattr(curses.reverse);}
    void bold() {tattr(curses.bold);}
    void underline() {tattr(curses.underline);}
    void blink() {tattr(curses.blink);}
    void exit_attr() {tattr(curses.exit);}
    int  request(const char *s, char *rbuf, int len, int flag=0);
    int  rows() {return curses.rows;}
    int  cols() {return curses.cols;}
private:
    Curses curses;
};

#endif
