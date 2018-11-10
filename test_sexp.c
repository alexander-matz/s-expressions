#include "minunit.h"

#include "sexp.h"

#include <stdlib.h>

MU_TEST(test_string) {
  mu_check(!sexp_is_string(NULL));
  sexp_t *e = sexp_new_string("asdf");
  mu_check(sexp_is_string(e));
  mu_check(sexp_string_length(e) == 4);
  mu_check(strcmp(sexp_string_get(e), "asdf") == 0);
  sexp_free(e);
}

MU_TEST(test_string2) {
  mu_check(!sexp_is_string(NULL));
  sexp_t *e = sexp_new_string_len("asdffdsa", 4);
  mu_check(sexp_is_string(e));
  mu_check(sexp_string_length(e) == 4);
  mu_check(strcmp(sexp_string_get(e), "asdf") == 0);
  sexp_free(e);
}

MU_TEST(test_symbol) {
  mu_check(!sexp_is_symbol(NULL));
  sexp_t *e = sexp_new_symbol("sym1");
  mu_check(sexp_is_symbol(e));
  mu_check(sexp_symbol_length(e) == 4);
  mu_check(strcmp(sexp_symbol_get(e), "sym1") == 0);
  sexp_free(e);
}

MU_TEST(test_symbol2) {
  mu_check(!sexp_is_symbol(NULL));
  sexp_t *e = sexp_new_symbol_len("sym1fdsa", 4);
  mu_check(sexp_is_symbol(e));
  mu_check(sexp_symbol_length(e) == 4);
  mu_check(strcmp(sexp_symbol_get(e), "sym1") == 0);
  sexp_free(e);
}

MU_TEST(test_number) {
  mu_check(!sexp_is_number(NULL));
  sexp_t *e = sexp_new_number(1.5);
  mu_check(sexp_is_number(e));
  mu_check(sexp_number_get(e) == 1.5);
  sexp_free(e);
}

MU_TEST(test_list) {
  mu_check(!sexp_is_list(NULL));
  sexp_t *l = sexp_new_list();
  mu_check(sexp_is_list(l));
  mu_check(sexp_list_length(l) == 0);
  l = sexp_list_append(l, sexp_new_number(1));
  l = sexp_list_append(l, sexp_new_number(2));
  l = sexp_list_append(l, sexp_new_number(3));
  l = sexp_list_append(l, sexp_new_number(4));
  mu_check(sexp_list_length(l) == 4);
  mu_check(sexp_number_get(sexp_list_nth(l, 0)) == 1);
  mu_check(sexp_number_get(sexp_list_nth(l, 1)) == 2);
  mu_check(sexp_number_get(sexp_list_nth(l, 2)) == 3);
  mu_check(sexp_number_get(sexp_list_nth(l, 3)) == 4);
  // reallocates here
  l = sexp_list_append(l, sexp_new_number(5));
  l = sexp_list_append(l, sexp_new_number(6));
  l = sexp_list_append(l, sexp_new_number(7));
  mu_check(sexp_list_length(l) == 7);
  mu_check(sexp_number_get(sexp_list_nth(l, 0)) == 1);
  mu_check(sexp_number_get(sexp_list_nth(l, 1)) == 2);
  mu_check(sexp_number_get(sexp_list_nth(l, 2)) == 3);
  mu_check(sexp_number_get(sexp_list_nth(l, 3)) == 4);
  mu_check(sexp_number_get(sexp_list_nth(l, 4)) == 5);
  mu_check(sexp_number_get(sexp_list_nth(l, 5)) == 6);
  mu_check(sexp_number_get(sexp_list_nth(l, 6)) == 7);
  
  sexp_free(l);
}

MU_TEST_SUITE(test_sexp_types) {
  MU_RUN_TEST(test_string);
  MU_RUN_TEST(test_string2);
  MU_RUN_TEST(test_symbol);
  MU_RUN_TEST(test_symbol2);
  MU_RUN_TEST(test_number);
  MU_RUN_TEST(test_list);
}

