/*
 * tests.h
 *
 *  Created on: May 3, 2017
 *      Author: tgil
 */

#ifndef TESTS_H_
#define TESTS_H_

#include <stfy/son.h>

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
	TEST_CASE_OPEN_OBJ,
	TEST_CASE_CLOSE_OBJ,
	TEST_CASE_OPEN_ARRAY,
	TEST_CASE_CLOSE_ARRAY
};

#if defined __cplusplus
extern "C" {
#endif


test_case_t * test_case_get(int i);
int test_case_count();


#if defined __cplusplus
}
#endif


#endif /* TESTS_H_ */
