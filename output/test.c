#include <stdio.h>
#include "vaarg.h"


int foo(char *fmt, ...)	{
	int arg_int;
	char arg_char;
	char *tmp = fmt;

	var_init(&fmt);	
	
	while(*tmp) {
		if(*tmp == '%') {
			tmp++;
			if(*tmp == '\0') {
				printf("%");
				break;
			}
			switch (*tmp){
				case 'd':
					var_arg(arg_int, int);
					printf("%d", arg_int);
					break;
				case 'c':
					var_arg(arg_char, char);
					printf("%c", arg_char);
					break;
				default:
					break;
			}
		}
		else {
			printf("%c", *tmp);
		}
		tmp++;
	}

	return 0;
}

int main(){
	foo("Hello!!\nx = %d y = %d\n", 100, 200);
	return 0;
}
