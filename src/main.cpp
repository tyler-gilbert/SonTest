#include <cstdio>
#include <cstring>
#include <stfy/sys.hpp>
#include <stfy/fmt.hpp>
#include <stfy/var.hpp>

#include "tests.h"

#define DATA_TEST_SIZE 64
#define CREATE_TEST_FILE "/home/create_test.son"
#define MAX_DEPTH 16

static int create_primary_values(const char * file_name);
static int edit_primary_values(const char * file_name);
static int verify_primary_values(const char * file_name);
static int verify_secondary_values(const char * file_name);

static int write_test_value(Son<MAX_DEPTH> & son, test_case_t * test_case, const test_value_t & value);
static int edit_test_value(Son<MAX_DEPTH> & son, test_case_t * test_case, test_value_t & value);
static int verify_test_value(Son<MAX_DEPTH> & son, test_case_t * test_case, test_value_t & value);

int main(int argc, char * argv[]){
	printf("Starting SON Test\n");

	int i;
	int tab_counter;
	int tab_count = 0;
	test_case_t * test_case;
	while(1){
		printf("{\n");
		tab_count=1;
		test_init(4, Timer::get_clock_usec());

		for(i=0; i < test_get_count(); i++){
			test_case = test_get_case(i);

			for(tab_counter = 0; tab_counter < tab_count; tab_counter++){
				printf("\t");
			}
			if( test_case->type == SON_OBJ ){
				tab_count++;
			} else if( test_case->type == TEST_CASE_CLOSE_OBJ ){
				tab_count--;
			} else if( test_case->type == SON_ARRAY ){
				tab_count++;
			} else if( test_case->type == TEST_CASE_CLOSE_ARRAY ){
				tab_count--;
			}
			test_show_case(test_case);
			printf("\n");
		}
		printf("}\n");

		unlink("/home/test.son");

		if( create_primary_values("/home/test.son") < 0 ){
			printf("create primary value file failed\n");
			exit(1);
		}

		if( verify_primary_values("/home/test.son") < 0 ){
			printf("Failed to verify primary test values\n");
			exit(1);
		}

		if( edit_primary_values("/home/test.son") < 0 ){
			printf("Failed to edit primary test values\n");
			exit(1);
		}

		if( verify_secondary_values("/home/test.son") < 0 ){
			printf("Failed to verify secondary test values\n");
			exit(1);
		}
	}

	printf("Test complete\n");

	return 0;
}

int create_primary_values(const char * file_name){
	int i;
	test_case_t * test_case;
	Son<MAX_DEPTH> son;

	if( son.create(file_name) < 0 ){
		printf("Failed to open file %s (%d)\n", file_name, son.err());
		perror("Errno Message");
		return -1;
	}

	if( son.open_obj("") < 0 ){
		printf("Failed to create root object (%d)\n", son.err());
		return -1;
	}

	for(i=0; i < test_get_count(); i++){
		test_case = test_get_case(i);
		printf("Write value:");
		test_show_case(test_case);
		printf("\n");
		if( write_test_value(son, test_case, test_case->primary_value) < 0 ){
			printf("Failed to write test value (%d)\n", son.err());
			return -1;
		}

	}

	if( son.close(true) < 0 ){
		printf("Failed to close file (%d-%d)\n", son.err(), son.fileno());
		perror("Error message");
		return -1;
	}

	return 0;
}

int edit_primary_values(const char * file_name){
	int i;
	test_case_t * test_case;
	Son<MAX_DEPTH> son;

	if( son.open_edit(file_name) < 0 ){
		return -1;
	}

	for(i=0; i < test_get_count(); i++){
		test_case = test_get_case(i);
		printf("Edit value:");
		test_show_case(test_case);
		printf("\n");
		if( edit_test_value(son, test_case, test_case->secondary_value) < 0 ){
			printf("Failed to edit test value (%d)\n", son.err());
			return -1;
		}

	}

	if( son.close(true) < 0 ){
		printf("Failed to close file (%d)\n", son.err());
		perror("Error message");
		return -1;
	}

	return 0;
}

int verify_primary_values(const char * file_name){
	Son<MAX_DEPTH> son;
	test_case_t * test_case;
	int i;

	errno = 0;
	if( son.open_read(file_name) < 0 ){
		printf("Failed to open file %s (%d)\n", file_name, son.err());
		perror("Errno Message");
		return -1;
	}

	for(i=0; i < test_get_count(); i++){
		test_case = test_get_case(i);
		printf("Verify Primary:");
		test_show_case(test_case);
		printf("\n");
		if( verify_test_value(son, test_case, test_case->primary_value) < 0 ){
			printf("Failed to verify test value (%d)\n", son.err());
			return -1;
		}
	}

	if ( son.close() < 0 ){
		printf("Failed to close file (%d)\n", son.err());
		perror("Error message");
		return -1;
	}
	return 0;
}

