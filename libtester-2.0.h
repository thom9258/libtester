/* 
# TESTLIB
	libtester - Library testing utility for c-c++ libraries,
	written in ansi-c99. Designed for test-driven development of
	of libraries & programs.

## ORIGINAL AUTHOR

Thomas Alexgaard Jensen (![github/Alexgaard](https://github.com/thom9258))

## LICENSE

Copyright (c) Thomas Alexgaard Jensen
This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software in
   a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

For more information, please refer to
![Zlib License](https://choosealicense.com/licenses/zlib/)
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> /*stdout*/
#include <stdarg.h> /*va_list*/
#include <stdint.h> /*uint32_t, uint8_t*/
#include <time.h> /*clock_t, clock(), CLOCKS_PER_SEC*/

#ifndef _LIBTESTER_H_
#define _LIBTESTER_H_
    
/* ==============================================================================
   Printing functionality start
   =============================================================================*/
    

#define LIBTESTER_FILE_TARGET stdout

#ifdef LIBTESTER_NO_COLOR
#  define LIBTESTER_SET_COLOR(COLOR)
#else /*NOT LIBTESTER_NO_COLOR*/
#  define LIBTESTER_SET_COLOR(COLOR) fprintf(LIBTESTER_FILE_TARGET, COLOR)
#endif /*LIBTESTER_NO_COLOR*/

#define LIBTESTER_RED     "\033[0;31m"
#define LIBTESTER_GREEN   "\033[0;32m"
#define LIBTESTER_BLUE    "\033[0;34m"
#define LIBTESTER_RESET   "\033[0m"
#define LIBTESTER_DEFAULT LIBTESTER_BLUE

#ifndef LIBTESTER_MAX_TESTS
#  define LIBTESTER_MAX_TESTS 128
#endif

#define LIBTESTER_FUNCTIONSTRING(FN) (const char*)#FN

#define LIBTESTER_LINE                                                                \
"----------------------------------------------------------------------------\n"

#define LIBTESTER_BOLDLINE                                                            \
"============================================================================\n"

/* ==============================================================================
   Testing functionality start
   =============================================================================*/

