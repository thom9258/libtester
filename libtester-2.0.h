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

#ifndef LIBTESTER_FILE_TARGET 
#define LIBTESTER_FILE_TARGET stdout
#endif /*LIBTESTER_FILE_TARGET*/

#ifdef LIBTESTER_NO_COLOR
#  define LIBTESTER_COLOR(COLOR)
#else /*NOT LIBTESTER_NO_COLOR*/
#  define LIBTESTER_COLOR(COLOR) fprintf(LIBTESTER_FILE_TARGET, COLOR)
#endif /*LIBTESTER_NO_COLOR*/

#define LIBTESTER_RED     "\033[0;31m"
#define LIBTESTER_GREEN   "\033[0;32m"
#define LIBTESTER_BLUE    "\033[0;34m"
#define LIBTESTER_RESET   "\033[0m"
#define LIBTESTER_DEFAULT LIBTESTER_BLUE

#define LIBTESTER_MAX_TESTS 128

#define TPRINT(MSG, ...)                                                     \
	fprintf(LIBTESTER_FILE_TARGET, MSG, ##__VA_ARGS__)

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
	_libtester_pre_function_call(__FILE__, #FUNCTION);                        \
	FUNCTION;                                                                  \
	_libtester_post_function_call(#FUNCTION);                                              \

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
    
typedef struct {
	time_t start;
	double elapsed_sec;
}libtester_timer;

static libtester_timer _timer = {0, 0};

/* ==============================================================================
   Timer functionality start
   =============================================================================*/

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
		LIBTESTER_COLOR(LIBTESTER_RED);
        TPRINT("[test L%d] ( %s ) failed. %s\n", _line, _expr_str, msg);
	}                                                                 
	else {                                                            
		LIBTESTER_COLOR(LIBTESTER_GREEN);
        TPRINT("[test L%d] ( %s ) passed. %s\n", _line, _expr_str, msg);
	}                                                                 
	LIBTESTER_COLOR(LIBTESTER_RESET);
    return _result;
}

void
_libtester_pre_function_call(const char* _file_name, const char* _test_name)
/**
 * _libtester_pre_function_call() - Print pre-function formality of testlib.
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

	LIBTESTER_COLOR(LIBTESTER_DEFAULT);
	TPRINT(LIBTESTER_BOLDLINE);
	TPRINT(LIBTESTER_BOLDLINE);
	TPRINT("[ TEST %d ]  File %s\n", _evaluated_tests_count, _file_name);
	LIBTESTER_COLOR(LIBTESTER_RESET);
	TPRINT("%s:\n", _test_name);
	LIBTESTER_COLOR(LIBTESTER_DEFAULT);
	TPRINT(LIBTESTER_BOLDLINE);                                      
	LIBTESTER_COLOR(LIBTESTER_RESET);

	libtester_timer_reset(&_timer);
	libtester_timer_start(&_timer);
}

void
_libtester_post_function_call(const char* _test_name)
/**
 * _libtester_post_function_call() - Print post-function formality of testlib.
 *
 * Prints:
 * - Visually eye-catching passed/fail
 * - execution time
 * - outcome summation of tests for given function
 */
{
	libtester_timer_stop(&_timer);

	char* outcome = NULL;
	_test_names[_evaluated_tests_count] = (char*)_test_name;
	if (_curr_test_errors == 0) {
		LIBTESTER_COLOR(LIBTESTER_GREEN);
		outcome = (char*)"PASSED";
		_test_statuses[_evaluated_tests_count] = 0;
	}
	else {
		LIBTESTER_COLOR(LIBTESTER_RED);
		outcome = (char*)"FAILED";
		_test_statuses[_evaluated_tests_count] = 1;
	}
	TPRINT(LIBTESTER_LINE);                                      
	TPRINT(">>>[[ TEST %d %s ]]  ->", _evaluated_tests_count, outcome);
	TPRINT("  (errors=%d/%d, time=%fs)\n",
	          _curr_test_errors, _total_test_errors,                  
	          _timer.elapsed_sec);
	TPRINT(LIBTESTER_BOLDLINE);                                      
	TPRINT(LIBTESTER_BOLDLINE);                                      
	LIBTESTER_COLOR(LIBTESTER_RESET);
	_evaluated_tests_count++;
}

int
test_summary(void)
/**
 * test_summary() - Print summary of all tests.
 */
{
	int i;
    int n_failed = 0;
	LIBTESTER_COLOR(LIBTESTER_DEFAULT);
	TPRINT(LIBTESTER_BOLDLINE);
	TPRINT(LIBTESTER_BOLDLINE);
	TPRINT("Tests Summary:\n");
	TPRINT(LIBTESTER_LINE);
	LIBTESTER_COLOR(LIBTESTER_RESET);
	TPRINT("Total tests = (%d)\n", _evaluated_tests_count);

	for (i = 0; i < _evaluated_tests_count; i++) {
		if (_test_statuses[i] == 0) {
			LIBTESTER_COLOR(LIBTESTER_GREEN);
			TPRINT("\t [%d]  %s\n", i, _test_names[i]);
		} else {
			LIBTESTER_COLOR(LIBTESTER_RED);
			TPRINT("\t [%d]  %s\n", i, _test_names[i]);
            n_failed++;
		}
	}
	LIBTESTER_COLOR(LIBTESTER_DEFAULT);
	TPRINT(LIBTESTER_BOLDLINE);
	TPRINT(LIBTESTER_BOLDLINE);
	LIBTESTER_COLOR(LIBTESTER_RESET);

    if (n_failed > 0) {
        LIBTESTER_COLOR(LIBTESTER_RED);
        TPRINT("FAILED %d UNIT-TESTS.\n", n_failed);
        LIBTESTER_COLOR(LIBTESTER_RESET);
        return -1;
    }
    LIBTESTER_COLOR(LIBTESTER_GREEN);
    TPRINT("ALL UNIT-TESTS PASSED.\n");
    LIBTESTER_COLOR(LIBTESTER_RESET);
    return 0;
}

#define TEST_END() return test_summary()

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
