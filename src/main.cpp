#include <cstdio>
#include <cstring>
#include <sapi/sys.hpp>
#include <sapi/fmt.hpp>
#include <sapi/var.hpp>
#include <sapi/hal.hpp>

#include "tests.h"

#define DATA_TEST_SIZE 64
#define CREATE_TEST_FILE "/home/create_test.son"
#define MAX_DEPTH 16

static int create_primary_values(const char * file_name, void * message = 0, int message_size = 0);
static int edit_primary_values(const char * file_name, void * message = 0, int message_size = 0);
static int verify_primary_values(const char * file_name, void * message = 0, int message_size = 0);
static int verify_secondary_values(const char * file_name, void * message = 0, int message_size = 0);
static int send_primary_values(const char * file_name, void * message, int message_size);
static int show_keys(const char * file_name);
static int to_json_callback(void * context, const char * entry);

void * receive_primary_values(void * args);

static int append_primary_values(const char * file_name);

static int write_test_value(Son<MAX_DEPTH> & son, test_case_t * test_case, const test_value_t & value);
static int edit_test_value(Son<MAX_DEPTH> & son, test_case_t * test_case, test_value_t & value);
static int verify_test_value(Son<MAX_DEPTH> & son, const test_case_t * test_case, test_value_t & value);

int main(int argc, char * argv[]){
	Cli cli(argc, argv);
	cli.set_publisher("Stratify Labs, Inc");
	cli.handle_version();
	Data message(2048);
	message.clear();

	printf("Starting SON Test\n");

	int i;
	int tab_counter;
	int tab_count = 0;
	test_case_t * test_case;
	printf("{\n");
	tab_count=1;
	test_init(10, Timer::get_clock_usec());

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

	printf("Delete file\n");
	unlink("/home/test.son");

	printf("Create values\n");

	if( create_primary_values("/home/test.son") < 0 ){
		printf("create primary value file failed\n");
		exit(1);
	}

	show_keys("/home/test.son");

	printf("Verify values\n");
	if( verify_primary_values("/home/test.son") < 0 ){
		printf("Failed to verify primary test values\n");
		exit(1);
	}

	printf("Edit primary values\n");
	if( edit_primary_values("/home/test.son") < 0 ){
		printf("Failed to edit primary test values\n");
		exit(1);
	}

	printf("Edit secondary values\n");
	if( verify_secondary_values("/home/test.son") < 0 ){
		printf("Failed to verify secondary test values\n");
		exit(1);
	}

	if( message.data() != 0 ){

		printf("Test messages\n");

		if( create_primary_values(0, message.data(), message.capacity()) < 0 ){
			printf("create primary value message failed\n");
			exit(1);
		}

		if( send_primary_values(0, message.data(), message.capacity()) < 0 ){
			printf("send primary value message failed\n");
			exit(1);
		}

		if( verify_primary_values(0, message.data(), message.capacity()) < 0 ){
			printf("Failed to verify primary message test values\n");
			exit(1);
		}

		if( edit_primary_values(0, message.data(), message.capacity()) < 0 ){
			printf("Failed to edit primary message test values\n");
			exit(1);
		}

		if( verify_secondary_values(0, message.data(), message.capacity()) < 0 ){
			printf("Failed to verify secondary message test values\n");
			exit(1);
		}

	}

	unlink("/home/test.son");

	if( append_primary_values("/home/test.son") < 0 ){
		printf("create primary value file failed\n");
		exit(1);
	}

	if( verify_primary_values("/home/test.son") < 0 ){
		printf("Failed to verify primary test values\n");
		exit(1);
	}

	printf("Test complete\n");

	return 0;
}

