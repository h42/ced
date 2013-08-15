#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <malloc.h>
#include "list.h"
#include "term.h"
#include "ced.h"

#define jcpy(dst,src) {strncpy(dst,src,sizeof(dst)-1); dst[sizeof(dst)-1]=0;}

#define MAXFN 256
char * jlib_readlink(char *fn,char *fn2,int size) {
    int rc;
    struct stat st1;
    rc=lstat(fn,&st1);
    // return fn if stat fails(file does not exist???) or not symlink
    if (rc || !S_ISLNK(st1.st_mode)) return fn;
    rc=readlink(fn,fn2,size);
    if (rc>0) {
	if (rc>=size) return 0;
	fn2[rc]=0;
    }
    return fn2;
}

//
// NEWFILE
//
int ced::newfile() {
    if (zfn[0]) zhist.push(zfn,zx,zy,zoff,ztop);
    ll.init();
    ll.ins(0,"",0);
    zx=zy=ztop=zoff=0;
    return 0;
}

//
// SWAPFILE
//
int ced::swapfile(int x) {
    if (zfn[0]) zhist.push(zfn,zx,zy,zoff,ztop);
    hist *h = zhist.pop(1);
    if (h->hfn[0]) {
        loadfile(h->hfn);
    }
    return 0;
}

//
// SAVEFILE
//
int ced::savefile() {
    pline();
    if (!zedit) {
	jcpy(zmsg,"File not changed");
	return 0;
    }
    char *buf;
    int fd = open(zfn, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (fd<0) {
	snprintf(zmsg,sizeof(zmsg),"Unable to open file - %s",strerror(errno));
	return 0;
    }
    int l;
    for (int i=0; i<ll.size(); i++) {
	buf=ll.get(i);
	l=strlen(buf);
	if (l+1 >= zfbufsize) {
	    zfbufsize *= 2;
	    zfbuf = (char *)realloc(zfbuf,zfbufsize);
	    if (l+1 >= zfbufsize) {
		ll.init();
	    }
	}
	memcpy(zfbuf,buf,l);
	zfbuf[l]=10;
	write(fd, zfbuf, l+1);
    }
    close(fd);
    jcpy(zmsg,"File saved");
    return 0;
}

//
// LOADFILE
//
int ced::loadfile(const char *fn) {
    int rc;
    char fn2[256];
    pline();
    if (zfn[0]) zhist.push(zfn,zx,zy,zoff,ztop);
    //if (zedit) checksave();

    if (!fn || !fn[0]) dsp.request("Enter filename: ",fn2,sizeof(fn2));
    else strcpy(fn2,fn);
    if (!fn2[0]) return -1;
    rc=readf(fn2);
    if (rc) strcpy(zmsg,"Open failed");
    else {
        hist *h=zhist.pop(fn2);
        if (h) {
            zx=h->hx;
            zy=h->hy;
            zoff=h->hoff;
            ztop=h->htop;
        }
	disppage(ztop);
	sprintf(zmsg,"%s loaded",fn2);
    }
    return rc;

}


int ced::readf(const char *fn) {
    // DO NOT RETURN WITHOUT RESETTING SIGSTUF
    int p,i,p2,rc;
    char c,buf2[512];
    FILE *f1;
    f1=fopen(fn,"rb");
    if (!f1) return -1;

    //sigstuf(1);

    ll.init();
    p2=0;
    p=0;
    while ((rc=fread(buf2,1,sizeof(buf2),f1)) > 0) {
	for (i=0;i<rc;i++) {
	    c=buf2[i];
	    if (!zoverride && c==13) continue;
	    if (c==10) {
		zfbuf[p2]=0;
		ll.ins(zfbuf);
		p2=0;
	    }
	    else {
		if (c>=' ' || c==9) zfbuf[p2++]=c;
		else {
		    if (!zoverride) zfbuf[p2++]='~';
		    else {
			if (c) zfbuf[p2++]=c;
			else {
			    zfbuf[p2++]='~';
			    zr0=1;
			}
		    }
		}
		if (p2 >= zfbufsize) {
		    zfbufsize *= 2;
		    zfbuf = (char *)realloc(zfbuf,zfbufsize);
		    if (p2 >= zfbufsize) {
			ll.init();
		    }
		}
	    }
	}
    }
    if (p2>0) {
	zfbuf[p2]=0;
	p++;
	ll.ins(zfbuf);
    }
    //zmsg[0]=0;
    fclose(f1);
    //sigstuf(0);
    jcpy(zfn,fn);
    return p;
}
