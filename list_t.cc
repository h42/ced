#include <stdio.h>
#include <string.h>
#include "list.h"

//using namespace std;

int main() {
    int i,n=10;
    char sx[80], *s;
    list ll;


    ll.log_on();
    ll.set_log_ptr(0);
    for (i=0;i<5;i++) {
	sprintf(sx,"%d",i);
	ll.ins(-1,sx,0);
    }

    ll.set_log_ptr(1);
    for (i=5;i<10;i++) {
	sprintf(sx,"%d",i);
	ll.ins(-1,sx,0);
    }
    ll.rollback(1);

    ll.set_log_ptr(1);
    for (i=10;i<15;i++) {
	sprintf(sx,"%d",i);
	ll.ins(-1,sx,0);
    }

    /*
    ll.log_on();
    ll.del(4);
    ll.del(4);
    ll.log_off();
    ll.rollback(0);
    */

    for (i=0;i<ll.size();i++) {
	s=ll.get(i);
	printf("i=%d s=%s\n",i,s);
    }
    // printf("i=%d s=%s\n",i,s);
    //getchar();
    return 0;
}