int show_keys_recursive(Son<MAX_DEPTH> & son, const String & current_access, bool is_array, int full_size){
	String access;
	String key;
	int ret;
	int array_index;
	son_value_t type;

	s32 bytes_remaining;
	son_size_t data_size;

	if( son.seek(current_access, data_size) <  0 ){
		return 0;
	}

	array_index = 0;
	bytes_remaining = data_size;
	while( (bytes_remaining > 0) && ((ret = son.seek_next(key, &type)) > 0) ){

		if( is_array ){
			if( current_access != "" ){
				printf("%s[%d]->%s",
						current_access.c_str(),
						array_index,
						Son<MAX_DEPTH>::get_type_description(type));
				access.sprintf("%s[%d]", current_access.c_str(), array_index);
			} else {
				printf("[%d]->%s", array_index, Son<MAX_DEPTH>::get_type_description(type));
				access.sprintf("[%d]", array_index);
			}
			array_index++;
		} else {
			if( current_access != "" ){
				printf("%s.%s->%s", current_access.c_str(), key.c_str(), Son<MAX_DEPTH>::get_type_description(type));
				access.sprintf("%s.%s", current_access.c_str(), key.c_str());
			} else {
				printf("%s->%s", key.c_str(), Son<MAX_DEPTH>::get_type_description(type));
				access.sprintf("%s", key.c_str());
			}
		}

		printf("\n");

		if( type == SON_OBJECT ){
			ret = show_keys_recursive(son, access, false, ret);
		} else if( type == SON_ARRAY ) {
			ret = show_keys_recursive(son, access, true, ret);
		}

		bytes_remaining -= ret;

	}

	return full_size;
}

int show_keys(const char * file_name){
	Son<MAX_DEPTH> son;
	String access;
	son_size_t size;

	if( son.open_read(file_name) < 0 ){
		printf("Failed to open %s\n", file_name);
	} else {
		son.seek("", size);
		show_keys_recursive(son, access, false, size);
		son.close();
	}

	return 0;
}

