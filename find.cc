#include <stdio.h>
#include <string.h>
#include "list.h"
#include "term.h"
#include "ced.h"

void ced::change() {
    char sx[128];
    zchange=false;
    dsp.request("Enter search string:", sx, sizeof(sx));
    if (sx[0]==0) {
        snprintf(zmsg,sizeof(zmsg),"change cancelled");
        return;
    }
    if (zre.open(sx, 1)) {
        snprintf(zmsg,sizeof(zmsg),"bad search string");
        return;
    }
    int rc = dsp.request("Enter replace string:", zchangebuf, sizeof(zchangebuf));
    if (!rc) zchange=true;
    snprintf(zmsg,sizeof(zmsg),"change primed");
}

void ced::rchange() {
    int rc,l1,l2;
    if (! zre.isOpened()) {
        snprintf(zmsg,sizeof(zmsg),"no search string");
        return;
    }
    if (! zchange) {
        snprintf(zmsg,sizeof(zmsg),"change not primed");
        return;
    }
    if (zx+zre.zrlen>=zbufl) {
        snprintf(zmsg,sizeof(zmsg),"rfind required");
        return;
    }
    rc = zre.wild((unsigned char *)&zbuf[zx], zre.zrlen);
    if (rc != 0) {
        snprintf(zmsg,sizeof(zmsg),"rfind required");
        return;
    }
    l1 = zre.zrlen;
    l2 = strlen(zchangebuf);
    if (l2-l1+zx >= zbufsize) {
        snprintf(zmsg,sizeof(zmsg),"buffer too small");
        return;
    }
    gline(1);
    memmove(&zbuf[zx+l2], &zbuf[zx+l1], zbufl-zx+l1); // memory area may overlap
    memcpy(&zbuf[zx], zchangebuf, l2);
    zbufl=zbufl+l2-l1;
    snprintf(zmsg,sizeof(zmsg),"changed %d %d",l1,l2);
    disppage(ztop);
}
void ced::find(int sensitive) {
    char sx[128];
    zchange=false;
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
    for (i=0; zy+i < zl.size(); i++) {
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
