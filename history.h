const int N = 10, PM=256;

struct hist {
    char hfn[PM];
    int  hx,hy,hoff,htop;
} ;

class history {
public:
    history() {init();}
    int  del(const char *);
    void display();
    void init();
    hist * pop(int n=0);
    hist * pop(char *fn);
    int  push(const char *fn,int x,int y,int off, int top);
    void read();
    void write();
private:
    struct hist zh[N];
};
