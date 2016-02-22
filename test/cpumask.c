/*
 * Test const int *const things...
 */

int main(void)
{
	int a[1] = { 100 };
	const int *const b = a;			/* b is a const pointer to const int */
	const int *c;				/* c is a pointer to const int */
	int *const d;				/* d is a const pointer to int */

/*
	b = a;
	*b = 100;
*/

/*
	c = a;
	*c = 100;
*/

/*
	d = a;
	*d = 100;
*/
}
