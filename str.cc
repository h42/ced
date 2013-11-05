#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

//ccinclude
#ifndef _STR_H
#define _STR_H
#ifndef _STDIO_H
#include <stdio.h>
#endif

class str {
public:
    str(const char *s,int ilen=0);
    str(const int ilen=0);
    str(const str &s);
    ~str();
    int center(int x=0);
    char *cstr() {return zbuf;};
    void display() {::puts(zbuf);}
    void log() {::printf("len=%d size=%d buf=%s\n",zlen,zsize,zbuf);}
    int find(const char *str,int x=0,int icase=0);
    void grow(int l);
    int  length() {return zlen;};
    str &concat(const str &s);
    str &concat(char c);
    str &copy(const str &s);
    str &copy(const char *s);
    str &operator=(const str &s);
    str &operator=(const char *s);
    bool operator<(const str &str1);
    bool operator<(const char *str1);
    bool operator>=(const str &str1);
    bool operator>=(const char *str1);
    str operator+(char c);
    str operator+(const str &s2);
    char operator[](int);
    friend str operator+(const char *s,const str &s1);
    int format(const char *fmt,...);
    int change(const char *,int x=0,int l1=0,int l2=0);
    int trim(int x=3);
    int triml() {return trim(1);}
    int trimr() {return trim(2);}
private:
    char *zbuf;
    short zlen,zsize;
} ;

#endif
//ccinclude

//
// Constructor / destructor
//
str::str(const char *s,int ilen) {
    int l=strlen(s);
    if (l+1>ilen) ilen=l+1;
    zsize=0; //req for initial malloc
    zbuf=0;
    grow(ilen); // size must be 0 before 1st grow
    if (zsize<ilen) return;
    if (s) memcpy(zbuf,s,l+1);
    zlen=l;
    //::printf("zbuf=%p\n",zbuf);
}

str::str(int size) {
    zlen=0;
    if (size<=0) size=80;
    zbuf=0;
    zsize=0; //req for initial malloc
    grow(size); // size must be 0 before 1st grow
    if (zsize<size) return;
    zbuf[0]=0;
    //::printf("buf=%p\n",zbuf);
}

str::str(const str &s) {
    zlen=s.zlen;
    zbuf=0;
    zsize=0; //req for initial malloc
    grow(s.zsize);
    if (zsize!=s.zsize) return;
    memcpy(zbuf,s.zbuf,zlen+1);
    //::printf("buf=%p\n",zbuf);
}

str::~str() {
    if (zbuf) {
	free(zbuf);
	zbuf=0;
    }
}

//////////////
// CONCAT
//////////////
str & str::concat(const str &s) {
    int l=zlen+s.zlen;
    if (zsize<l+1) grow(l+1);
    if (zsize<l+1) return *this;
    if (s.zlen>0) memcpy(&zbuf[zlen], s.zbuf, s.zlen);
    zlen=l;
    zbuf[l]=0;
    return *this;
}

str &str::concat(char c) {
    int l=zlen+1;
    if (zsize<l+1) grow(l+1);
    if (zsize<l+1) return *this;
    zbuf[zlen++]=c;
    zbuf[zlen]=0;
    return *this;
}

//////////////
// CENTER
//////////////
int str::center(int x) {
    if (x<=0) x=zsize-1;
    if (zsize<=0 || x>=zsize-1) return -1;
    if (zlen<=0 || zlen==x) return 0;
    if (zlen<x) {
	int p1;
	p1=(x-zlen)/2;
	if (p1>0) {
	    memmove(zbuf+p1,zbuf,zlen);
	    memset(zbuf,' ',p1);
	}
	memset(zbuf+p1+zlen,' ',x-(p1+zlen));
    }
    zbuf[x]=0;
    zlen=x;
    return 0;
}

//////////////
// COPY
//////////////
str &str::copy(const str &s) {
    if (&s == this) return *this;
    if (zsize<s.zlen+1) grow(s.zlen+1);
    if (zsize<s.zlen+1) return *this;
    if (s.zsize) memcpy(zbuf,s.zbuf,s.zlen+1);
    else if (zsize) zbuf[zlen=0]=0;
    zlen=s.zlen;
    return *this;
}

str &str::copy(const char *str1) {
    int l=strlen(str1);
    if (zsize<l+1) grow(l+1);
    if (zsize<l+1) return *this;
    memcpy(zbuf,str1,l+1);
    zlen=l;
    return *this;
}

