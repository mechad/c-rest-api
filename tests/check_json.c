#include "../src/utils/json.h"
#include <check.h>

START_TEST(simple_json_parse_t)
{
    char json[] = "{\"name\": \"sample\"}";
    JSONString* jstring = copy_chars(json, 19);
    bool succss = false;
    JSONObject* obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString* sample = json_get_string_c(obj, "name");
    ck_assert_str_eq(sample->chars, "sample");
    free_json(obj);
    STRINGP_FREE(sample);
    STRINGP_FREE(jstring);
}
END_TEST

START_TEST(multiple_pair_json_parse_t)
{
    char json[] = "{\"name\": \"sample\", \"number\": \"12345\", \"address\": \"Test Addr 1234\"}";
    JSONString* jstring = copy_chars(json, 67);
    bool succss = false;
    JSONObject* obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString* sample = json_get_string_c(obj, "name");
    ck_assert_str_eq(sample->chars, "sample");
    JSONString* number = json_get_string_c(obj, "number");
    ck_assert_str_eq(number->chars, "12345");
    JSONString* address = json_get_string_c(obj, "address");
    ck_assert_str_eq(address->chars, "Test Addr 1234");
    free_json(obj);
    STRINGP_FREE(sample);
    STRINGP_FREE(number);
    STRINGP_FREE(address);
    STRINGP_FREE(jstring);
}
END_TEST

START_TEST(object_json_parse_t)
{
    char json[] = "{\"name\": \"sample\", \"number\": {\"real\": \"1234567\"}}";
    JSONString* jstring = copy_chars(json, 67);
    bool succss = false;
    JSONObject* obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString* sample = json_get_string_c(obj, "name");
    ck_assert_str_eq(sample->chars, "sample");
    JSONObject* number = json_get_object_c(obj, "number");
    JSONString* real = json_get_string_c(number, "real");
    ck_assert_str_eq(real->chars, "1234567");
    free_json(obj);
    free_json(number);
    STRINGP_FREE(sample);
    STRINGP_FREE(real);
    STRINGP_FREE(jstring);
}
END_TEST

START_TEST(multiple_object_json_parse_t)
{
    char json[] = "{\"name\": \"sample\", \"number\": {\"real\": \"1234567\"}, \"addr\": {\"actual\": \"Test Addr 1234\"}}";
    JSONString* jstring = copy_chars(json, 88);
    bool succss = false;
    JSONObject* obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString* sample = json_get_string_c(obj, "name");
    ck_assert_str_eq(sample->chars, "sample");
    JSONObject* number = json_get_object_c(obj, "number");
    JSONString* real = json_get_string_c(number, "real");
    ck_assert_str_eq(real->chars, "1234567");
    JSONObject* addr = json_get_object_c(obj, "addr");
    JSONString* actual = json_get_string_c(addr, "actual");
    ck_assert_str_eq(actual->chars, "Test Addr 1234");
    free_json(obj);
    free_json(number);
    free_json(addr);
    STRINGP_FREE(sample);
    STRINGP_FREE(real);
    STRINGP_FREE(actual);
    STRINGP_FREE(jstring);
}
END_TEST

START_TEST(nested_object_json_parse_t)
{
    char json[] = "{\"name\": \"sample\", \"number\": {\"real\": \"1234567\", \"addr\": {\"actual\": \"Test Addr 1234\"}}}";
    JSONString* jstring = copy_chars(json, 88);
    bool succss = false;
    JSONObject* obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString* sample = json_get_string_c(obj, "name");
    ck_assert_str_eq(sample->chars, "sample");
    JSONObject* number = json_get_object_c(obj, "number");
    JSONString* real = json_get_string_c(number, "real");
    ck_assert_str_eq(real->chars, "1234567");
    JSONObject* addr = json_get_object_c(number, "addr");
    JSONString* actual = json_get_string_c(addr, "actual");
    ck_assert_str_eq(actual->chars, "Test Addr 1234");
    free_json(obj);
    free_json(number);
    free_json(addr);
    STRINGP_FREE(sample);
    STRINGP_FREE(real);
    STRINGP_FREE(actual);
    STRINGP_FREE(jstring);
}
END_TEST

START_TEST(simple_json_parse_fail_t)
{
    char json[] = "{\"name\", \"sample\"}";
    JSONString* jstring = copy_chars(json, 19);
    bool succss = false;
    JSONObject* obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, false);
    free_json(obj);
}
END_TEST

START_TEST(simple_json_bool_and_number_t)
{
    char json[] = "{\"false\": false, \"true\": true, \"number\": 1234}";
    JSONString* jstring = copy_chars(json, 47);
    bool succss = true;
    JSONObject* obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONBool* fal = json_get_bool_c(obj, "false");
    ck_assert_int_eq(*fal, false);
    JSONBool* tr = json_get_bool_c(obj, "true");
    ck_assert_int_eq(*tr, true);
    JSONNumber* nr = json_get_number_c(obj, "number");
    ck_assert_int_eq((int)(*nr), 1234);
    free_json(obj);
}
END_TEST

Suite* json_suite()
{
    Suite* s;
    TCase* tc_core;

    s = suite_create("JSON");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, simple_json_parse_t);
    tcase_add_test(tc_core, multiple_pair_json_parse_t);
    tcase_add_test(tc_core, object_json_parse_t);
    tcase_add_test(tc_core, multiple_object_json_parse_t);
    tcase_add_test(tc_core, nested_object_json_parse_t);
    tcase_add_test(tc_core, simple_json_parse_fail_t);
    tcase_add_test(tc_core, simple_json_bool_and_number_t);
    suite_add_tcase(s, tc_core);

    return s;
}

int main()
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = json_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
