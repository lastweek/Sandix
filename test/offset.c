#include <stdio.h>

#define offsetof(TYPE, MEMBER) \
	(((unsigned int)) &((TYPE *)0)->MEMBER)

//typeof( ((type *)0)->member ) *__ptr = (ptr);

#define container_of(ptr, type, member) ({			\
	typeof( member ) *__ptr = (ptr);	\
	(type *)( (char *)__ptr - offsetof(type,member) );})

struct tt {
	int a;
	int *b;
	char c;
};

int main()
{
	struct tt t={1,2,3}; 
	short a;
	
	typeof(a) xa1;
	typeof(((struct tt *)0)->a) xa2;
	
	printf("%d %d\n", sizeof(xa1), sizeof(xa2));

	//struct tt *p = container_of(tt.b, struct tt, b);
	return 0;
}
