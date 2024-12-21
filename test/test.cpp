#include "../libtester/libtester.h"

int
plus(int a, int b)
{
	return a+b;
}

void
test_if_simple(void)
{
	TEST(2+2==5);
	TEST(2+2==4);
	TEST(4+4==7);
}

void
test_random(int N)
{
	int i;
	for (i = 0; i < N; i++) {
        int a;
        int b;
		a = lt_rand_ubetween(0, 300);
		b = lt_rand_ubetween(0, 300);
        lt_print("a: %d, b: %d\n", a, b);
		TESTM(plus(a,b) == a+b, "Succsesfully calculated a corect result");
	}
}

void
break_if_simple(void)
{
	TEST(2+3==5);
	TEST(2+2==5);
	//BREAK(4+4==7);
}

void
test_lt_str_eq()
{
    /*Lets use our library to test itself*/
    
    const char* hi = "hi";
    const char* hello = "hello";
    TEST(lt_str_eq(hi, hello) == false);
    TEST(lt_str_eq(hello, hi) == false);
    TEST(lt_str_eq(hi, hi) == true);
    TEST(lt_str_eq(hello, hello) == true);

    TEST(lt_str_eq(NULL, hello) == false);
    TEST(lt_str_eq(hi, NULL) == false);
    TEST(lt_str_eq(NULL, NULL) == false);
}

int main(int argc, char **argv) {
	ltcontext_begin(argc, argv);

	TEST_UNIT(test_if_simple());
	TEST_UNIT(test_random(10));
	TEST_UNIT(break_if_simple());
	TEST_UNIT(test_lt_str_eq());

    return ltcontext_end();
}
