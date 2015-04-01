#include <stdio.h>

#define com(var)	#var
#define xoxo(var)	var##XX

int main(){
	int u;
	printf("%d\n", __STDC_VERSION__);
	return 0;
}
