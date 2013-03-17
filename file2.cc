#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <malloc.h>
#include "list.h"
#include "term.h"


//ccinclude

#define jcpy(dst,src) {strncpy(dst,src,sizeof(dst)-1); dst[sizeof(dst)-1]=0;}

#define MAXFN 256
class file {
public:
    file();
    ~file() {};
    int savef();
    int loadf(const char *fn, list &list, term &dsp);
    int readf(const char *fn, list &ll);
    char *zmsg() {return zzmsg;}
private:
    void growbuf();
    char zfn[MAXFN];
    char *filebuf;
    int fbufsize;
    int zr0;
    int zoverride;
    char zzmsg[256];
};

//ccinclude

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

void file::growbuf() {
    fbufsize *= 2;
    filebuf = (char *)realloc(filebuf,fbufsize);
}

file::file() {
    zfn[0]=zr0=zoverride=0;
    fbufsize=1000;
    filebuf=(char *)malloc(fbufsize);
}

//
// SAVEFILE
//
int file::savef() {
    //if (zedit) dsp.request("Enter filename: ",fn2,sizeof(fn2));
    return 0;
}

//
// LOADFILE
//
int file::loadf(const char *fn, list &list, term &dsp) {
    int rc;
    char fn2[256];
    if (!fn || !fn[0]) dsp.request("Enter filename: ",fn2,sizeof(fn2));
    else strcpy(fn2,fn);
    if (!fn2[0]) return -1;
    rc=readf(fn2,list);
    if (rc) strcpy(zzmsg,"Open failed");
    sprintf(zzmsg,"%s loaded",fn2);
    return rc;
}

int file::readf(const char *fn, list &ll) {
    /*

      DO NOT RETURN WITHOUT RESETTING SIGSTUF

    */
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
		filebuf[p2]=0;
		ll.ins(filebuf);
		p2=0;
	    }
	    else {
		if (c>=' ' || c==9) filebuf[p2++]=c;
		else {
		    if (!zoverride) filebuf[p2++]='~';
		    else {
			if (c) filebuf[p2++]=c;
			else {
			    filebuf[p2++]='~';
			    zr0=1;
			}
		    }
		}
		if (p2 >= fbufsize) {
		    growbuf();
		    if (p2 >= fbufsize) {
			ll.init();
		    }
		}
	    }
	}
    }
    if (p2>0) {
	filebuf[p2]=0;
	p++;
	ll.ins(filebuf);
    }
    //zmsg[0]=0;
    fclose(f1);
    //sigstuf(0);
    jcpy(zfn,fn);
    return p;
}

/*
void tfile() {
    file f;
    list ll;
    f.readf("temp",ll);
    for (int i=0; i<ll.size(); i++) printf("sx=%s\n",ll.get(i));
}
*/