#define TEST_IF(_EXPR)                                                         \
	_LIBTESTER_TEST_FUNCTION(!(_EXPR), #_EXPR, __LINE__, NULL)

#define TESTM_IF(_EXPR, _MSG)                                                  \
	_LIBTESTER_TEST_FUNCTION(!(_EXPR), #_EXPR, __LINE__, _MSG)

#define TEST_UNIT(FUNCTION)                                                     \
	_libtester_pre_unit_call(__FILE__, #FUNCTION);                        \
	FUNCTION;                                                                  \
	_libtester_post_unit_call(#FUNCTION);                                              \

#define LIBTESTER_IS_MEMALIGNED(POINTER, BYTE_COUNT) \
	( (((uintptr_t)(const void*)(POINTER)) & (BYTE_COUNT-1)) == 0)


/* ==============================================================================
   Global test context
   =============================================================================*/

static char*    _test_names[LIBTESTER_MAX_TESTS]      = {0};
static char     _test_statuses[LIBTESTER_MAX_TESTS]   = {0};
static int      _evaluated_tests_count                = 0;
static int      _curr_test_errors                     = 0;
static int      _total_test_errors                    = 0;
static uint32_t _randseed                             = 2501;
static FILE*    _filetarget                           = NULL;
   
/* ==============================================================================
   Timer functionality start
   =============================================================================*/

size_t _lt_printf(const char* fmt, ...) {
    va_list termlist;
    va_list filelist;

    va_start(termlist, fmt);
    if (_filetarget != NULL) {
        va_copy(filelist, termlist);
        vfprintf(_filetarget, fmt, filelist);
        va_end(filelist);
    }

    vfprintf(stdout, fmt, termlist);
    va_end(termlist);
}
   
#define lt_print(msg, ...)    _lt_printf(msg, ##__VA_ARGS__)
#define lt_bgprint(msg, ...)  _lt_printf(LIBTESTER_BLUE  msg LIBTESTER_RESET, ##__VA_ARGS__)
#define lt_errprint(msg, ...) _lt_printf(LIBTESTER_RED   msg LIBTESTER_RESET, ##__VA_ARGS__)
#define lt_okprint(msg, ...)  _lt_printf(LIBTESTER_GREEN msg LIBTESTER_RESET, ##__VA_ARGS__)
 

/* ==============================================================================
   Timer functionality start
   =============================================================================*/
    
typedef struct {
	time_t start;
	double elapsed_sec;
}libtester_timer;

static libtester_timer _timer = {0, 0};

void
libtester_timer_start(libtester_timer* _t)
{
	if (_t == NULL)
		return;
	_t->start = clock();
}

void
libtester_timer_stop(libtester_timer* _t)
{
	if (_t == NULL)
		return;
	_t->elapsed_sec += (double)(clock() - _t->start) / CLOCKS_PER_SEC;
}

void
libtester_timer_reset(libtester_timer* _t)
{
	if (_t == NULL)
		return;
	_t->elapsed_sec = 0;
	_t->start = 0;
}

char
_LIBTESTER_TEST_FUNCTION(char _result, const char* _expr_str, int _line, const char* _msg)
/**
 * _LIBTESTER_TEST_FUNCTION() - Test expression for pass/fail 
 * @arg1: Expression result
 * @arg2: Expression string
 * @arg3: Line number
 * @arg4: External message for test
 *
 * Tests function expression.
 */
{
	char* msg;
    if (_msg == NULL) msg = (char*)"";
    else              msg = (char*)_msg;
	_total_test_errors++;                                                   

	if (_result != 0) {                                                   
		_curr_test_errors++;
		LIBTESTER_SET_COLOR(LIBTESTER_RED);
        lt_errprint("[test L%d] ( %s ) failed. %s\n", _line, _expr_str, msg);
        //TPRINT("[test L%d] ( %s ) failed. %s\n", _line, _expr_str, msg);
	}                                                                 
	else {                                                            
		LIBTESTER_SET_COLOR(LIBTESTER_GREEN);
        //TPRINT("[test L%d] ( %s ) passed. %s\n", _line, _expr_str, msg);
        lt_okprint("[test L%d] ( %s ) passed. %s\n", _line, _expr_str, msg);
	}                                                                 
	LIBTESTER_SET_COLOR(LIBTESTER_RESET);
    return _result;
}

void
_libtester_pre_unit_call(const char* _file_name, const char* _test_name)
/**
 * _libtester_pre_unit_call()- Print status before unit-test.
 * @arg1: File name
 * @arg2: Test function name
 *
 * Prints:
 * - Visually eye-catching start of test section
 * - test file name
 * - function to test & its line number
 */
{
	_curr_test_errors = 0;                     
	_total_test_errors = 0;                           

	lt_bgprint(LIBTESTER_BOLDLINE);
	lt_bgprint("[ TEST %d ]  File:", _evaluated_tests_count);
	lt_print(" %s\n", _file_name);
	lt_bgprint(LIBTESTER_BOLDLINE);                                      

	libtester_timer_reset(&_timer);
	libtester_timer_start(&_timer);
}

void
_libtester_post_unit_call(const char* _test_name)
/**
 * _libtester_post_unit_call() - Print status after unit-test.
 *
 * Prints:
 * - Visually eye-catching passed/fail
 * - execution time
 * - outcome summation of tests for given function
 */
{
	libtester_timer_stop(&_timer);
	lt_bgprint(LIBTESTER_LINE);                                      
	_test_names[_evaluated_tests_count] = (char*)_test_name;
    
	if (_curr_test_errors == 0) {
		_test_statuses[_evaluated_tests_count] = 0;
        lt_okprint(">>>[[ TEST %d PASSED ]]  ->  (errors=%d/%d, time=%fs)\n",
                   _evaluated_tests_count,
                   _curr_test_errors,
                   _total_test_errors,
                   _timer.elapsed_sec);
        lt_bgprint(LIBTESTER_BOLDLINE);
	}
	else {
		_test_statuses[_evaluated_tests_count] = 1;
        lt_errprint(">>>[[ TEST %d FAILED ]]  ->  (errors=%d/%d, time=%fs)\n",
                    _evaluated_tests_count,
                    _curr_test_errors,
                    _total_test_errors,
                    _timer.elapsed_sec);
        lt_bgprint(LIBTESTER_BOLDLINE);
	}
	_evaluated_tests_count++;
    lt_print("\n");
}


void
tlargs_shift(int* _argc, char*** _argv) {
    if (_argc == NULL || _argv == NULL)
        return;
    *_argv++;
    *_argc--;
}

int
tlcontext_begin(int _argc, char** _argv) {

}
   
int
tlcontext_end(void)
/**
 * tlcontext_end() - Print summary of all tests.
 */
{
	int i;
    int n_failed = 0;
	lt_bgprint(LIBTESTER_BOLDLINE "Tests Summary:\n" LIBTESTER_LINE);

	for (i = 0; i < _evaluated_tests_count; i++) {
		if (_test_statuses[i] == 0) {
			lt_okprint("\t[%d]  %s\n", i, _test_names[i]);
		} else {
			lt_errprint("\t[%d]  %s\n", i, _test_names[i]);
            n_failed++;
		}
	}
    lt_bgprint(LIBTESTER_BOLDLINE);

    if (n_failed > 0) {
        lt_errprint("FAILED %d UNIT-TESTS OUT OF %d\n" , n_failed, _evaluated_tests_count);
        return -1;
    }
    lt_okprint("PASSED ALL %d UNIT-TESTS.\n", _evaluated_tests_count);
    return 0;
}
    
void
tl_rand_seed(uint32_t _seed)
/**
 * tl_rand_seed() - Set seed for random functions.
 * @arg1: the seed used to generate random value.
 */
{
	_randseed = _seed;
}

uint32_t
tl_rand_uint(void)
/**
 * tl_rand_uint() - generate seeded random uint32.
 * @arg1: the seed used to generate random value.
 *
 * Generate psuedo-random unsigned integer of 32 bit.
 * seed value is incremented after use.
 *
 * Return: generated number.
 */
{
	static const uint32_t mangle_1     = 0x3D73;
	static const uint32_t mangle_2     = 0xC0AE5;
	static const uint32_t mangle_3     = 0x5208DD0D;
	static const uint32_t mangle_shift = 13;
	uint32_t n;
	n = (_randseed << mangle_shift) ^ _randseed;
	n *= n * mangle_1;
	n += mangle_2;
	n *= n;
	n += mangle_3;
	_randseed++;
	return n;
}

uint8_t
tl_rand_bool(void)
/**
 * tl_rand_uint() - generate seeded random bool (0/1).
 * @arg1: the seed used to generate random value.
 *
 * Generate psuedo-random boolean (0/1).
 * Effectively returns even/odd of tl_rand_uint().
 *
 * Return: generated number.
 */
{
	return (tl_rand_uint() & 1);
}

uint32_t
tl_rand_ubetween(uint32_t _min, uint32_t _max)
/**
 * tl_rand_ubetween() - generate seeded random between specified min/max.
 * @arg1: the seed used to generate random value.
 * @arg2: minimum generated value.
 * @arg3: maximum generated value.
 *
 * Generate psuedo-random unsigned integer 32 bit, generated number is modified
 * to fit between specified max/min).
 *
 * Return: generated number.
 */
{
	return ((tl_rand_uint() % (_max - _min + 1)) + _min);
}

#endif /*_LIBTESTER_H_*/

#ifdef __cplusplus
}
#endif
