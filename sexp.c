#include "sexp.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define die(...) do{fprintf(stderr,__VA_ARGS__);abort();}while(0)

typedef enum sexp_type_t {
  SEXP_NONE,
  SEXP_STRING,
  SEXP_SYMBOL,
  SEXP_NUMBER,
  SEXP_LIST,
} sexp_type_t;

typedef struct sexp_t {
  sexp_type_t type;
  char _[];
} sexp_t;

void sexp_free(sexp_t *e) {
  if (e == NULL) return;
  switch (e->type) {
    case SEXP_STRING: sexp_string_free(e); return;
    case SEXP_SYMBOL: sexp_symbol_free(e); return;
    case SEXP_NUMBER: sexp_number_free(e); return;
    case SEXP_LIST: sexp_list_free(e); return;
    default: die("invalid S-Expression");
  }
}

/******************************************************************************
 * STRING
 *****************************************************************************/

typedef struct sexp_string_t {
  sexp_type_t type;
  size_t len;
  char val[];
} sexp_string_t;

sexp_t *sexp_new_string(const char* s) {
  return sexp_new_string_len(s, strlen(s));
}

sexp_t *sexp_new_string_len(const char* s, size_t len) {
  if (s == NULL) return NULL;
  sexp_string_t *e = malloc(sizeof(sexp_string_t) + len + 1);
  if (!e) die("out of memory");
  e->type = SEXP_STRING;
  e->len = len;
  memcpy(e->val, s, len);
  e->val[len] = '\0';
  return (sexp_t*)e;
}

void sexp_string_free(sexp_t *e) {
  free(e);
}

int sexp_is_string(const sexp_t *e) {
  return e != NULL && e->type == SEXP_STRING;
}

const char* sexp_string_get(const sexp_t* e) {
  return ((sexp_string_t*)e)->val;
}

char* sexp_string_get_mut(const sexp_t* e) {
  return ((sexp_string_t*)e)->val;
}

size_t sexp_string_length(const sexp_t* e) {
  return ((sexp_string_t*)e)->len;
}

/******************************************************************************
 * SYMBOL
 *****************************************************************************/

typedef struct sexp_symbol_t {
  sexp_type_t type;
  size_t len;
  char val[];
} sexp_symbol_t;

sexp_t *sexp_new_symbol(const char* s) {
  return sexp_new_symbol_len(s, strlen(s));
}

sexp_t *sexp_new_symbol_len(const char* s, size_t len) {
  if (s == NULL) return NULL;
  sexp_string_t *e = malloc(sizeof(sexp_symbol_t) + len + 1);
  if (!e) die("out of memory");
  e->type = SEXP_SYMBOL;
  e->len = len;
  memcpy(e->val, s, len);
  e->val[len] = '\0';
  return (sexp_t*)e;
}

void sexp_symbol_free(sexp_t *e) {
  free(e);
}

int sexp_is_symbol(const sexp_t *e) {
  return e != NULL && e->type == SEXP_SYMBOL;
}

int sexp_symbol_eq(const sexp_t *e, const char* ref) {
  return strcmp(((sexp_symbol_t*)e)->val, ref) == 0;
}

const char* sexp_symbol_get(const sexp_t* e) {
  return ((sexp_symbol_t*)e)->val;
}

size_t sexp_symbol_length(const sexp_t* e) {
  return ((sexp_symbol_t*)e)->len;
}

/******************************************************************************
 * NUMBER
 *****************************************************************************/

typedef struct sexp_num_t {
  sexp_type_t type;
  double val;
} sexp_num_t;

sexp_t *sexp_new_number(double num) {
  sexp_num_t *e = malloc(sizeof(sexp_num_t));
  if (!e) die("out of memory");
  e->type = SEXP_NUMBER;
  e->val = num;
  return (sexp_t*)e;
}

void sexp_number_free(sexp_t *e) {
  free(e);
}

int sexp_is_number(const sexp_t *e) {
  return e != NULL && e->type == SEXP_NUMBER;
}

double sexp_number_get(const sexp_t *e) {
  return ((sexp_num_t*)e)->val;
}


/******************************************************************************
 * LIST
 *****************************************************************************/

typedef struct sexp_list_t {
  sexp_type_t type;
  size_t len;
  size_t cap;
  sexp_t *elements[];
} sexp_list_t;

