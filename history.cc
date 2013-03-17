#include <stdlib.h>

const char *get_home() {
    static const char* sp="";
    const char *h = getenv("HOME");
    return (h ? h : sp);
}


