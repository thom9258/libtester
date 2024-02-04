#include "../libtester-2.0.h"

int
plus(int a, int b)
{
	return a+b;
}

void
test_if_simple(void)
{
	TEST_IF(2+2==5);
	TEST_IF(2+2==4);
	TEST_IF(4+4==7);
}

void
test_random(int N)
{
	int i;
	for (i = 0; i < N; i++) {
        unsigned int a;
        unsigned int b;
		a = tl_rand_ubetween(0, 300);
		b = tl_rand_ubetween(0, 300);
        TPRINT("a: %d, b: %d\n", a, b);
		TESTM_IF(plus(a,b) == a+b, "Succsesfully calculated a corect result");
	}
}

void
break_if_simple(void)
{
	TEST_IF(2+3==5);
	TEST_IF(2+2==5);
	//BREAK_IF(4+4==7);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TEST_UNIT(test_if_simple());
	TEST_UNIT(test_random(10));
	TEST_UNIT(break_if_simple());

    TEST_END();
}