static sexp_list_t *sexp_list_ensure_size(sexp_list_t *list, size_t capacity) {
  if (list->cap < capacity) {
    size_t newcap = list->cap < 2 ? 2 : list->cap;
    while (newcap < capacity) {
      newcap *= 1.5;
    }
    list = realloc(list, sizeof(sexp_list_t) + sizeof(sexp_t*) * newcap);
    if (!list) die("out of memory");
    list->cap = newcap;
  }
  return list;
}

sexp_t *sexp_new_list() {
  sexp_list_t *e = malloc(sizeof(sexp_list_t));
  if (!e) die("out of memory");
  e->type = SEXP_LIST;
  e->len = 0;
  e->cap = 0;
  return (sexp_t*)e;
}

void sexp_list_free(sexp_t *e) {
  sexp_list_t *list = (sexp_list_t*)e;
  for (int i = 0; i < list->len; ++i) {
    sexp_free(list->elements[i]);
  }
  free(e);
}

int sexp_is_list(const sexp_t *e) {
  return e != NULL && e->type == SEXP_LIST;
}

size_t sexp_list_length(const sexp_t *e) {
  return ((sexp_list_t*)e)->len;
}

sexp_t *sexp_list_nth(const sexp_t *e, int n) {
  sexp_list_t *list = (sexp_list_t*)e;
  assert(n >= 0 && n < list->len);
  return list->elements[n];
}

sexp_t *sexp_list_append(sexp_t *e, sexp_t *val) {
  sexp_list_t *list = (sexp_list_t*)e;
  size_t len = list->len;
  list = sexp_list_ensure_size(list, len + 1);
  list->elements[len] = val;
  list->len = len + 1;
  return (sexp_t*)list;
}

/******************************************************************************
 * PARSER
 *****************************************************************************/

static inline int isws(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\f' || ch == '\n';
}

int unescaped_length(const char* src, int len) {
  if (len < 0) len = strlen(src);
  size_t res = 0;
  int p = 0;
  while (p < len) {
    if (src[p] == '\\') ++p;
    ++p;
    ++res;
  }
  return res;
}

void unescape(const char* src, char* dst, int len) {
  if (len < 0) len = strlen(src);
  int p = 0;
  while (p < len) {
    if (src[p] == '\\') {
      ++p;
      switch (src[p]) {
        case '0': *dst = '\0'; break;
        case 'a': *dst = '\a'; break;
        case 'b': *dst = '\b'; break;
        case 'f': *dst = '\f'; break;
        case 'n': *dst = '\n'; break;
        case 'r': *dst = '\r'; break;
        case 't': *dst = '\t'; break;
        case 'v': *dst = '\v'; break;
        case '?': *dst = '\?'; break;
        default: *dst = src[p];
      }
    } else {
      *dst = src[p];
    }
    dst++;
    p++;
  }
}

typedef enum token_type {
  TT_ERR,
  TT_EOF,
  TT_OPEN,
  TT_CLOSE,
  TT_STRING,
  TT_ELSE
} token_type;

typedef struct lexer {
  token_type type;
  const char* src;
  const char* start;
  const char* end;
} lexer;

static void lexer_init(lexer *lex, const char* src) {
  lex->type = TT_ERR;
  lex->src = src;
  lex->start = src;
  lex->end = src;
}

static void lexer_print(lexer *lex) {
  const char* rev[] = {
    "TT_ERR", "TT_EOF", "TT_OPEN", "TT_CLOSE", "TT_STRING", "TT_ELSE"
  };
  if (lex->type != TT_ERR && lex->type != TT_EOF) {
    printf("type = %s, start = %zu, end = %zu, val = '%.*s'\n",
        rev[lex->type], lex->start - lex->src, lex->end - lex->src, 
        (int)(lex->end - lex->start), lex->start);
  } else {
    printf("type = %s, start = %zu, end = %zu, val = 'N/A'\n",
        rev[lex->type], lex->start - lex->src, lex->end - lex->src);
  }
}

