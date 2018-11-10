/******************************************************************************
 * Copyright 2018 by Alexander Matz
 *
 * This work is licensed under
 * Creative Commons Attribution 4.0 International (CC BY 4.0)
 *
 * For more info, visit:  https://creativecommons.org/licenses/by/4.0/
 *****************************************************************************/
#ifndef __RUB_SEXP_
#define __RUB_SEXP_

#include <stddef.h>

typedef struct sexp_t sexp_t;

void sexp_free(sexp_t *e);

sexp_t *sexp_new_string(const char* s);
sexp_t *sexp_new_string_len(const char* s, size_t len);
void sexp_string_free(sexp_t *e);
int sexp_is_string(const sexp_t *e);
size_t sexp_string_length(const sexp_t *e);
const char* sexp_string_get(const sexp_t* e);



sexp_t *sexp_new_symbol(const char* s);
sexp_t *sexp_new_symbol_len(const char* s, size_t len);
void sexp_symbol_free(sexp_t *e);
int sexp_is_symbol(const sexp_t *e);
int sexp_symbol_eq(const sexp_t *e, const char* ref);
size_t sexp_symbol_length(const sexp_t *e);
const char* sexp_symbol_get(const sexp_t* e);



sexp_t *sexp_new_number(double num);
void sexp_number_free(sexp_t *e);
int sexp_is_number(const sexp_t *e);
double sexp_number_get(const sexp_t *e);



sexp_t *sexp_new_list();
void sexp_list_free(sexp_t *e);
int sexp_is_list(const sexp_t *e);
size_t sexp_list_length(const sexp_t *e);
sexp_t *sexp_list_nth(const sexp_t *e, int n);
sexp_t *sexp_list_append(sexp_t *list, sexp_t *val); // consumes list and returns new



sexp_t *sexp_read(const char* src, char** end);

char *sexp_display(sexp_t *e);

#endif