int verify_secondary_values(const char * file_name){
	Son<MAX_DEPTH> son;
	test_case_t * test_case;
	int i;

	if( son.open_read(file_name) < 0 ){
		printf("Failed to open file %s (%d)\n", file_name, son.err());
		perror("Errno Message");
		return -1;
	}

	for(i=0; i < test_get_count(); i++){
		test_case = test_get_case(i);
		printf("Verify Secondary:");
		test_show_case(test_case);
		printf("\n");
		if( verify_test_value(son, test_case, test_case->secondary_value) < 0 ){
			printf("Failed to verify test value (%d)\n", son.err());
			return -1;
		}
	}

	if( son.close(true) < 0 ){
		printf("Failed to close file (%d)\n", son.err());
		perror("Error message");
		return -1;
	}

	return 0;
}

int verify_test_value(Son<MAX_DEPTH> & son, test_case_t * test_case, test_value_t & value){
	test_value_t verify_value;
	memset(verify_value.str, 0, TEST_CASE_STR_SIZE);

	switch(test_case->type){
	case SON_OBJ:
	case TEST_CASE_CLOSE_OBJ:
	case SON_ARRAY:
	case TEST_CASE_CLOSE_ARRAY:
		return 0;
	case SON_STRING:
		if( son.read_str(test_case->access, verify_value.str, TEST_CASE_STR_SIZE) < 0 ){
			printf("Failed to read key '%s' (%d)\n", test_case->access, son.err());
			return -1;
		}
		if( strncmp(verify_value.str, value.str, TEST_CASE_STR_SIZE) != 0){
			printf("Failed. Values not equivalent %s != %s\n", verify_value.str, value.str);
			return -1;
		}

		printf("Verified %s: %s\n", test_case->access, verify_value.str);
		return 0;
	case SON_FLOAT:
		verify_value.fnum = son.read_float(test_case->access);
		if( verify_value.fnum == value.fnum ){
			printf("Verified %s: %f\n", test_case->access, verify_value.fnum);
			return 0;
		}
		printf("Failed to verify %s: %f != %f\n", test_case->access, verify_value.fnum, value.fnum);
		return -1;
	case SON_NUMBER_U32:
		verify_value.unum = son.read_unum(test_case->access);
		if( verify_value.unum == value.unum ){
			printf("Verified %s: %ld\n", test_case->access, verify_value.unum);
			return 0;
		}
		printf("Failed to verify %s: %ld != %ld\n", test_case->access, verify_value.unum, value.unum);
		return -1;
	case SON_NUMBER_S32:
		verify_value.num = son.read_num(test_case->access);
		if( verify_value.num == value.num ){
			printf("Verified %s: %ld\n", test_case->access, verify_value.num);
			return 0;
		}
		printf("Failed to verify %s: %ld != %ld\n", test_case->access, verify_value.num, value.num);
		return -1;
	case SON_TRUE:
		if( son.read_bool(test_case->access) == true ){
			printf("Verified %s: true\n", test_case->access);
			return 0;
		}
		return -1;
	case SON_FALSE:
		if( son.read_bool(test_case->access) == false ){
			printf("Verified %s: false\n", test_case->access);
			return 0;
		}
		return -1;
	case SON_NULL:
		return 0;
	}

	printf("Bad test case type\n");
	return -1;
}

int edit_test_value(Son<MAX_DEPTH> & son, test_case_t * test_case, test_value_t & value){

	switch(test_case->type){
	case SON_STRING: return son.edit(test_case->access, value.str);
	case SON_FLOAT: return son.edit(test_case->access, value.fnum);
	case SON_NUMBER_U32: return son.edit(test_case->access, value.unum);
	case SON_NUMBER_S32: return son.edit(test_case->access, value.num);
	case SON_TRUE: return son.edit(test_case->access, true);
	case SON_FALSE: return son.edit(test_case->access, false);
	case SON_NULL: return 0; //can't edit null
	case SON_OBJ:
	case TEST_CASE_CLOSE_OBJ:
	case SON_ARRAY:
	case TEST_CASE_CLOSE_ARRAY:
		return 0;
	}

	printf("Bad test case type\n");
	return -1;
}

int write_test_value(Son<MAX_DEPTH> & son, test_case_t * test_case, const test_value_t & value){

	switch(test_case->type){
	case SON_STRING: return son.write(test_case->key, value.str);
	case SON_FLOAT: return son.write(test_case->key, value.fnum);
	case SON_NUMBER_U32: return son.write(test_case->key, value.unum);
	case SON_NUMBER_S32: return son.write(test_case->key, value.num);
	case SON_TRUE: return son.write(test_case->key, true);
	case SON_FALSE: return son.write(test_case->key, false);
	case SON_NULL: return son.write(test_case->key, (const char*)0);
	case SON_OBJ:
		return son.open_obj(test_case->key);
	case TEST_CASE_CLOSE_OBJ:
		return son.close_obj();
	case SON_ARRAY:
		return son.open_array(test_case->key);
	case TEST_CASE_CLOSE_ARRAY:
		return son.close_array();
	}

	printf("Bad test case type %d\n", test_case->type);
	return -1;
}

