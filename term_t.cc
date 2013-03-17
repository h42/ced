#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "term.h"

using namespace std;

int main() {
    term dsp;
    dsp.setmode(1);
    int i,j;
    dsp.cup(1,1);
    dsp.eos();
    while (1) {
	int c=dsp.get();
	if (c==F12) break;
	i=(i+1)%14;
	j=i%7;
	switch (j) {
	    case 0:
		dsp.fg0();
		break;
	    case 1:
		dsp.fg1();
		break;
	    case 2:
		dsp.fg2();
		break;
	    case 3:
		dsp.fg3();
		break;
	    case 4:
		dsp.fg4();
		break;
	    case 5:
		dsp.fg5();
		break;
	    case 6:
		dsp.fg6();
		break;
	    case 7:
		dsp.fg7();
		break;
	}
	char buf[80];
	sprintf(buf,"i=%d j=%d hey",i,j);
	dsp.puts(buf,0,i+1,5);
    }
    return 0;
}
