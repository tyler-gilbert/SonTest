#include <cstdio>
#include <cstring>
#include <stfy/fmt.hpp>
#include <stfy/var.hpp>

#include "tests.h"

#define DATA_TEST_SIZE 64
#define CREATE_TEST_FILE "/home/create_test.son"


static int create_primary_values(const char * file_name);
static int edit_primary_values(const char * file_name);
static int verify_primary_values(const char * file_name);
static int verify_secondary_values(const char * file_name);

static int write_test_value(Son<8> & son, test_case_t * test_case, test_value_t & value);
static int edit_test_value(Son<8> & son, test_case_t * test_case, test_value_t & value);
static int verify_test_value(Son<8> & son, test_case_t * test_case, test_value_t & value);

static int create_test_file();
static int verify_test_file();
static int edit_test_file();
static int verify_edit_test_file();

int main(int argc, char * argv[]){
	printf("Starting SON Test\n");


	unlink("/home/test.son");

	if( create_primary_values("/home/test.son") < 0 ){
		printf("create primary value file failed\n");
	}

	if( verify_primary_values("/home/test.son") < 0 ){
		printf("Failed to verify primary test values\n");
	}

	if( edit_primary_values("/home/test.son") < 0 ){
		printf("Failed to edit primary test values\n");
	}

	if( verify_secondary_values("/home/test.son") < 0 ){
		printf("Failed to verify secondary test values\n");
	}

	printf("Test complete\n");

	exit(0);

	unlink(CREATE_TEST_FILE);

	if( create_test_file() < 0 ){
		printf("create_test_file() failed\n");
		exit(1);
	}

	if( verify_test_file() < 0 ){
		printf("verify_test_file() failed\n");
		exit(1);
	}

	if( edit_test_file() < 0 ){
		printf("edit_test_file() failed\n");
		exit(1);
	}

	if( verify_edit_test_file() < 0 ){
		printf("verify_edit_test_file() failed\n");
		exit(1);
	}

	printf("All tests passed\n");

	return 0;
}

int create_primary_values(const char * file_name){
	int i;
	test_case_t * test_case;
	Son<8> son;

	if( son.create(file_name) < 0 ){
		return -1;
	}

	if( son.open_obj("root") < 0 ){
		printf("Failed to create root object");
		return -1;
	}

	for(i=0; i < test_case_count(); i++){
		test_case = test_case_get(i);
		if( write_test_value(son, test_case, test_case->primary_value) < 0 ){
			printf("Failed\n");
			return -1;
		}

	}

	son.close(true);

	return 0;
}

int edit_primary_values(const char * file_name){
	int i;
	test_case_t * test_case;
	Son<8> son;

	if( son.open_edit(file_name) < 0 ){
		return -1;
	}

	for(i=0; i < test_case_count(); i++){
		test_case = test_case_get(i);
		if( edit_test_value(son, test_case, test_case->secondary_value) < 0 ){
			printf("Failed\n");
			return -1;
		}

	}

	son.close(true);

	return 0;
}

int verify_primary_values(const char * file_name){
	Son<8> son;
	test_case_t * test_case;
	int i;

	if( son.open_read(file_name) < 0 ){
		return -1;
	}

	for(i=0; i < test_case_count(); i++){
		test_case = test_case_get(i);
		if( verify_test_value(son, test_case, test_case->primary_value) < 0 ){
			printf("Failed to verify\n");
			return -1;
		}
	}

	son.close();
	return 0;
}

int verify_secondary_values(const char * file_name){
	Son<8> son;
	test_case_t * test_case;
	int i;

	if( son.open_read(file_name) < 0 ){
		return -1;
	}

	for(i=0; i < test_case_count(); i++){
		test_case = test_case_get(i);
		if( verify_test_value(son, test_case, test_case->secondary_value) < 0 ){
			printf("Failed to verify\n");
			return -1;
		}
	}

	son.close();
	return 0;
}

int verify_test_value(Son<8> & son, test_case_t * test_case, test_value_t & value){
	test_value_t verify_value;
	memset(verify_value.str, 0, TEST_CASE_STR_SIZE);

	switch(test_case->type){
	case SON_STRING:
		if( son.read_str(test_case->key, verify_value.str, TEST_CASE_STR_SIZE) < 0 ){
			printf("Failed to read key '%s'\n", test_case->key);
			return -1;
		}
		if( strncmp(verify_value.str, value.str, TEST_CASE_STR_SIZE) != 0){
			printf("Failed to compare values %s != %s\n", verify_value.str, value.str);
			return -1;
		}

		return 0;
	case SON_FLOAT:
		verify_value.fnum = son.read_float(test_case->key);
		if( verify_value.fnum == value.fnum ){
			return 0;
		}
		printf("Failed to verify %s: %f != %f\n", test_case->key, verify_value.fnum, value.fnum);
		return -1;
	case SON_NUMBER_U32:
		verify_value.unum = son.read_unum(test_case->key);
		if( verify_value.unum == value.unum ){
			return 0;
		}
		printf("Failed to verify %s: %ld != %ld\n", test_case->key, verify_value.unum, value.unum);
		return -1;
	case SON_NUMBER_S32:
		verify_value.num = son.read_num(test_case->key);
		if( verify_value.num == value.num ){
			return 0;
		}
		printf("Failed to verify %s: %ld != %ld\n", test_case->key, verify_value.num, value.num);
		return -1;
	case SON_TRUE:
		if( son.read_bool(test_case->key) == true ){
			return 0;
		}
		return -1;
	case SON_FALSE:
		if( son.read_bool(test_case->key) == false ){
			return 0;
		}
		return -1;
	case SON_NULL:
		verify_value.fnum = son.read_float(test_case->key);
		if( verify_value.fnum == value.fnum ){
			return 0;
		}
		return -1;
	}

	printf("Bad test case type\n");
	return -1;
}