static int lexer_next(lexer *lex) {
  const char* s = lex->end;
skip:
  while (isws(*s)) ++s;
  if (*s == ';') {
    while (*s != '\n' && *s != '\0') ++s;
    goto skip;
  }

  const char* start = s;

  if (*s == '\0') {
    lex->type = TT_EOF;
    goto done;
  }

  if (*s == '"') {
    ++s;
    while (*s != '\0' && *s != '\n' && *s != '"') {
      if (*s == '\\') ++s;
      ++s;
    }
    if (*s == '"') {
      ++s;
      lex->type = TT_STRING;
      goto done;
    } else {
      goto fail;
    }
  }

  if (*s == '(' || *s == '{' || *s == '[') {
    ++s;
    lex->type = TT_OPEN;
    goto done;
  }

  if (*s == ')' || *s == '}' || *s == ']') {
    ++s;
    lex->type = TT_CLOSE;
    goto done;
  }

  while (*s != 0 && strchr(" \n\t\f;({[]})\"", *s) == NULL) {
    ++s;
  }
  lex->type = TT_ELSE;
  goto done;

fail:
  lex->type = TT_ERR;
  return 0;

done:
  lex->start = start;
  lex->end = s;
  return 1;
}

sexp_t *sexp_read_string(lexer *lex);
sexp_t *sexp_read_symbol(lexer *lex);
sexp_t *sexp_read_number(lexer *lex);
sexp_t *sexp_read_list(lexer *lex);
sexp_t *sexp_read_list_items(lexer *lex);
sexp_t *sexp_read_any(lexer *lex);

sexp_t *sexp_read(const char* src, char** end) {
  lexer lex;
  lexer_init(&lex, src);
  lexer_next(&lex);
  sexp_t *res = sexp_read_any(&lex);
  if (end) *end = (char*)lex.end;
  return res;
}

sexp_t *sexp_read_any(lexer *lex) {
  sexp_t *res;
  switch(lex->type) {
  case TT_EOF:
  case TT_ERR:
  case TT_CLOSE:
    return NULL;
  case TT_OPEN:
    return sexp_read_list(lex);
  case TT_STRING:
    return sexp_read_string(lex);
  case TT_ELSE:
    if ((res = sexp_read_number(lex)) != NULL) return res;
    if ((res = sexp_read_symbol(lex)) != NULL) return res;
    return NULL;
  }
  die("unreachable");
}

sexp_t *sexp_read_string(lexer *lex) {
  if (lex->type != TT_STRING) return NULL;
  const char* start = lex->start+1;
  const char* end = lex->end-1;

  size_t len = unescaped_length(start, end-start);
  sexp_t *e = sexp_new_string_len(start, len);
  unescape(start, sexp_string_get_mut(e), end-start);

  lexer_next(lex);
  return e;
}

sexp_t *sexp_read_symbol(lexer *lex) {
  if (lex->type != TT_ELSE) return NULL;
  sexp_t *e = sexp_new_symbol_len(lex->start, lex->end - lex->start);
  lexer_next(lex);
  return e;
}

sexp_t *sexp_read_number(lexer *lex) {
  char* end;
  double val = strtod(lex->start, &end);
  if (end != lex->start) {
    lexer_next(lex);
    return sexp_new_number(val);
  } else {
    return NULL;
  }
}

sexp_t *sexp_read_list(lexer *lex) {
  char term = 0;
  if (*lex->start == '(') term = ')';
  else if (*lex->start == '[') term = ']';
  else if (*lex->start == '{') term = '}';
  assert(term != 0);
  lexer_next(lex);
  sexp_t *list = sexp_read_list_items(lex);
  if (list == NULL || lex->type != TT_CLOSE || *lex->start != term) {
    return NULL;
  }
  lexer_next(lex);
  return list;
}

sexp_t *sexp_read_list_items(lexer *lex) {
  sexp_t *list = sexp_new_list();
  while (lex->type != TT_CLOSE && lex->type != TT_EOF && lex->type != TT_ERR) {
    sexp_t *item = sexp_read_any(lex);
    list = sexp_list_append(list, item);
  }
  if (lex->type != TT_CLOSE) {
    sexp_free(list);
    return NULL;
  } else {
    return list;
  }
}

/******************************************************************************
 * PRINTER
 *****************************************************************************/

typedef struct {
  size_t len;
  size_t cap;
  char buf[];
} printer_t;

