/*-
 * Copyright (c) 2025 <xael.south@yandex.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "json_writer.h"

typedef struct {
  char *buf;
  size_t len;
  size_t used;
  unsigned dynamic: 1;
} json_buffer;

static inline void* __json_init_buffer(size_t len, void *buf, int dynamic) {

  if (!buf)
    return NULL;

  json_buffer * const p = buf;

  p->buf = (char*)(((uint8_t*)p) + sizeof(*p));
  p->len = len;
  p->used = 0;
  p->dynamic = dynamic;

  memset(p->buf, 0, p->len);

  return p;
}

#if !defined(NO_MALLOC)
__attribute__((weak)) void* json_malloc(size_t len) { return (malloc(len)); }

__attribute__((weak)) void json_free(void *p) { free(p); }

void* json_alloc_buffer(size_t len) {

  return (__json_init_buffer(len, json_malloc(sizeof(json_buffer) + len), 1));
}

void json_destroy_buffer(void *p) {

  if (!p)
    return;

  if (((json_buffer*)p)->dynamic)
    json_free(p);
}
#endif

void* json_init_buffer(size_t len, void *buf) {

  if (len < sizeof(json_buffer))
    return NULL; /* Insufficient space. */

  return (__json_init_buffer(len, buf, 0));
}

static inline size_t __json_handler_snprintf(json_buffer *p, const char *format, ...) {

  va_list valist;
  va_start(valist, format);
  p->used += vsnprintf(p->buf + p->used, p->len - p->used, format, valist);
  va_end(valist);

  return (p->used);
}

static inline int __json_handler_error_code(json_buffer *p) {

  return ((p->used < p->len) ? 0 : 1);
}

/** 32 tab characters. */
static const char tab_chars[] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

int json_handler_ctag(void *p, const json_handler_data *hndl_data, void * const data) {

  __json_handler_snprintf(p, "%.*s", hndl_data->level, tab_chars);
  __json_handler_snprintf(p, "%s", hndl_data->ctag ? hndl_data->ctag : "");

  return (__json_handler_error_code(p));
}

int json_handler_otag(void *p, const json_handler_data *hndl_data, void * const data) {

  __json_handler_snprintf(p, "%.*s", hndl_data->level, tab_chars);
  __json_handler_snprintf(p, "%s", hndl_data->otag ? hndl_data->otag : "");

  return (__json_handler_error_code(p));
}

int json_handler_entry_text(void *p, const json_handler_data *hndl_data, void * const data) {

  assert(hndl_data->name != NULL && strlen(hndl_data->name) > 0);
  assert(hndl_data->ctag != NULL && strlen(hndl_data->ctag) > 0);

  __json_handler_snprintf(p, "%.*s", hndl_data->level, tab_chars);
  __json_handler_snprintf(p, "%s", hndl_data->otag ? hndl_data->otag : "");
  __json_handler_snprintf(p, "\"%s\": \"", hndl_data->name);
  __json_handler_snprintf(p, hndl_data->fmt ? hndl_data->fmt : "%s", data ? (const char*)data : "");
  __json_handler_snprintf(p, "\"%s", hndl_data->ctag ? hndl_data->ctag : "");

  return (__json_handler_error_code(p));
}

int json_handler_entry_number(void *p, const json_handler_data *hndl_data, void * const data) {

  assert(hndl_data->name != NULL && strlen(hndl_data->name) > 0);
  assert(hndl_data->ctag != NULL && strlen(hndl_data->ctag) > 0);

  __json_handler_snprintf(p, "%.*s", hndl_data->level, tab_chars);
  __json_handler_snprintf(p, "%s", hndl_data->otag ? hndl_data->otag : "");
  __json_handler_snprintf(p, "\"%s\": ", hndl_data->name);
  __json_handler_snprintf(p, hndl_data->fmt ? hndl_data->fmt : "%d", data ? *(int*)data : INT_MIN);
  __json_handler_snprintf(p, "%s", hndl_data->ctag);

  return (__json_handler_error_code(p));
}

char* json_get_string(void *p) { return (((json_buffer*)p)->buf); }

typedef enum {
  STATE_CHAR = 0, STATE_QUOTE, STATE_ESC, STATE_ERROR, STATE_MAX
} FSMState;

typedef FSMState (*state_hndl)(char ch, char **w);

static FSMState __state_char(char ch, char **w) {

  if (ch == '"') {
    **w = ch;
    (*w)++;
    return STATE_QUOTE;
  }
  else if (ch == '\\') {
    assert(0); /* Escape character only possible if in quotes. */
    return STATE_ERROR;
  }
  else if (!isspace(ch)) {
    **w = ch;
    (*w)++;
  }

  return STATE_CHAR;
}

static FSMState __state_quote(char ch, char **w) {

  if (ch == '"') {
    **w = ch;
    (*w)++;
    return STATE_CHAR;
  }
  else if (ch == '\\') {
    **w = ch;
    (*w)++;
    return STATE_ESC;
  }

  **w = ch;
  (*w)++;
  return STATE_QUOTE;
}

static FSMState __state_esc(char ch, char **w) {

  if (ch == '"') {
    **w = ch;
    (*w)++;
    return STATE_QUOTE;
  }
  else if (ch == '\\') {
    **w = ch;
    (*w)++;
    return STATE_QUOTE;
  }

  assert(0);
  return STATE_ERROR;
}

static FSMState __state_error(char ch, char **w) {

  (void)ch;
  (void)w;

  assert(0);
  return STATE_ERROR;
}

char* json_get_compressed_string(void *p) {

  static const state_hndl states[] = {
    [STATE_CHAR] = __state_char,
    [STATE_QUOTE] = __state_quote,
    [STATE_ESC] = __state_esc,
    [STATE_ERROR] = __state_error,
  };

  _Static_assert(STATE_MAX == sizeof(states) / sizeof(states[0]), "Unimplemented state");

  char * const str = json_get_string(p);
  char *r = str, *w = str;
  FSMState state = STATE_CHAR;

  /* Process input string. */
  while (*r && state != STATE_ERROR) {
    state = states[state](*r++, &w);
  }

  /* Fill up the result string with '\0'. */
  while (w != r) {
    *w++ = '\0';
  }

  return str;
}

