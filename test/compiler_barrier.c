/*
 * Compiler barrier
 */
int A, B;
int _main()
{
	A = B+1;
	B = 8;
	return A;
}

/*
gcc -S -O2
...............
__main:
LFB0:
	movq	_B@GOTPCREL(%rip), %rdx
	movq	_A@GOTPCREL(%rip), %rcx
	movl	(%rdx), %eax
	movl	$8, (%rdx)
	addl	$1, %eax
	movl	%eax, (%rcx)
	ret

*/
int _foo()
{
	A = B+1;
	asm("":::"memory");
	B = 8;
	return A;
}
/*
gcc -S -O2
............
__foo:
LFB1:
	movq	_B@GOTPCREL(%rip), %rcx
	movq	_A@GOTPCREL(%rip), %rdx
	movl	(%rcx), %eax
	addl	$1, %eax
	movl	%eax, (%rdx)
	movl	$8, (%rcx)
	movl	(%rdx), %eax
	ret
*/