int edit_test_value(Son<8> & son, test_case_t * test_case, test_value_t & value){

	switch(test_case->type){
	case SON_STRING: return son.edit(test_case->key, value.str);
	case SON_FLOAT: return son.edit(test_case->key, value.fnum);
	case SON_NUMBER_U32: return son.edit(test_case->key, value.unum);
	case SON_NUMBER_S32: return son.edit(test_case->key, value.num);
	case SON_TRUE: return son.edit(test_case->key, true);
	case SON_FALSE: return son.edit(test_case->key, false);
	case SON_NULL: return son.edit(test_case->key, (const char*)0);
	}

	printf("Bad test case type\n");
	return -1;
}

int write_test_value(Son<8> & son, test_case_t * test_case, test_value_t & value){

	switch(test_case->type){
	case SON_STRING: return son.write(test_case->key, value.str);
	case SON_FLOAT: return son.write(test_case->key, value.fnum);
	case SON_NUMBER_U32: return son.write(test_case->key, value.unum);
	case SON_NUMBER_S32: return son.write(test_case->key, value.num);
	case SON_TRUE: return son.write(test_case->key, true);
	case SON_FALSE: return son.write(test_case->key, false);
	case SON_NULL: return son.write(test_case->key, (const char*)0);
	}

	printf("Bad test case type\n");
	return -1;
}

int create_test_file(){
	Son<8> son;
	String str;
	int i;
	char data[DATA_TEST_SIZE];

	printf("Creating test file...");
	fflush(stdout);

	if( son.create(CREATE_TEST_FILE) < 0 ){
		printf("Failed to create test file\n");
		return -1;
	}

	if( son.open_obj("root") < 0 ){
		printf("Failed to create root object");
		return -1;
	}

	if( son.write("first", "john") < 0 ){
		printf("Failed to write 'first' key\n");
		return -1;
	}

	str = "jacob";
	if( son.write("middle", str) < 0 ){
		printf("Failed to write 'first' key\n");
		return -1;
	}

	if( son.write("last", "jingleheimer schmidt") < 0 ){
		printf("Failed to write 'first' key\n");
		return -1;
	}

	if( son.write("age", (u32)57) < 0 ){
		printf("Failed to write 'first' key\n");
		return -1;
	}

	if( son.write("married", true) < 0 ){
		printf("Failed to write 'first' key\n");
		return -1;
	}

	if( son.write("score", (s32)-10) < 0 ){
		printf("Failed to write 'first' key\n");
		return -1;
	}

	if( son.write("pi", (float)3.1415) < 0 ){
		printf("Failed to write 'first' key\n");
		return -1;
	}

	for(i=0; i < DATA_TEST_SIZE; i++){ data[i] = i; }
	if( son.write("data", data, DATA_TEST_SIZE) < 0 ){
		printf("Failed to write 'first' key\n");
		return -1;
	}

	if( son.close_obj() < 0 ){
		printf("Failed to close object\n");
		return -1;
	}

	if( son.close() < 0 ){
		printf("Failed to close SON file\n");
		return -1;
	}
	printf("passed\n");

	return 0;
}

int verify_test_file(){
	Son<8> son;
	String str;
	int i;
	u32 var_u32;
	s32 var_s32;
	float var_float;
	char cstr[24];
	char data[DATA_TEST_SIZE];

	printf("Verifying test file...");
	fflush(stdout);

	if( son.open_read(CREATE_TEST_FILE) < 0 ){
		printf("Failed to open test file\n");
		return -1;
	}

	if( son.read_str("first", str) < 0 ){
		printf("Failed to read first\n");
		return -1;
	}

	if( str != "john" ){
		printf("Failed to compare first %s != 'john'\n", str.c_str());
		return -1;
	}

	if( son.read_str("middle", cstr, 24) < 0 ){
		printf("Failed to read first\n");
		return -1;
	}

	if( strcmp(cstr, "jacob") != 0 ){
		printf("Failed to compare middle %s != 'jacob'\n", cstr);
		return -1;
	}

	if( son.read_str("last", str) < 0 ){
		printf("Failed to read first\n");
		return -1;
	}

	if( str != "jingleheimer schmidt" ){
		printf("Failed to compare first %s != 'jingleheimer schmidt'\n", str.c_str());
		return -1;
	}

	var_u32 = son.read_unum("age");
	if( var_u32 != 57 ){
		printf("Failed to read/compare age %ld != 57\n", var_u32);
	}

	var_s32 = son.read_num("score");
	if( var_s32 != -10 ){
		printf("Failed to read/compare age %ld != -10\n", var_s32);
	}

	var_float = son.read_float("pi");
	if( var_float != (float)3.1415 ){
		printf("Failed to read/compare age %f != 3.1415 \n", var_float);
	}

	if( son.read_bool("married") != true ){
		printf("Failed to read bool key\n");
		return -1;
	}

	if( son.read_data("data", data, DATA_TEST_SIZE) < 0 ){
		printf("Failed to read 'data' key\n");
		return -1;
	}

	for(i=0; i < DATA_TEST_SIZE; i++){
		if( data[i] != i ){
			printf("Failed to compare data at %d\n", i);
			return -1;
		}
	}

	if( son.close() < 0 ){
		printf("Failed to close SON file\n");
		return -1;
	}
	printf("passed\n");

	return 0;
}

