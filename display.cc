#include <stdio.h>
#include <string.h>
#include "list.h"
#include "term.h"
#include "file.h"
#include "ced.h"

void ced::dispchar(int c, int y, int x) {
    if (y<0) y = zy;
    if (x<0) x = zx;
    y -= ztop ;
    x -= zoff ;
    dsp.cup(y+1,x+1);
    putchar(c);
}

void ced::displine(char *sx, int y, int len) {
    if (len<=0) len=strlen(sx);
    dsp.cup(y+1-ztop,1);
    dsp.eol();
    if (zoff>=len) return;
    sx=&sx[zoff];
    len-=zoff;
    for (int i=0; i<len; i++) putchar(sx[i]);
}

void ced::disppage(int top) {
    int i;
    pline();
    ztop=top;
    for (i=0; i<zmaxy-2 && i+top<ll.size(); i++) {
	gline2(i+top);
	zbuf2[zbufl2]=0;
	displine(zbuf2, i+top, zbufl2);
    }
    dsp.eos();
}

void ced::dispstat() {
    char sx[80];
    if (1) {
	dsp.cup(zmaxy,1);
	dsp.eol();
    }
    if (zmsg[0]) {
	int mlen=(zmaxy>50) ? 50 : zmaxy-1;
	//dsp.cup(rows,1);
	zmsg[mlen]=0;
	dsp.puts(zmsg,0,zmaxy,1);
    }
    sprintf(sx,"%4d,%d %d  ",zy+1,zx+1,zoff);
    sx[12]=0;
    dsp.puts(sx,0,zmaxy,zmaxx-12);
}

void ced::request(const char *s, char *rbuf, int len) {
    dsp.request(s,rbuf,len);
}