int create_primary_values(const char * file_name, void * message, int message_size){
	int i;
	test_case_t * test_case;
	Son<MAX_DEPTH> son;

	if( file_name == 0 ){
		if( son.create_message(message, message_size) < 0 ){
			printf("Failed to open message (%d)\n", son.err());
			return -1;
		}
	} else {
		if( son.create(file_name) < 0 ){
			printf("Failed to open file %s (%d)\n", file_name, son.err());
			perror("Errno Message");
			return -1;
		}
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

	if( son.close() < 0 ){
		printf("Failed to close file (%d-%d)\n", son.err(), son.fileno());
		perror("Error message");
		return -1;
	}

	return 0;
}

int append_primary_values(const char * file_name){
	int i;
	test_case_t * test_case;
	Son<MAX_DEPTH> son;

	//first create a file with no values
	if( son.create(file_name) < 0 ){
		printf("Failed to open file %s (%d)\n", file_name, son.err());
		perror("Errno Message");
		return -1;
	}

	if( son.open_obj("") < 0 ){
		printf("Failed to create root object (%d)\n", son.err());
		return -1;
	}

	if( son.close() < 0 ){
		printf("Failed to close file (%d-%d)\n", son.err(), son.fileno());
		perror("Error message");
		return -1;
	}


	//now append all values to the file
	if( son.open_append(file_name) < 0 ){
		printf("Failed to open file %s for appending (%d)\n", file_name, son.err());
		perror("Errno Message");
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

	if( son.close() < 0 ){
		printf("Failed to close appended file (%d-%d)\n", son.err(), son.fileno());
		perror("Error message");
		return -1;
	}

	return 0;

}

int edit_primary_values(const char * file_name, void * message, int message_size){
	int i;
	test_case_t * test_case;
	Son<MAX_DEPTH> son;

	if( file_name == 0 ){
		if( son.open_edit_message(message, message_size) < 0 ){
			printf("Failed to open message (%d)\n", son.err());
			return -1;
		}
	} else {
		if( son.open_edit(file_name) < 0 ){
			printf("Failed to open file %s (%d)\n", file_name, son.get_error());
			return -1;
		}
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

	if( son.close() < 0 ){
		printf("Failed to close file (%d)\n", son.err());
		perror("Error message");
		return -1;
	}

	return 0;
}

void * receive_primary_values(void * args){
	Data message(2048);
	int i;
	int ret;
	test_case_t * test_case;

	Son<MAX_DEPTH> son;

	Dev fifo;

	if( fifo.open("/dev/multistream", Dev::READONLY | Dev::NONBLOCK) < 0 ){
		//can send messages on this platform
		return 0;
	}

	fifo.seek(0);

	son.open_read_message(message.data(), message.capacity());

	if( (ret = son.recv_message(fifo.fileno(), 1000)) < 0 ){
		printf("Failed to receive the message\n");
		return (void*)-1;
	}

	printf("RX'd:%d\n", ret);

	for(i=0; i < test_get_count(); i++){
		test_case = test_get_case(i);
		printf("RX Verify Primary:");
		test_show_case(test_case);
		printf("\n");
		if( verify_test_value(son, test_case, test_case->primary_value) < 0 ){
			printf("Failed to verify test value (%d)\n", son.err());
			return (void*)-1;
		}
	}

	return 0;

}

int send_primary_values(const char * file_name, void * message, int message_size){
	Son<MAX_DEPTH> son;
	void * value;
	int i;
	int ret;
	test_case_t * test_case;
	Thread listener(2048, false);

	CFifo fifo;

	if( fifo.open("/dev/multistream", Dev::WRITEONLY) < 0 ){
		//can send messages on this platform
		return 0;
	}

	fifo_attr_t fifo_attr;

	fifo_attr.o_flags = Fifo::FLAG_SET_WRITEBLOCK;

	fifo.set_attr(0, fifo_attr);
	fifo.seek(0);
	fifo.flush(0);


	son.open_read_message(message, message_size);

	listener.create(receive_primary_values);

	if( (ret = son.send_message(fifo.fileno(), 500)) != son.get_message_size() ){
		printf("Failed to send message %d\n", son.get_error());
	}

	listener.join(&value);

	for(i=0; i < test_get_count(); i++){
		test_case = test_get_case(i);
		printf("TX Verify Primary:");
		test_show_case(test_case);
		printf("\n");
		if( verify_test_value(son, test_case, test_case->primary_value) < 0 ){
			printf("Failed to verify test value (%d)\n", son.err());
			return -1;
		}
	}

	ret = (s32)value;
	if( ret != 0 ){
		printf("Failed\n");
		return -1;
	}


	return 0;
}

int verify_primary_values(const char * file_name, void * message, int message_size){
	Son<MAX_DEPTH> son;
	test_case_t * test_case;
	int i;

	errno = 0;
	if( file_name == 0 ){
		if( son.open_read_message(message, message_size) < 0 ){
			printf("Failed to open message (%d)\n", son.err());
			return -1;
		}
	} else {
		if( son.open_read(file_name) < 0 ){
			printf("Failed to open file %s (%d)\n", file_name, son.err());
			perror("Errno Message");
			return -1;
		}

		son.to_json(to_json_callback);
		son.close();

		son.open_read(file_name);

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

int verify_secondary_values(const char * file_name, void * message, int message_size){
	Son<MAX_DEPTH> son;
	test_case_t * test_case;
	int i;

	if( file_name == 0 ){
		if( son.open_read_message(message, message_size) < 0 ){
			printf("Failed to open message for reading\n");
			return -1;
		}
	} else {
		if( son.open_read(file_name) < 0 ){
			printf("Failed to open file %s (%d)\n", file_name, son.err());
			perror("Errno Message");
			return -1;
		}
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

	if( son.close() < 0 ){
		printf("Failed to close file (%d)\n", son.err());
		perror("Error message");
		return -1;
	}

	return 0;
}

int verify_test_value(Son<MAX_DEPTH> & son, const test_case_t * test_case, test_value_t & value){
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

int to_json_callback(void * context, const char * entry){
	printf("%s", entry);
	return 0;
}