int edit_test_file(){
	Son<8> son;
	String str;
	int i;
	char data[DATA_TEST_SIZE];

	printf("Edit test file...");
	fflush(stdout);

	if( son.open_edit(CREATE_TEST_FILE) < 0 ){
		printf("Failed to create test file\n");
		return -1;
	}

	if( son.edit("first", "JOHN") < 0 ){
		printf("Failed to edit 'first' key\n");
		return -1;
	}

	str = "JACOB";
	if( son.edit("middle", str) < 0 ){
		printf("Failed to edit 'middle' key\n");
		return -1;
	}

	if( son.edit("last", "JINGLEHEIMER SCHMIDT") < 0 ){
		printf("Failed to edit 'last' key\n");
		return -1;
	}

	if( son.edit("age", (u32)100) < 0 ){
		printf("Failed to edit 'age' key\n");
		return -1;
	}

	if( son.edit("married", false) < 0 ){
		printf("Failed to edit 'married' key\n");
		return -1;
	}

	if( son.edit("score", (s32)-100) < 0 ){
		printf("Failed to edit 'score' key\n");
		return -1;
	}

	if( son.edit("pi", (float)3.14159265) < 0 ){
		printf("Failed to edit 'pi' key\n");
		return -1;
	}

	for(i=0; i < DATA_TEST_SIZE; i++){ data[i] = DATA_TEST_SIZE - i; }
	if( son.edit("data", data, DATA_TEST_SIZE) < 0 ){
		printf("Failed to edit 'data' key\n");
		return -1;
	}

	if( son.close() < 0 ){
		printf("Failed to close SON file\n");
		return -1;
	}
	printf("passed\n");

	return 0;

}

int verify_edit_test_file(){
	Son<8> son;
	String str;
	int i;
	u32 var_u32;
	s32 var_s32;
	float var_float;
	char cstr[24];
	char data[DATA_TEST_SIZE];

	printf("Verifying edited test file...");
	fflush(stdout);

	if( son.open_read(CREATE_TEST_FILE) < 0 ){
		printf("Failed to open test file\n");
		return -1;
	}

	if( son.read_str("first", str) < 0 ){
		printf("Failed to read first\n");
		return -1;
	}

	if( str != "JOHN" ){
		printf("Failed to compare first %s != 'JOHN'\n", str.c_str());
		return -1;
	}

	if( son.read_str("middle", cstr, 24) < 0 ){
		printf("Failed to read first\n");
		return -1;
	}

	if( strcmp(cstr, "JACOB") != 0 ){
		printf("Failed to compare middle %s != 'JACOB'\n", cstr);
		return -1;
	}

	if( son.read_str("last", str) < 0 ){
		printf("Failed to read first\n");
		return -1;
	}

	if( str != "JINGLEHEIMER SCHMIDT" ){
		printf("Failed to compare first %s != 'JINGLEHEIMER SCHMIDT'\n", str.c_str());
		return -1;
	}

	var_u32 = son.read_unum("age");
	if( var_u32 != 100 ){
		printf("Failed to read/compare age %ld != 100\n", var_u32);
	}

	var_s32 = son.read_num("score");
	if( var_s32 != -100 ){
		printf("Failed to read/compare age %ld != -100\n", var_s32);
	}

	var_float = son.read_float("pi");
	if( var_float != (float)3.14159265 ){
		printf("Failed to read/compare age %f != 3.14159265 \n", var_float);
	}

	if( son.read_bool("married") != false ){
		printf("Failed to read bool key\n");
		return -1;
	}

	if( son.read_data("data", data, DATA_TEST_SIZE) < 0 ){
		printf("Failed to read 'data' key\n");
		return -1;
	}

	for(i=0; i < DATA_TEST_SIZE; i++){
		if( data[i] != DATA_TEST_SIZE - i ){
			printf("Failed to compare data at %d\n", i);
			return -1;
		}
	}

	if( son.close() < 0 ){
		printf("Failed to close SON file\n");
		return -1;
	}
	printf("passed\n");

	return 0;
}
