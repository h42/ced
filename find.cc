#include <stdio.h>
#include <string.h>
#include "list.h"
#include "term.h"
#include "file.h"
#include "ced.h"

void ced::find(int sensitive) {
    char sx[128];
    dsp.request("Enter search string:", sx, sizeof(sx));
    if (sx[0]==0) {
        snprintf(zmsg,sizeof(zmsg),"find cancelled");
        return;
    }
}

void ced::rfind() {
}
