#define CURRENT_MASK 0xffffe000

struct task_struct {
	int state;
	int prio;
	int x;
};

#define current				\
({					\
	unsigned int *__p;		\
	asm 				\
	(				\
		"movl %1, %%ecx\n\t"	\
		"andl %%esp, %%ecx\n\t"	\
		"movl %%ecx, %0\n\t"	\
		:"=r"(__p)		\
		:"i"(CURRENT_MASK)	\
		:"%ecx"			\
	);				\
	(struct task_struct *)__p;	\
})

int main()
{
	int s,p;
	s = current->state;
	p = current->prio;

	return 0;
}
