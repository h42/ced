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
    if (zre.open(sx, sensitive)) {
        snprintf(zmsg,sizeof(zmsg),"bad search string");
        return;
    }
    rfind();
}

void ced::rfind() {
    if (! zre.isOpened()) {
        snprintf(zmsg,sizeof(zmsg),"no search string");
        return;
    }
    int i,j,rc;
    pline();
    for (i=0; zy+i < ll.size(); i++) {
        gline2(zy+i);
        j = i==0 ? zx+1 : 0;
        rc = zre.wild((unsigned char *)&zbuf2[j], zbufl2-j);
        if (rc<0) continue;
        zy=zy+i;
        zx=j+rc;
        gline();
        if (zy >= ztop+zmaxy-2) {
            ztop=zy;
            disppage(ztop);
        }
        upoff();
        return;
    }
    snprintf(zmsg,sizeof(zmsg),"not found");
}
