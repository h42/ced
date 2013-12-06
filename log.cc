#include <stdarg.h>

//ccinclude
#ifndef _LOG_H
#define _LOG_H
#ifndef _STDIO_H
#include <stdio.h>
#endif

class log {
public:
    log(const char *fn) {open(fn);}
    log() {fh=0;}
    FILE *open(const char *fn="temp.log");
    ~log() {if (fh) fclose(fh);}
    int put(const char *fmt, ...);
private:
    char fn[256];
    FILE *fh;
};

#endif
//ccinclude

FILE *
log::open(const char *fn) {
    fh = fopen(fn,"w");
    return fh;
}

int log::put(const char *fmt, ...) {
    if (!fh) return -1;
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    fprintf(fh,"%s\n",buf);
    return 0;
}

int xmain() {
    return 0;
}
