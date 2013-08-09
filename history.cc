#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>

#include "term.h"
#include "list.h"
#include "ced.h"

//ccinclude
const int N = 10, PM=256;

struct hist {
    char hfn[PM];
    int  hx,hy,hoff;
} ;

class history {
public:
    history() {init();}
    int  del(const char *);
    void display();
    void init();
    int  push(hist &h);
    void read();
    void write();
private:
    struct hist zh[N];
};
//ccinclude

const char* gethome() {
    const char *h = getenv("HOME");
    return h ? h : "";
}

void history::read() {
    int rc,i,l;
    char cwd[PM],*cp,sx[PM];
    FILE *fh;
    sprintf(sx,"%s/.ced",getenv("HOME"));
    if (access(sx,W_OK)) {
        mkdir(sx,0775);
    }
    cp=get_current_dir_name();
    if (!cp) return;
    if (cp) {
        strncpy(cwd,cp,sizeof(cwd)-1);
        cwd[sizeof(cwd)-1]=0;
        free(cp);
        l=strlen(cwd);
        for (i=0;i<l;i++) {
            if (cwd[i]==0) break;
            if (cwd[i]=='/') cwd[i]='_';
        }
    }
    snprintf(sx,sizeof(sx)-1,"%s/.ced/%s",getenv("HOME"),cwd);
    sx[sizeof(sx)-1]=0;
    fh=fopen(sx,"rb");
    if (fh) {
        rc=fread(&zh,1,sizeof(zh),fh);
        if (rc!=sizeof(zh)) init();
        fclose(fh);
    }
    else init();
    return ;
}

void history::write() {
    int fh,i,l;
    char cwd[PM],*cp,sx[PM];
    cp=get_current_dir_name();
    if (!cp) return;
    if (cp) {
	strncpy(cwd,cp,sizeof(cwd)-1);
	cwd[sizeof(cwd)-1]=0;
        free(cp);
	l=strlen(cwd);
	for (i=0;i<l;i++) {
	    if (cwd[i]==0) break;
	    if (cwd[i]=='/') cwd[i]='_';
	}
    }
    sprintf(sx,"%s/.ced",getenv("HOME"));
    if (access(sx,W_OK)) {
        mkdir(sx,0775);
    }
    snprintf(sx,sizeof(sx)-1,"%s/.ced/%s",getenv("HOME"),cwd);
    sx[sizeof(sx)-1]=0;
    fh=open(sx,O_CREAT | O_WRONLY,0);
    if (fh>0) {
	fchmod(fh,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
	    S_IROTH | S_IWOTH);
        ::write(fh,&zh,sizeof(zh));
	close(fh);
    }
}

int history::del(const char *fn) {
    int i,j;
    for (i=0;i<N;i++) {
        if (strcmp(fn, zh[i].hfn)==0) zh[i].hfn[0]=0;
    }
    for (i=0,j=0;i<N;i++) {
        if (zh[i].hfn[0]) {
            if (i != j) {
                zh[j]=zh[i];
                zh[i].hfn[0]=0;
            }
            j++;
        }
    }
    return 0;
}

int history::push(hist &h) {
    if (h.hfn[0]==0) return -1;
    //char path[PATH_MAX];
    //char *p=realpath(h.hfn,path);
    //if (!p) return -1;
    //strcpy(h.hfn,path);
    del(h.hfn);
    for (int i=N-1;i>0;i--) zh[i]=zh[i-1];
    zh[0]=h;
    return 0;
}

void history::init() {
    int i;
    for (i=0;i<N;i++) {
        zh[i].hfn[0]=0;
    }
}

void history::display() {
    for (int i=0;i<N;i++) {
        printf("%2d. %s\n",i+1,zh[i].hfn);
    }
}

void testhist() {
    int i;
    hist h1;
    history h;
    for (i=0;i<N;i++) {
        sprintf(h1.hfn,"temp%d",i+1);
        h.push(h1);
    }
    i=5;
    sprintf(h1.hfn,"temp%d",i+1);
    h.push(h1);

    h.write();
    h.init();
    h.display();

    h.read();
    h.display();
    printf("%d %ld %ld\n",PM,sizeof(h1),sizeof(h));
}
