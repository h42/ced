//#define _POSIX_SOURCE 1
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include "term.h"
#include "vec.h"
#include "str.h"

//ccinclude
char * getfn(term &dsp, const char *dir, const char *suffix, char *, int);
//ccinclude

static bool goodSuffix(const char *base, const char *slist) {
    int i,c,p1=0,l1=strlen(base);
    char sx[512];
    if (strlen(slist)+2 > sizeof(sx)) return 0;
    for (i=0;;i++) {
        c=slist[i];
        if (c==0 || c==',') {
            sx[p1]=0;
            if (p1<=l1 && strcasecmp(&base[l1-p1],sx)==0) return true;
            if (c==0) break;
            p1=0;
        }
        else sx[p1++]=c;
    }
    return false;
}

static void getfiles(const char *dir, const char *suffix, vec<str> &zv) {
    DIR *zdir;
    zv.sp(0);
    zdir = opendir(dir);
    if (!zdir) return;
    dirent *de;
    str sx;
    for (;;) {
        de=readdir(zdir);
        if (!de) break;
        if (!strcmp(de->d_name,".") || !strcmp(de->d_name,"..")) continue;
        if (de->d_type!=DT_REG) continue;
        sx=de->d_name;
        if (goodSuffix(sx.cstr(),suffix)) zv.push(sx);
    }
    closedir(zdir);
    if (zv.sp()-1 <= 0) return;
    zv.merge_sort(0,0,zv.sp()-1);
}

static bool readint(const char *s, int *sum) {
    int i;
    for (i=0,*sum=0; s[i]; i++) {
        if (!isdigit(s[i])) return false;
        *sum = *sum*10 + s[i] - '0';
    }
    return true;
}

char * getfn(term &dsp, const char *dir, const char *suffix, char *buf, int bufl) {
    int i,top=0,Y=10,c,p1=0;
    vec<str> zv;
    getfiles(dir,suffix,zv);
    buf[0]=0;
    while (1) {
        dsp.clrscr();
        for (i=0; i+top<zv.sp() && i<Y; i++) {
            printf("%d. %s\n", i+top+1, zv[i+top].cstr());
        }
        dsp.cup(dsp.rows(),1);
        putchar('='); putchar('='); putchar('>'); putchar(' ');
        for (i=0; i<p1; i++) putchar(buf[i]);
        c=dsp.get();
        if (c==F12 || c=='q') {
            buf[0]=0;
            return buf;
        }
        else if (c==PGDOWN) {
            top+=Y;
            if (top>=zv.sp()) top=zv.sp()-1;
            if (top<0) top=0;
            p1=0;
        }
        else if (c==PGUP) {
            top-=Y;
            if (top<0) top=0;
            p1=0;
        }
        else if (c=='.' && p1==0) getfiles(dir, ".cc,.c,.h,.hs,makefile",zv);
        else if (c=='*' && p1==0) getfiles(dir, "", zv);
        else if (c==13 && p1) {
            buf[p1]=0;
            if (readint(buf, &i)) {
                if (i>0 && i<= zv.sp()) {
                    snprintf(buf,bufl,"%s",zv[i-1].cstr());
                    return buf;
                }
            }
            p1=0;
        }
        else if (p1<bufl-1) {
            buf[p1++]=c;
            putchar(c);
        }

    }
    return buf;
}
