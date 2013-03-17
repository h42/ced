#include <stdio.h>
#include <string.h>
#include "list.h"

//using namespace std;

int main() {
    int i,n=10;
    char sx[80], *s;
    list ll;
    for (i=0;i<n;i++) {
	sprintf(sx,"%d",i);
	ll.ins(-1,sx,0);
    }
    ll.del(4);
    ll.del(4);
    for (i=0;i<ll.size();i++) {
	s=ll.get(i);
	printf("i=%d s=%s\n",i,s);
    }
    // printf("i=%d s=%s\n",i,s);
    //getchar();
    return 0;
}