MU_TEST(test_read_string) {
  sexp_t *e;

  const char* ref;
  char* end;

  ref = "\"asdf\"";
  e = sexp_read(ref, &end);
  mu_check(sexp_is_string(e));
  mu_check(strcmp(sexp_string_get(e), "asdf") == 0);
  mu_check(end - ref == 6);
  sexp_free(e);

  ref = "\"asd)f\"";
  e = sexp_read(ref, &end);
  mu_check(sexp_is_string(e));
  mu_check(strcmp(sexp_string_get(e), "asd)f") == 0);
  mu_check(end - ref == 7);
  sexp_free(e);

  ref = "   \"asdf\"";
  e = sexp_read(ref, &end);
  mu_check(sexp_is_string(e));
  mu_check(strcmp(sexp_string_get(e), "asdf") == 0);
  mu_check(end - ref == 9);
  sexp_free(e);

  ref = "   \"asdf";
  e = sexp_read(ref, &end);
  mu_check(e == NULL);
  mu_check(end - ref == 0);
  sexp_free(e);

  ref = "\"as \\n\\t\\fdf\"";
  e = sexp_read(ref, &end);
  mu_check(sexp_is_string(e));
  mu_check(strcmp(sexp_string_get(e), "as \n\t\fdf") == 0);
  mu_check(end - ref == 13);
  sexp_free(e);
}

MU_TEST(test_read_symbol) {
  sexp_t *e;

  e = sexp_read(" asdf", NULL);
  mu_check(sexp_is_symbol(e));
  mu_check(sexp_symbol_eq(e, "asdf"));
  sexp_free(e);

  e = sexp_read("  asd)", NULL);
  mu_check(sexp_is_symbol(e));
  mu_check(sexp_symbol_eq(e, "asd"));
  sexp_free(e);

  e = sexp_read("  asd(", NULL);
  mu_check(sexp_is_symbol(e));
  mu_check(sexp_symbol_eq(e, "asd"));
  sexp_free(e);
}

MU_TEST(test_read_number) {
  sexp_t *e;

  e = sexp_read("123", NULL);
  mu_check(sexp_is_number(e));
  mu_check(sexp_number_get(e) == 123);
  sexp_free(e);

  e = sexp_read("-1.e2", NULL);
  mu_check(sexp_is_number(e));
  mu_check(sexp_number_get(e) == -1.e2);
  sexp_free(e);

  e = sexp_read("asdf", NULL);
  mu_check(!sexp_is_number(e));
  sexp_free(e);

  e = sexp_read("-bla", NULL);
  mu_check(!sexp_is_number(e));
  sexp_free(e);
}

MU_TEST(test_read_list) {
  sexp_t *e;

  e = sexp_read("()", NULL);
  mu_check(sexp_is_list(e));
  mu_check(sexp_list_length(e) == 0);
  sexp_free(e);

  e = sexp_read("(123)", NULL);
  mu_check(sexp_is_list(e));
  mu_check(sexp_list_length(e) == 1);
  mu_check(sexp_is_number(sexp_list_nth(e, 0)));
  sexp_free(e);

  e = sexp_read("( 123 )", NULL);
  mu_check(sexp_is_list(e));
  mu_check(sexp_list_length(e) == 1);
  mu_check(sexp_is_number(sexp_list_nth(e, 0)));
  sexp_free(e);

  e = sexp_read("( 123", NULL);
  mu_check(e == NULL);

  e = sexp_read("( 123 asdf \"asdf fdsa\" (321))", NULL);
  mu_check(sexp_is_list(e));
  mu_check(sexp_list_length(e) == 4);
  mu_check(sexp_is_number(sexp_list_nth(e, 0)));
  mu_check(sexp_is_symbol(sexp_list_nth(e, 1)));
  mu_check(sexp_is_string(sexp_list_nth(e, 2)));

  mu_check(sexp_is_list(sexp_list_nth(e, 3)));
  mu_check(sexp_list_length(sexp_list_nth(e, 3)));
  mu_check(sexp_is_number(sexp_list_nth(sexp_list_nth(e, 3), 0)));
  sexp_free(e);
}

