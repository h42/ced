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
#include "glob.h"

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
// CHECKSAVE
//
int ced::checksave() {
    char sx[80];
    if (!zedit) return 0;
    dsp.request("Do you want to save current file (y/n)?", sx, sizeof(sx),1);
    while (1) {
        if (sx[0]=='y' || sx[0]=='Y') return savefile();
        else if (sx[0]=='n' || sx[0]=='N') return 0;
        dsp.request("Do you want to save current file (y/n)?",
             sx, sizeof(sx),1);
    }
    return 0;
}

//
// NEWFILE
//
int ced::newfile() {
    int rc = checksave();
    if (rc<0) return -1;
    ll.log_off();
    if (zfn[0]) zhist.push(zfn,zx,zy,zoff,ztop);
    ll.reset();
    ll.ins(0,"",0);
    zx=zy=ztop=zoff=zedit=zedit2=0;
    zfn[0]=0;
    disppage(ztop);
    ll.log_on();
    zu.reset();
    return 0;
}

//
// RECENT
//
void ced::recent() {
    dsp.clrscr();
    hist *h;
    int i,c;
    for (i=0; i<MAXHIST && i<9; i++) {
        h = zhist.pop(i);
        if (!h->hfn[0]) break;
        printf("%d. %s\n", i+1, h->hfn);
    }
    printf("\nSelect file: ");
    c = dsp.get() - '1'; // NOTE ( - '1' ) for zero offset
    if (c>=0 && c<=8 && c<i) {
        h = zhist.pop(c);
        char buf[512]; // loadfile updates history vector so necessary
        strncpy(buf,h->hfn,511);
        loadfile(buf);
    }
    else disppage(ztop);
}


//
// SWAPFILE
//
int ced::swapfile(int x) {
    if (x>=MAXHIST) return -1;
    if (zfn[0]) zhist.push(zfn,zx,zy,zoff,ztop);
    hist *h = zhist.pop(x);
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
    if (!zfn[0]) {
        char fn[256];
        dsp.request("Enter filename: ",fn,sizeof(fn));
        if (!fn[0]) {
            snprintf(zmsg,sizeof(zmsg),"File not save - no name");
            return -1;
        }
        strncpy(zfn,fn,sizeof(zfn)-1); zfn[sizeof(zfn)-1]=0;
    }
    char *buf;
    int fd = open(zfn, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (fd<0) {
        snprintf(zmsg,sizeof(zmsg),"Unable to open file %d - %s",fd,strerror(errno));
        return -1;
    }
    int l;
    for (int ii=0; ii<ll.size(); ii++) {
        buf=ll.get(ii);
	l=strlen(buf);
	if (l+1 >= zfbufsize) {
	    zfbufsize *= 2;
	    zfbuf = (char *)realloc(zfbuf,zfbufsize);
	    if (l+1 >= zfbufsize) {
                ll.reset(); // THROW AND ERROR
	    }
	}
	memcpy(zfbuf,buf,l);
	zfbuf[l]=10;
        int rc=write(fd, zfbuf, l+1);
        if (rc != l+1) {
            snprintf(zmsg,sizeof(zmsg),"write failed - rc= %d - %s",
                rc,strerror(errno));
            return -1;
        }
    }
    close(fd);
    jcpy(zmsg,"File saved");
    zedit=0;
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
    if ((rc=checksave())) return rc;

    if (!fn || !fn[0]) dsp.request("Enter filename: ",fn2,sizeof(fn2));
    else strcpy(fn2,fn);
    if (!fn2[0]) {
        getfn(dsp, ".", ".cc", fn2, sizeof(fn2));
        if (!fn2[0]) {
            disppage(ztop);
            return -1;
        }
    }

    zu.reset();
    ll.log_off();
    rc=readf(fn2);
    ll.log_on();

    if (rc) strcpy(zmsg,"Open failed");
    else {
	ztabcomp=(strcmp(fn2,"makefile")==0 ||
		  strcmp(fn2,"Makefile")==0) ? 1 : 0;
        hist *h=zhist.pop(fn2);
        if (h) {
            zx=h->hx;
            zy=h->hy;
            zoff=h->hoff;
            ztop=h->htop;
            zedit=zedit2=0;
        }
        else  zx=zy=ztop=zoff=zedit=zedit2=0;
        if (zy<0 || zy>ll.size()-1 || ztop<0 || ztop>ll.size()-1) ztop=zy=0;
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

    ll.reset();
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
                        ll.reset(); // THROW AN ERROR
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
