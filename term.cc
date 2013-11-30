#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <string.h>

//
// CLASS TERM
//

//ccinclude
#ifndef _TERM_H
#define _TERM_H
#ifndef _TERMIOS_H
#include <termios.h>
#endif

enum FKEY {
    F1=1000,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,F13,F14,F15,F16,F17,F18,
    F19,F20,F21,F22,F23,F24,INS,DEL,HOME,HOME2,END,END2,PGUP,PGDOWN,
    BTAB,UP,DOWN,RIGHT,LEFT,ZERO,ERROR,ALT_M
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
//ccinclude


//////////////////////////
// Display + KB defs
//////////////////////////
Curses xterm = {
    24,80
    ,"\x1b[H\x1b[2J" // CLRSCR
    ,"\x1b[%d;%dH"  // CUP
    ,"\x1b[K"       // EOL
    ,"\x1b[J"       // EOS
    ,"\x1b[M"       // DEL_LINE
    ,"\x1b[L"       // INS_LINE
    ,"\x1b[H\x1b[2J" // CLR_SCR
    ,"\x1b[0;40m"   // BG
    ,"\x1b[m"       // FG - exit att mode
    ,"\x1b[1;30m"   // FG0
    ,"\x1b[0;31m"   // FG1
    ,"\x1b[0;32m"   // FG2
    ,"\x1b[0;33m"   // FG3
    ,"\x1b[0;34m"   // FG4
    ,"\x1b[0;35m"   // FG5
    ,"\x1b[0;36m"   // FG6
    ,"\x1b[0;30m"   // FG7
    ,"\x1b[7m"      // REVERSE
    ,"\x1b[1m"      // BOLD
    ,"\x1b[4m"      // UNDERLINE
    ,"\x1b[5m"      // BLINK
    ,"\x1b[m"       // EXIT_ATTR_MODE
};

funckey fkey[] = {
     {F1, "\x1b""OP"}
    ,{F2, "\x1b""OQ"}
    ,{F3, "\x1b""OR"}
    ,{F4, "\x1b""OS"}
    ,{F5, "\x1b""[15~"}
    ,{F6, "\x1b""[17~"}
    ,{F7, "\x1b""[18~"}
    ,{F8, "\x1b""[19~"}
    ,{F9, "\x1b""[20~"}
    ,{F10, "\x1b""[21~"}
    ,{F11, "\x1b""[23~"}
    ,{F12, "\x1b""[24~"}
    ,{F13, "\x1b\x1b""OP"}
    ,{F14, "\x1b\x1b""OQ"}
    ,{F15, "\x1b\x1b""OR"}
    ,{F16, "\x1b\x1b""OS"}
    ,{F17, "\x1b\x1b""[15~"}
    ,{F18, "\x1b\x1b""[17~"}
    ,{F19, "\x1b\x1b""[18~"}
    ,{F20, "\x1b\x1b""[19~"}
    ,{F21, "\x1b\x1b""[20~"}
    ,{F22, "\x1b\x1b""[21~"}
    ,{F23, "\x1b\x1b""[23~"}
    ,{F24, "\x1b\x1b""[24~"}
    ,{INS, "\x1b""[2~"}
    ,{DEL, "\x1b""[3~"}
    ,{HOME, "\x1b""[H"}
    ,{END, "\x1b""[F"}
    ,{PGUP, "\x1b""[5~"}
    ,{PGDOWN, "\x1b""[6~"}
    ,{BTAB, "\x1b""[Z"}
    ,{UP, "\x1b""[A"}
    ,{DOWN, "\x1b""[B"}
    ,{RIGHT, "\x1b""[C"}
    ,{LEFT, "\x1b""[D"}
    ,{HOME2, "\x1b""OH"}
    ,{END2, "\x1b""OF"}
    ,{ALT_M, "\x1b""m"}
    ,{ALT_M, "\x1b""M"}
    ,{ZERO, ""}
    ,{ERROR, "~"}
};

///////////////////////
// FUNCTIONS
///////////////////////
char used [256];
void init_used(int cnt) {
     memset(used,0,cnt);
}

int getfkeycnt() {
    for (int i=0; sizeof(used); i++) if (fkey[i].kval == ZERO) return i;
}


term::term() {curses=xterm; open_kb();}
term::~term() {exit_attr(); fsync(1); close_kb();};

void term::puts(const char *s, int len, int row, int col) {
    if (row>=0) cup(row,col);
    if (len<=0) len=strlen(s);
    for (int i=0;i<len && s[i];i++) putchar(s[i]);
}

int term::get() {
    unsigned int c,gotsome;
    c = getchar();
    if (zstate==0) {
	if (c==27) {
	    zp1=1;
	    zbuf[0]=27;
	    zstate=1;
	    //printf(" cnt=%d\n",zkeycnt);
	    init_used(zkeycnt);
	    return 0;
	}
	else if (c<=127) return c;
        else {
            c-=128;
            if (c=='m') return ALT_M;
        }
    }
    else if (zstate==1) {
	zbuf[zp1++]=c;
	zbuf[zp1]=0;
	gotsome=0;
	for (int i=0; i<zkeycnt; i++) {
	    if (used[i]) continue;
	    gotsome=1;
	    if (strcmp(zbuf,fkey[i].kstr)==0) {
		zstate=0;
		zp1=0;
		return fkey[i].kval;
	    }
	    else if (strlen(zbuf) >= strlen(fkey[i].kstr)) used[i]=1;
	}
	if (gotsome==0) {
	    zp1=0;
	    zstate=0;
	    return (ERROR);
	}
    }
    return 0;
}

void term::open_kb() {
    zstate=zmode=0;
    tcgetattr(0,&termios1);
    zfunckeys=fkey;
    zkeycnt=getfkeycnt();
}

void term::close_kb() {
    clrscr();
    setmode(0);
}

int term::setmode(int x) {
    int rc;
    zerrno=0;
    if (x==0) {
	rc=tcsetattr(0,TCSANOW,&termios1);
	if (rc) zerrno=errno;
	else zmode=x;
	return rc;
    }

    termios2=termios1;
    cfmakeraw(&termios2);
    termios2.c_oflag |= OPOST;
    if (x==2) termios2.c_lflag |= ISIG;
    rc=tcsetattr(0,TCSANOW,&termios2);
    if (rc) {
	zerrno=errno;
	return -1;
    }
    zmode=0;
    return 0;
}

int term::request(const char *s, char *rbuf, int len, int flag) {
    int i,c,x=0,ins=1;
    cup(curses.rows,1);
    eol();
    puts(s);
    int off=strlen(s)+1;
    if (len<2) {
	rbuf[0]=0;
        return -1;
    }
    int xl=0;
    while (1) {
	cup(curses.rows,off+1);
	eol();
	for (i=0;i<xl;i++) putchar(rbuf[i]);
	cup(curses.rows,off+x+1);
	c=get();
	if (c==13 || c==F12) {
            if (c==F12) {
                rbuf[0]=0;
                return -1;
            }
            else {
                rbuf[xl]=0;
                return 0;
            }
	}
        if (flag==1) {
            rbuf[0]=c;rbuf[1]=0;
            return 0;
        }
	if (c>=' ' && c<128) {
	    if (ins) for (i=xl; i>x; i--) rbuf[i]=rbuf[i-1];
	    if (x>=xl || ins) xl++;
	    rbuf[x++]=c;
	    if (x==len-1) {  // set len == 2 and get instant response
		rbuf[x]=0;
                return 0;
	    }
	}
	else if (c<' ') {
	    if (c==8) {
		if (x<1) continue;
		x--;
		for (i=x;i<xl-1;i++) rbuf[i]=rbuf[i+1];
		xl--;
	    }
	}
	else if (c==LEFT && x>0) x--;
	else if (c==RIGHT && x<xl) x++;
	else if (c==HOME) x=0;
	else if (c==END) x=xl;
	else if (c==INS) ins=ins==0 ? 1 : 0;
    }
}
