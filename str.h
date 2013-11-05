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
