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
typedef struct _RE {
    unsigned char w[256];
    char *t[256];
    int len,len2,jump,fast,rlen;
} RE;

RE *re_open(char *ws,int sensitive);
void re_close(RE *re);
int re_wild(RE *re,unsigned char *buf,int len);
//ccinclude

/////////////////////////
// String Search Stuf
/////////////////////////

void re_close(RE *re) {
    int j;
    for (j=0;j<re->len;j++) if (re->t[j]) free(re->t[j]);
    free(re);
}

static int upd_t(RE *re,char *s,int sensitive,int not) {
    int i,val;
    char c,*cp;
    cp=malloc(256);
    if (!cp) {
        re_close(re);
        return -1;
    }
    if (not) {
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
    re->t[re->len++]=cp;
    re->len2++;
    return 0;
}

RE *re_open(char *ws,int sensitive) {
    int i,j,rc,state=0,esc=0,not,fast=1,cl=0;
    unsigned char c,jump[256],ws2[256];
    RE *re;
    re=(RE *)malloc(sizeof(RE));
    memset(re,0,sizeof(*re));
    if (!re) return 0;
    re->len=0;
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
	if (c)cl++;

	if (!c && state) {
            re_close(re);
            return 0;
	}
	if (state==0) {
	    if (!c) {
		if (re->len==0) {
                    re_close(re);
                    return 0;
		}
		if (fast) {
		    re->fast=1;
		    memset(re->w,cl,256);
		    cl--;
		    for (i=0;i<cl;i++) {
			if (sensitive) re->w[(int)ws2[i]]=cl-i;
			else {
                            re->w[toupper(ws2[i])]=cl-i;
                            re->w[tolower(ws2[i])]=cl-i;
			}
		    }
		}
		return re;
	    }
	    if (c==AST || c==QM) {
                re->w[re->len++]=c;
                fast=0;
	    }
	    else if (c==LB) {
                state=2;
                fast=0;
	    }
	    else {
                jump[0]=c;jump[1]=0;
                rc=upd_t(re,jump,sensitive,0);
                if (rc) return 0;
	    }
	}
	else if (state==2) {
	    j=not=0;
	    if (c==NOT) not=1;
	    else if (c==RB) {
		    re_close(re);
		    return 0;
	    }
	    else jump[j++]=c;
	    state=3;
	}
	else if (state==3) {
	    if (c==RB) {
		if (j==0) {
                    re_close(re);
                    return 0;
		}
		jump[j]=0;
		state=0;
		rc=upd_t(re,jump,sensitive,not);
		if (rc) return 0;
	    }
	    else if (j<255) jump[j++]=c;
	}
    }
    return re;
}

#define comptab(p1,c2) (re->t[p1][(int)c2])

int re_wild(RE *re,unsigned char *buf,int len) {
    int i,j,t,eq,lp;
    int p1,p2,wildstack[2],wp,wild,len2,wlen,wlen2;
    unsigned char c,c2,*pbuf;
    if (re->fast) {
	lp=re->len; //accurate since no * or []
	for (i=lp-1,j=lp-1;j>=0;i--,j--) {
	    eq=comptab(j,buf[i]);
	    while (!eq) {
		t=re->w[(int)buf[i]];  // re->w used as jump vector in fast by open
		i += (t < lp-j) ? lp-j : t;
		if (i>=len) return -1;
		j=lp-1;
		eq=comptab(j,buf[i]);
	    }
	}
	re->rlen=re->len;
	return i+1;
    }
    wlen=re->len;
    wlen2=re->len2;
    for (i=0;i<=len-wlen2;i++) {
	p1=p2=wp=wild=0;
	len2=len-i;
	pbuf=&buf[i];
	while (1) {
	    if (p1>=wlen) {
		    re->rlen=p2;
		    return i;
	    }
	    c=re->w[p1]; // so we can check for * or ?
	    c2=pbuf[p2];
	    if (c==AST) {
		wp=1;
		while (1) {
		    c=re->w[++p1];
		    if (p1>=wlen) {
                        re->rlen=len-i;
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
