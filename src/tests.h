/*
 * tests.h
 *
 *  Created on: May 3, 2017
 *      Author: tgil
 */

#ifndef TESTS_H_
#define TESTS_H_

#include <sapi/son.h>

#define TEST_CASE_STR_SIZE 64
#define TEST_CASE_DATA_SIZE 64

typedef union {
	char str[TEST_CASE_STR_SIZE];
	u8 data[TEST_CASE_DATA_SIZE];
	u32 unum;
	s32 num;
	float fnum;
} test_value_t;

typedef struct {
	char key[SON_KEY_NAME_SIZE];
	char access[SON_ACCESS_NAME_CAPACITY];
	u8 type;
	test_value_t primary_value;
	test_value_t secondary_value;
} test_case_t;

enum {
	TEST_CASE_CLOSE_OBJ = SON_TOTAL,
	TEST_CASE_CLOSE_ARRAY,
	TEST_CASE_TOTAL
};

#if defined __cplusplus
extern "C" {
#endif


void test_init(int max_depth, int seed);
test_case_t * test_get_case(u8 test);
int test_get_count();
void test_show_case(const test_case_t * test);


#if defined __cplusplus
}
#endif


#endif /* TESTS_H_ */
