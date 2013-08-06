#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

const char* gethome() {
    const char *h = getenv("HOME");
    return h ? h : "";
}

struct hist {
    char hfn[PATH_MAX];
    int  hx,hy,hoff;
} ;

class history {
public:
    int put(hist &h);
private:
    struct hist zh[10];
};

int history::put(hist &h) {
    char path[PATH_MAX];
    if (h.hfn[0]==0) return -1;
    char *p=realpath(h.hfn,path);
    if (!p) return -1;
    //strncpy(zmsg,path,sizeof(zmsg)-1);
    //zmsg[sizeof(zmsg)-1]=0;
    return 0;
}

