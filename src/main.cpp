#include <cstdio>
#include <stfy/fmt.hpp>
#include <stfy/var.hpp>

#define DATA_TEST_SIZE 64
#define CREATE_TEST_FILE "/home/create_test.son"

static int create_test_file();
static int verify_test_file();
static int edit_test_file();
static int verify_edit_test_file();

int main(int argc, char * argv[]){
	printf("Starting SON Test\n");

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

	if( son.open(CREATE_TEST_FILE) < 0 ){
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

	if( son.edit(CREATE_TEST_FILE) < 0 ){
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

	if( son.open(CREATE_TEST_FILE) < 0 ){
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
