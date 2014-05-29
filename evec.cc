#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <vector>

enum {INS=0, REM, UPD};

class bstr {
private:
    char *s;
    int slen;
public:
    bstr() {s=0; slen=0;}
    ~bstr() {
        if (s) {
            free(s);
            s=0;
            slen=0;
        }
    }
    bstr& operator=(bstr& b) {
        this->update(b.s,b.slen);
        return *this;
    }
    void update(const char *istr, int len=0) {
        if (istr) {
            int l= len>=0 ? strlen(istr) : len;
            if (slen < l+1) {
                if (s) s=(char *)realloc(s,l+1);
                else s = (char*) malloc(l+1);
                slen=l+1;
                if (!s) throw ("OOM");
            }
            memcpy(s,istr,l+1);
        }
        else if (s) s[0]=0;
    }
    void display() {printf("s=%s\n",s?s:"nill");}
};

class ulog {
private:
    int cmd;
    int pos;
    bstr str;
public:
    ulog() {cmd=pos=0;}
    void update(int icmd, int ipos, const char *istr, int slen=0) {
        cmd=icmd;
        pos=ipos;
        str.update(istr,slen);
    }
    ulog& operator=(ulog& u) {
        this->cmd=u.cmd;
        this->pos=u.pos;
        this->str=u.str;
        return *this;
    }
    void display() {printf("%d %d\n",cmd,pos);str.display();}
};

void tu() {
    bstr s1;
    s1.update("hey there");
    s1.display();

    ulog u,u2,u3;
    u.update(7,0,"hey now");
    u.display();
    u.update(7,0,"brown cow");
    u.display();
    u2=u;
    u.display();

}

int main() {
    printf("%d %d %d\n", INS, REM, UPD);
    tu();
    std::vector<char *> v;
}
