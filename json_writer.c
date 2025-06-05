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
#include <ctype.h>

#include "json_writer.h"

typedef struct {
  char *buf;
  size_t len;
  size_t used;
  unsigned dynamic: 1;
} json_buffer;

static void* __alloc_json_buffer(size_t len, void *buf, int dynamic) {

  if (!buf)
    return NULL;

  json_buffer *const p = buf;

  p->buf = (char *)(((uint8_t *)p) + sizeof(*p));
  p->len = len;
  p->used = 0;
  p->dynamic = dynamic;

  memset(p->buf, 0, p->len);

  return p;
}

void* alloc_json_buffer(size_t len) {

  return __alloc_json_buffer(len, malloc(sizeof(json_buffer) + len), 1);
}

void* alloc_json_buffer_static(size_t len, void *buf) {

  if (len < sizeof(json_buffer))
    return NULL; /* Insufficient space. */

  return __alloc_json_buffer(len, buf, 0);
}


void destroy_json_buffer(void *p) {

  if (!p)
    return;

  if (((json_buffer *)p)->dynamic)
    free(p);
}

#define json_handler_snprintf(p, ...) \
  p->used += snprintf(p->buf + p->used, p->len - p->used, __VA_ARGS__)

#define json_handler_error_code(p) \
  ((p->used < p->len) ? 0 : 1)

#define tab_chars "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"

int json_handler_ctag(void *_p, const json_handler_data *hndl_data, void *data) {

  json_buffer *p = _p;
  json_handler_snprintf(p, "%.*s", hndl_data->level, tab_chars);
  json_handler_snprintf(p, "%s", hndl_data->ctag ? hndl_data->ctag : "");
  
  return json_handler_error_code(p);
}

int json_handler_otag(void *_p, const json_handler_data *hndl_data, void *data) {

  json_buffer *p = _p;
  json_handler_snprintf(p, "%.*s", hndl_data->level, tab_chars);
  json_handler_snprintf(p, "%s", hndl_data->otag ? hndl_data->otag : "");
  
  return json_handler_error_code(p);
}

int json_handler_entry_text(void *_p, const json_handler_data *hndl_data, void *data) {

  assert(hndl_data->name != NULL && strlen(hndl_data->name) > 0);
  assert(hndl_data->ctag != NULL && strlen(hndl_data->ctag) > 0);

  json_buffer *p = _p;
  json_handler_snprintf(p, "%.*s", hndl_data->level, tab_chars);
  json_handler_snprintf(p, "%s", hndl_data->otag ? hndl_data->otag : "");
  json_handler_snprintf(p, "\"%s\": \"", hndl_data->name);
  json_handler_snprintf(p, hndl_data->fmt ? hndl_data->fmt : "%s", data ? (const char *)data : "");
  json_handler_snprintf(p, "\"%s", hndl_data->ctag ? hndl_data->ctag : "");
  
  return json_handler_error_code(p);
}

int json_handler_entry_number(void *_p, const json_handler_data *hndl_data, void *data) {

  assert(hndl_data->name != NULL && strlen(hndl_data->name) > 0);
  assert(hndl_data->ctag != NULL && strlen(hndl_data->ctag) > 0);

  json_buffer *p = _p;
  json_handler_snprintf(p, "%.*s", hndl_data->level, tab_chars);
  json_handler_snprintf(p, "%s", hndl_data->otag ? hndl_data->otag : "");
  json_handler_snprintf(p, "\"%s\": ", hndl_data->name);
  json_handler_snprintf(p, hndl_data->fmt ? hndl_data->fmt : "%d", data ? *(int *)data : INT_MIN);
  json_handler_snprintf(p, "%s", hndl_data->ctag);
  
  return json_handler_error_code(p);
}

char* json_handler_string_buffer(void *p) {

  return ((json_buffer *)p)->buf;
}

char* json_handler_compress_in_place(void *p) {

  char *str = json_handler_string_buffer(p);
  int string_found = 0;
  char *r, *w;

  for (r = str, w = str; *r; ++r) {
    if (*r == '"') {
      string_found ^= 1;
    }

    if (string_found) {
      *w++ = *r;
    }
    else if (!isspace(*r)) {
      *w++ = *r;
    }
  }

  while (w != r) {
    *w++ = '\0';
  }

  return str;
}