//
// FIND
//
int str::find(const char *str,int x,int icase) {
    if (x>=zlen) return -1;
    char *cp;
    cp=(!icase)?strstr(&zbuf[x],str):strcasestr(&zbuf[x],str);
    return (cp)?cp-zbuf:-1;
}

//
// GROW
//
void str::grow(int l) {
    if (zsize<=0) zbuf=(char *)malloc(l);
    else zbuf=(char *)realloc(zbuf,l);
    if (!zbuf) zlen=zsize=0;
    zsize=l;
}

///////////////////
// operator
///////////////////
str &str::operator=(const str &s) {
    if (&s == this) return *this;
    if (zsize<s.zlen+1) grow(s.zlen+1);
    if (zsize<s.zlen+1) return *this;
    if (s.zsize) memcpy(zbuf,s.zbuf,s.zlen+1);
    else if (zsize) zbuf[zlen=0]=0;
    zlen=s.zlen;
    return *this;
}

str &str::operator=(const char *str1) {
    int l=strlen(str1);
    if (zsize<l+1) grow(l+1);
    if (zsize<l+1) return *this;
    memcpy(zbuf,str1,l+1);
    zlen=l;
    return *this;
}

bool str::operator<(const char *str1) {
    return  strcmp(zbuf,str1) < 0 ;
}

bool str::operator>=(const str &str1) {
    return  strcmp(zbuf,str1.zbuf) >= 0 ;
}

bool str::operator>=(const char *str1) {
    return  strcmp(zbuf,str1) >= 0 ;
}

bool str::operator<(const str &str1) {
    return  strcmp(zbuf,str1.zbuf) < 0 ;
}

str str::operator+(const str &s2) {
    str s0;
    int l=zlen+s2.zlen;
    s0.zbuf=(char *)malloc(l+1);
    if (!s0.zbuf) return s0;
    s0.zsize=l+1;
    s0.zlen=l;
    if (zlen>0) memcpy(s0.zbuf,zbuf,zlen);
    if (s2.zlen>0) memcpy(&s0.zbuf[zlen],s2.zbuf,s2.zlen);
    s0.zbuf[l]=0; // incase s2.zlen=0
    return s0;
}

str str::operator+(char c) {
    int l=zlen+1;
    if (zsize<l+1) grow(zsize*2);
    if (zsize<l+1) return *this;
    zbuf[zlen++]=c;
    zbuf[zlen]=0;
    return *this;
}

str operator+(const char *s1,const str &s2) {
    str s0;
    int s1l=strlen(s1);
    int l = s1l + s2.zlen;
    s0.zbuf=(char *)malloc(l+1);
    if (!s0.zbuf) return s0;
    s0.zsize=l+1;
    s0.zlen=l;
    if (s1l) memcpy(s0.zbuf,s1,s1l);
    if (s2.zlen) memcpy(&s0.zbuf[s1l],s2.zbuf,s2.zlen);
    s0.zbuf[l]=0;
    return s0;
}

char str::operator[](int i) {
    return zbuf[i];
}

//
// PRINTF
//
int str::format(const char *fmt,...) {
    va_list ap;
    if (zsize==0) return -1;
    va_start(ap,fmt);
    int rc=vsnprintf(zbuf,zsize,fmt,ap);
    zlen = (rc < zsize) ? rc : zsize-1;
    va_end(ap);
    return rc;
}

//
// REPLACE
//
int str::change(const char *str,int x,int l1,int l2) {
    if (l2<=0) l2=strlen(str);
    if (l1<=0) l1=l2;
    if (zlen+l2-l1>=zsize) return -1;
    memmove(zbuf+x+l2,zbuf+x+l1,zlen-(x+l1));
    memmove(zbuf+x,str,l2);
    return 0;
}

//
// TRIM
//
int str::trim(int x) {
    if (zlen<=0) return -1;
    int i;
    if (x&1) {
	int p1=0;
	for (i=0;i<zlen;i++) if (isgraph(zbuf[i])) {
	    p1=i;
	    break;
	}
	if (p1) {
	    zlen-=p1;
	    memmove(zbuf,&zbuf[p1],zlen+1);
	}
    }
    if (x&2) {
	for (i=zlen-1;i>=0;i--) if (isgraph(zbuf[i])) {
	    zlen=i+1;
	    zbuf[zlen]=0;
	    break;
	}
    }
    return 0;
}