printer_t *printer_new() {
  printer_t *printer = malloc(sizeof(printer_t) + 64);
  if (!printer) die("out of memory");
  printer->len = 0;
  printer->cap = 64;
  return printer;
}

void printer_free(printer_t *printer) {
  free(printer);
}

printer_t *printer_ensure(printer_t *printer, size_t cap) {
  if (printer->cap <= cap) {
    size_t newcap = printer->cap;
    while (newcap < cap) newcap *= 1.5;
    printer = realloc(printer, sizeof(printer_t) + newcap);
    if (!printer) die("out of memory");
    printer->cap = newcap;
  }
  return printer;
}

char* printer_cstr(printer_t *printer) {
  char* buf = malloc(printer->len+1);
  if (!buf) die("out of memory");
  memcpy(buf, printer->buf, printer->len);
  buf[printer->len] = '\0';
  return buf;
}

printer_t *printer_append_lpstring(printer_t *p, const char* str, size_t len) {
  p = printer_ensure(p, p->len + len);
  memcpy(&(p->buf[p->len]), str, len);
  p->len += len;
  return p;
}

printer_t *printer_append_char(printer_t *printer, char ch) {
  printer = printer_ensure(printer, printer->len);
  printer->buf[printer->len] = ch;
  printer->len += 1;
  return printer;
}

static printer_t *printer_append_sexp(printer_t *p, const sexp_t *e);

static printer_t *printer_append_sexp_string(printer_t *p, const sexp_t *e) {
  sexp_string_t *s = (sexp_string_t*)e;
  p = printer_append_char(p, '"');
  const char* buf = s->val;
  int len = s->len;
  for (int i = 0; i < len; ++i) {
    switch (buf[i]) {
      case '\a': p = printer_append_lpstring(p, "\\a", 2); break;
      case '\b': p = printer_append_lpstring(p, "\\b", 2); break;
      case '\f': p = printer_append_lpstring(p, "\\f", 2); break;
      case '\n': p = printer_append_lpstring(p, "\\n", 2); break;
      case '\r': p = printer_append_lpstring(p, "\\r", 2); break;
      case '\t': p = printer_append_lpstring(p, "\\t", 2); break;
      case '\v': p = printer_append_lpstring(p, "\\v", 2); break;
      case '\?': p = printer_append_lpstring(p, "\\?", 2); break;
      case '\0': p = printer_append_lpstring(p, "\\0", 2); break;
      case '\"': p = printer_append_lpstring(p, "\\\"", 2); break;
      case '\'': p = printer_append_lpstring(p, "\\\'", 2); break;
      default: printer_append_char(p, buf[i]); break;
    }
  }
  p = printer_append_char(p, '"');
  return p;
}

static printer_t *printer_append_sexp_symbol(printer_t *p, const sexp_t *e) {
  sexp_symbol_t *sym = (sexp_symbol_t*)e;
  p = printer_append_lpstring(p, sym->val, sym->len);
  return p;
}

static printer_t *printer_append_sexp_number(printer_t *p, const sexp_t *e) {
  double val = ((sexp_num_t*)e)->val;
  char* res = NULL;
  int len = asprintf(&res, "%lg", val);
  p = printer_append_lpstring(p, res, len);
  return p;
}

static printer_t *printer_append_sexp_list(printer_t *p, const sexp_t *e) {
  p = printer_append_char(p, '(');
  size_t len = sexp_list_length(e);
  for (int i = 0; i < len; ++i) {
    if (i > 0) p = printer_append_char(p, ' ');
    p = printer_append_sexp(p, sexp_list_nth(e, i));
  }
  p = printer_append_char(p, ')');
  return p;
}

static printer_t *printer_append_sexp(printer_t *p, const sexp_t *e) {
  if (sexp_is_string(e)) {
    return printer_append_sexp_string(p, e);
  } else if (sexp_is_symbol(e)) {
    return printer_append_sexp_symbol(p, e);
  } else if (sexp_is_number(e)) {
    return printer_append_sexp_number(p, e);
  } else if (sexp_is_list(e)) {
    return printer_append_sexp_list(p, e);
  } else {
    die("invalid sexp type");
  }
}

char* sexp_display(sexp_t *e) {
  printer_t *p = printer_new();
  p = printer_append_sexp(p, e);
  char* res = printer_cstr(p);
  printer_free(p);
  return res;
}
