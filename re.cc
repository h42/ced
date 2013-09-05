#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <alloca.h>

#define AST 1
#define QM  2
#define LB  3
#define RB  4
#define NOT 5

//ccinclude
class re {
public:
    ~re();
    re();
    int open(const char *ws,int sensitive);
    void close();
    int wild(unsigned char *buf, int len);
public:
    int upd_t(unsigned char *s, int sensitive, int not0);
    int zopened;
    unsigned char zw[256];
    char *zt[256];
    int zlen,zlen2,zfast,zrlen;
};
//ccinclude

/////////////////////////
// String Search Stuf
/////////////////////////

re::re() {
    zopened=0;
    zlen=zlen2=zfast=zrlen=0;
    for (int i=0;i<256;i++) {
        zt[i]=0; zw[0]=0;
    }
}

re::~re() {
    close();
}

void re::close() {
    if (!zopened) return;
    int j;
    for (j=0;j<zlen;j++) {
        if (zt[j]) {
            free(zt[j]);
            zt[j]=0;
        }
    }
    zopened=0;
}

int re::upd_t(unsigned char *s, int sensitive, int not0) {
    int i,val;
    char c, *cp;
    cp=(char *)malloc(256);
    if (!cp) {
        close();
        return -1;
    }
    if (not0) {
        memset(cp,1,256);
        val=0;
    }
    else {
        memset(cp,0,256);
        val=1;
    }
    for (i=0;i<256;i++) {
	c=s[i];
	if (!c) break;
	if (sensitive) cp[(int)c]=val;
	else {
            cp[toupper(c)]=val;
            cp[tolower(c)]=val;
	}
    }
    zt[zlen++]=cp;
    zlen2++;
    return 0;
}

int re::open(const char *ws,int sensitive) {
    int i,j,rc,state=0,esc=0,not0,fast=1,cl=0;
    unsigned char c,jump[256],ws2[256];
    if (zopened) close();
    zlen=zlen2=zfast=zrlen=0;
    for (i=0;i<256;i++) {
	c=ws[i];
	if (esc==1) {
            esc=0;
	}
	else {
	    if (c=='*') c=AST;
	    else if (c=='?') c=QM;
	    else if (c=='[') c=LB;
	    else if (c==']') c=RB;
	    else if (c=='^') c=NOT;
	    else if (c=='\\') {
                esc=1;
                continue;
	    }
	}

	ws2[cl]=c;
        if (c) cl++;

	if (!c && state) {
            close();
            return -1;
	}
	if (state==0) {
	    if (!c) {
                if (zlen==0) {
                    close();
                    return -1;
		}
                if (fast) {
                    zfast=1;
                    memset(zw,cl,256);
		    cl--;
		    for (i=0;i<cl;i++) {
                        if (sensitive) zw[(int)ws2[i]]=cl-i;
			else {
                            zw[toupper(ws2[i])]=cl-i;
                            zw[tolower(ws2[i])]=cl-i;
			}
		    }
		}
                return 0;
	    }
	    if (c==AST || c==QM) {
                zw[zlen++]=c;
                fast=0;
	    }
	    else if (c==LB) {
                state=2;
                fast=0;
	    }
	    else {
                jump[0]=c;jump[1]=0;
                rc=upd_t(jump,sensitive,0);
                if (rc) return -1;
	    }
	}
	else if (state==2) {
            j=not0=0;
            if (c==NOT) not0=1;
	    else if (c==RB) {
                    close();
                    return -1;
	    }
	    else jump[j++]=c;
	    state=3;
	}
	else if (state==3) {
	    if (c==RB) {
		if (j==0) {
                    close();
                    return -1;
		}
		jump[j]=0;
		state=0;
                rc=upd_t(jump,sensitive,not0);
                if (rc) return -1;
	    }
	    else if (j<255) jump[j++]=c;
	}
    }
    zopened=1;
    return 0;
}

#define comptab(p1,c2) (zt[p1][(int)c2])

int re::wild(unsigned char *buf,int len0) {
    int i,j,t,eq,lp;
    int p1,p2,wildstack[2],wp,wild,len2,wlen,wlen2;
    unsigned char c,c2,*pbuf;
    if (zfast) {
        lp=zlen; //accurate since no * or []
	for (i=lp-1,j=lp-1;j>=0;i--,j--) {
	    eq=comptab(j,buf[i]);
	    while (!eq) {
                t=zw[(int)buf[i]];  // zw used as jump vector in fast by open
		i += (t < lp-j) ? lp-j : t;
                if (i>=len0) return -1;
		j=lp-1;
		eq=comptab(j,buf[i]);
	    }
	}
        zrlen=zlen;
	return i+1;
    }
    wlen=zlen;
    wlen2=zlen2;
    for (i=0;i<=len0-wlen2;i++) {
	p1=p2=wp=wild=0;
        len2=len0-i;
	pbuf=&buf[i];
	while (1) {
	    if (p1>=wlen) {
                    zrlen=p2;
		    return i;
	    }
            c=zw[p1]; // so we can check for * or ?
	    c2=pbuf[p2];
	    if (c==AST) {
		wp=1;
		while (1) {
                    c=zw[++p1];
		    if (p1>=wlen) {
                        zrlen=len0-i;
                        return i;
		    }
		    wildstack[0]=p1; // do after ++p1
		    if  (c != AST) break;
		}
		wild=1;
	    }

	    // check here instaed of before c=='*' check to detect case
	    // where x* matches last char of input.
	    if (p2>=len2) break;

	    if (c==QM || comptab(p1,c2)) { //c should ONLY = ? or 0->comtab
		if (wild==1) {
                    wildstack[1]=p2+1;
                    wild=0;
		}
		p1++;
		p2++;
	    }
	    else { /* c != c2 */
		if (wild==1) p2++;
		else {
		    if (wp) {
                        p1=wildstack[0];
                        p2=wildstack[1];
                        wild=1;
		    }
		    else break;
		}
	    }
	}
    }
    return -1;
}