MU_TEST(test_read_comment) {
  sexp_t *e;

  e = sexp_read("(1 ;asdf 2\n3)", NULL);
  mu_check(sexp_is_list(e));
  mu_check(sexp_list_length(e) == 2);
  mu_check(sexp_is_number(sexp_list_nth(e, 0)));
  mu_check(sexp_number_get(sexp_list_nth(e, 0)) == 1);
  mu_check(sexp_is_number(sexp_list_nth(e, 1)));
  mu_check(sexp_number_get(sexp_list_nth(e, 1)) == 3);
  sexp_free(e);
}

MU_TEST_SUITE(test_sexp_read) {
  MU_RUN_TEST(test_read_string);
  MU_RUN_TEST(test_read_symbol);
  MU_RUN_TEST(test_read_number);
  MU_RUN_TEST(test_read_list);
  MU_RUN_TEST(test_read_comment);
}



MU_TEST(test_sexp_print_number) {
  sexp_t *e = sexp_new_number(1.5);
  char* buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "1.5") == 0);
  free(buf);
  sexp_free(e);

  e = sexp_new_number(-1);
  buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "-1") == 0);
  free(buf);
  sexp_free(e);
}

MU_TEST(test_sexp_print_symbol) {
  sexp_t *e = sexp_new_symbol("a.symbol");
  char* buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "a.symbol") == 0);
  free(buf);
  sexp_free(e);
}

MU_TEST(test_sexp_print_string) {
  sexp_t *e;
  char* buf;

  e = sexp_new_string("a string");
  buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "\"a string\"") == 0);
  free(buf);
  sexp_free(e);

  e = sexp_new_string_len("a\tstring\nwith\0escapes\"", 22);
  buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "\"a\\tstring\\nwith\\0escapes\\\"\"") == 0);
  free(buf);
  sexp_free(e);
}

MU_TEST(test_sexp_print_list) {
  sexp_t *e;
  char* buf;

  e = sexp_new_list();
  buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "()") == 0);
  free(buf);
  sexp_free(e);

  e = sexp_new_list();
  e = sexp_list_append(e, sexp_new_number(1.5));
  buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "(1.5)") == 0);
  free(buf);
  sexp_free(e);

  e = sexp_new_list();
  e = sexp_list_append(e, sexp_new_number(1.5));
  e = sexp_list_append(e, sexp_new_symbol("a"));
  buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "(1.5 a)") == 0);
  free(buf);
  sexp_free(e);

  e = sexp_new_list();
  e = sexp_list_append(e, sexp_new_number(1.5));
  e = sexp_list_append(e, sexp_new_symbol("a"));

  sexp_t *aux = sexp_new_list();
  aux = sexp_list_append(aux, sexp_new_string("a string"));
  aux = sexp_list_append(aux, sexp_new_number(3));
  e = sexp_list_append(e, aux);

  buf = sexp_display(e);
  mu_check(buf != NULL);
  mu_check(strcmp(buf, "(1.5 a (\"a string\" 3))") == 0);
  free(buf);
  sexp_free(e);
}

MU_TEST_SUITE(test_sexp_print) {
  MU_RUN_TEST(test_sexp_print_number);
  MU_RUN_TEST(test_sexp_print_symbol);
  MU_RUN_TEST(test_sexp_print_string);
  MU_RUN_TEST(test_sexp_print_list);
}

int main(int argc, char** argv) {
  MU_RUN_SUITE(test_sexp_types);
  MU_RUN_SUITE(test_sexp_read);
  MU_RUN_SUITE(test_sexp_print);
  MU_REPORT();
  return minunit_status;
}
