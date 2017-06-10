/*
 * tests.c
 *
 *  Created on: May 3, 2017
 *      Author: tgil
 */

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tests.h"


typedef struct {
	int array_counter;
	int type;
	char access[SON_ACCESS_NAME_CAPACITY];
} test_stack_t;

#define TEST_STACK_SIZE 16

static int m_depth;
static int m_max_depth;
static int m_count;
static int m_counter;
static test_case_t m_test_case;
static test_stack_t m_stack[TEST_STACK_SIZE];
static int m_stack_loc;
static char m_random_string[TEST_CASE_STR_SIZE];
static const char * generate_random_string();
static test_case_t * generate_test_case();
static test_case_t * create_test_case();

#define TEST_CASE_SIZE 40

test_case_t test_cases[TEST_CASE_SIZE];


void test_init(int max_depth, int seed){
	test_case_t * test_case;
	m_max_depth = max_depth;
	m_count = TEST_CASE_SIZE;
	m_counter = 0;
	m_depth = 0;
	m_stack[0].type = SON_OBJ;
	m_stack[0].array_counter = 0;
	m_stack_loc = 0;
	memset(m_stack[m_stack_loc].access, 0, SON_ACCESS_NAME_CAPACITY);
	srand(seed);

	for(int i = 0; i < TEST_CASE_SIZE; i++){
		test_case = create_test_case();
		memcpy(test_cases + i, test_case, sizeof(test_case_t));
	}

}

test_case_t * test_get_case(u8 test){
	if( test < TEST_CASE_SIZE ){
		return test_cases + test;
	}
	return 0;
}

test_case_t * create_test_case(){
	test_case_t * test_case;
	do {
		test_case = generate_test_case();
	} while(test_case == 0);
	return test_case;
}


test_case_t * generate_test_case(){

	if( m_counter == m_count ){
		return 0;
	}

	m_test_case.type = rand() % TEST_CASE_TOTAL;

	if( m_test_case.type == SON_DATA ){
		return 0;
	}

	if( m_count - (m_counter+1) < m_stack_loc ){
		m_test_case.type = TEST_CASE_CLOSE_OBJ;
	} else {
		sprintf(m_test_case.key, "k%d", m_counter);

		if( m_stack[m_stack_loc].type == SON_OBJ ){
			if( m_stack[m_stack_loc].access[0] == 0 ){
				strncpy(m_test_case.access, m_test_case.key, SON_ACCESS_NAME_SIZE);
			} else {
				snprintf(m_test_case.access, SON_ACCESS_NAME_SIZE, "%s.%s", m_stack[m_stack_loc].access, m_test_case.key);
			}
		} else if( m_stack[m_stack_loc].type == SON_ARRAY ){
			if( m_test_case.type == SON_ARRAY ){
				return 0;
			}
			snprintf(m_test_case.access, SON_ACCESS_NAME_SIZE, "%s[%d]", m_stack[m_stack_loc].access, m_stack[m_stack_loc].array_counter);
			m_stack[m_stack_loc].array_counter++;
		} else {
			printf("What stack type %d\n", m_stack[m_stack_loc].type);
		}
	}


	switch(m_test_case.type){
	case SON_STRING:
		strncpy(m_test_case.primary_value.str, generate_random_string(), TEST_CASE_STR_SIZE-1);
		strncpy(m_test_case.secondary_value.str, generate_random_string(), TEST_CASE_STR_SIZE-1);
		break;
	case SON_FLOAT:
		m_test_case.primary_value.fnum = rand() * 1.0 / RAND_MAX;
		m_test_case.secondary_value.fnum = rand() * 1.0 / RAND_MAX;
		break;

	case SON_NUMBER_U32:
		m_test_case.primary_value.unum = rand();
		m_test_case.secondary_value.unum = rand();
		break;
	case SON_NUMBER_S32:
		m_test_case.primary_value.num = rand() - INT_MAX/2;
		m_test_case.secondary_value.num = rand() - INT_MAX/2;
		break;
	case SON_FALSE:
	case SON_NULL:
	case SON_TRUE:
		m_test_case.primary_value.num = 0;
		m_test_case.secondary_value.num = 0;
		break;

	case SON_OBJ:
	case SON_ARRAY:
		if( m_stack_loc < TEST_STACK_SIZE-1 ){
			m_stack_loc++;
			m_stack[m_stack_loc].type = m_test_case.type;
			m_stack[m_stack_loc].array_counter = 0;
			strncpy(m_stack[m_stack_loc].access, m_test_case.access, SON_ACCESS_NAME_SIZE);
		} else {
			return 0;
		}
		break;
	case TEST_CASE_CLOSE_OBJ:
	case TEST_CASE_CLOSE_ARRAY:

		if( m_stack[m_stack_loc].type == SON_OBJ ){
			m_test_case.type = TEST_CASE_CLOSE_OBJ;
		} else if( m_stack[m_stack_loc].type == SON_ARRAY ){
			m_test_case.type = TEST_CASE_CLOSE_ARRAY;
		}

		if( m_stack_loc > 0 ){
			m_stack_loc--;
		} else {
			return 0;
		}
		break;
	}

	m_counter++;
	return &m_test_case;

}

int test_get_count(){
	return m_count;
}

void test_show_case(const test_case_t * test){
	if( (test->type != TEST_CASE_CLOSE_OBJ) && (test->type != TEST_CASE_CLOSE_ARRAY) ){
		printf("Access: %s", test->access);
	}

	switch(test->type){
	case SON_STRING:
		printf("->str p:%s s:%s", test->primary_value.str, test->secondary_value.str);
		break;
	case SON_FLOAT:
		printf("->float p:%f s:%f", test->primary_value.fnum, test->secondary_value.fnum);
		break;
	case SON_NUMBER_U32:
		printf("->unum p:%ld s:%ld", test->primary_value.unum, test->secondary_value.unum);
		break;
	case SON_NUMBER_S32:
		printf("->num p:%ld s:%ld", test->primary_value.num, test->secondary_value.num);
		break;
	case SON_FALSE:
		printf("->bool p:false s:false");
		break;
	case SON_NULL:
		printf("->null p:null s:null");
		break;
	case SON_TRUE:
		printf("->bool p:true s:true");
		break;
	case SON_OBJ:
		printf(" {");
		break;
	case SON_ARRAY:
		printf(" [");
		break;
	case TEST_CASE_CLOSE_OBJ:
		printf("}");
		break;
	case TEST_CASE_CLOSE_ARRAY:
		printf("]");
		break;
	}
}

int is_alpha_num(char c){
	const char * alnum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=`~";
	int i;
	int len = strlen(alnum);
	for(i=0; i < len; i++){
		if( alnum[i] == c){
			return 1;
		}
	}
	return 0;
}

const char * generate_random_string(){
	int i;
	for(i = 0; i < TEST_CASE_STR_SIZE-1; i++){
		do {
			m_random_string[i] = rand();
		} while( is_alpha_num(m_random_string[i]) == 0 );
	}
	m_random_string[i] = 0;
	return m_random_string;
}
