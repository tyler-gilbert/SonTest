/*
 * tests.c
 *
 *  Created on: May 3, 2017
 *      Author: tgil
 */


#include "tests.h"

#define TEST_CASE_SIZE 28

test_case_t test_cases[TEST_CASE_SIZE] = {
		{ .key="str0", .access="str0", .type=SON_STRING, .primary_value.str="benjamin", .secondary_value.str="BENJAMIN" },
		{ .key="str1", .access="str1", .type=SON_STRING, .primary_value.str="franklin", .secondary_value.str="FRANKLIN" },
		{ .key="str2", .access="str2", .type=SON_STRING, .primary_value.str="james", .secondary_value.str="JAMES" },
		{ .key="str3", .access="str3", .type=SON_STRING, .primary_value.str="pennsylvania", .secondary_value.str="PENNSYLVANIA" },
		{ .key="str4", .access="str4", .type=SON_STRING, .primary_value.str="90000", .secondary_value.str="84000" },
		{ .key="float", .access="float", .type=SON_FLOAT, .primary_value.fnum=3.1415, .secondary_value.fnum=2.7198 },
		{ .key="num", .access="num.num0", .type=TEST_CASE_OPEN_OBJ, .primary_value.num=100, .secondary_value.num=-100 },
		{ .key="num0", .access="num.num0", .type=SON_NUMBER_S32, .primary_value.num=100, .secondary_value.num=-100 },
		{ .key="num1", .access="num.num1", .type=SON_NUMBER_S32, .primary_value.num=200, .secondary_value.num=1984773 },
		{ .key="num2", .access="num.num2", .type=SON_NUMBER_S32, .primary_value.num=50971, .secondary_value.num=108751 },
		{ .key="num3", .access="num.num3", .type=SON_NUMBER_S32, .primary_value.num=13957101, .secondary_value.num=94736389 },
		{ .key="num4", .access="num.num4", .type=SON_NUMBER_S32, .primary_value.num=39375129, .secondary_value.num=8877272 },
		{ .key="num5", .access="num.num5", .type=SON_NUMBER_S32, .primary_value.num=-1938572, .secondary_value.num=83837675 },
		{ .key="num6", .access="num.num6", .type=SON_NUMBER_S32, .primary_value.num=-1356876543, .secondary_value.num=38387573 },
		{ .key="num7", .access="num.num7", .type=SON_NUMBER_S32, .primary_value.num=1837365738, .secondary_value.num=757262637 },
		{ .key="num8", .access="num.num8", .type=SON_NUMBER_S32, .primary_value.num=62748450, .secondary_value.num=83736572 },
		{ .key="num9", .access="num.num9", .type=SON_NUMBER_S32, .primary_value.num=484756629, .secondary_value.num=-3837568 },
		{ .key="num", .access="num.num9", .type=TEST_CASE_CLOSE_OBJ, .primary_value.num=484756629, .secondary_value.num=-3837568 },
		{ .key="unum0", .access="unum0", .type=SON_NUMBER_U32, .primary_value.unum=100, .secondary_value.unum=10 },
		{ .key="unum1", .access="unum1", .type=SON_NUMBER_U32, .primary_value.unum=200, .secondary_value.unum=20 },
		{ .key="unum2", .access="unum2", .type=SON_NUMBER_U32, .primary_value.unum=300, .secondary_value.unum=30 },
		{ .key="unum3", .access="unum3", .type=SON_NUMBER_U32, .primary_value.unum=400, .secondary_value.unum=40 },
		{ .key="unum4", .access="unum4", .type=SON_NUMBER_U32, .primary_value.unum=500, .secondary_value.unum=50 },
		{ .key="unum5", .access="unum5", .type=SON_NUMBER_U32, .primary_value.unum=600, .secondary_value.unum=60 },
		{ .key="unum6", .access="unum6", .type=SON_NUMBER_U32, .primary_value.unum=700, .secondary_value.unum=70 },
		{ .key="unum7", .access="unum7", .type=SON_NUMBER_U32, .primary_value.unum=800, .secondary_value.unum=80 },
		{ .key="unum8", .access="unum8", .type=SON_NUMBER_U32, .primary_value.unum=900, .secondary_value.unum=90 },
		{ .key="unum9", .access="unum9", .type=SON_NUMBER_U32, .primary_value.unum=1000, .secondary_value.unum=20 },
		{ .key="bool0", .access="bool0", .type=SON_TRUE, .primary_value.unum=0, .secondary_value.unum=0 },
		{ .key="bool1", .access="bool1", .type=SON_FALSE, .primary_value.unum=0, .secondary_value.unum=0 },
};


test_case_t * test_case_get(int i){

	if( i < test_case_count() ){
		return &(test_cases[i]);
	}

	return 0;

}

int test_case_count(){
	return TEST_CASE_SIZE;
}
