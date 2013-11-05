class re {
public:
    ~re();
    re();
    int open(const char *ws,int sensitive);
    void close();
    int wild(unsigned char *buf, int len);
    bool isOpened() {return (zopened != 0);}
public:
    int upd_t(unsigned char *s, int sensitive, int not0);
    int zopened;
    unsigned char zw[256];
    char *zt[256];
    int zlen,zlen2,zfast,zrlen;
};
