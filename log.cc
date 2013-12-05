
//ccinclude
#ifndef _LOG_H
#define _LOG_H
#ifndef _STDIO_H
#include <stdio.h>
#endif

class log {
public:
    log(const char *fn);
    ~log() {if (fh) fclose(fh);}
    int put(const char *s);
private:
    char fn[256];
    FILE *fh;
};

#endif
//ccinclude

log::log(const char *fn="temp") {
    fh = fopen(fn,"w");
}

int log::put(const char *s) {
    if (!fh) return -1;
    fprintf(fh,s);
    return 0;
}

int xmain() {
    return 0;
}
