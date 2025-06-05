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

#pragma once

#include <stddef.h>

typedef struct {
  char *buf;
  size_t len;
  size_t used;
  unsigned dynamic: 1;
} json_buffer;

typedef struct
{
  const char *otag; /**< Opening tag. */
  const char *name; /**< Entry name. */
  const char *fmt;  /**< Format. */
  const char *ctag; /**< Closing tag. */
  int level;        /* Amount of \t for pretty print. */
} json_handler_data;

typedef int (*json_handler)(json_buffer *p, const json_handler_data *hndl_data, void *data);

typedef struct {
  json_handler hndl;
  const json_handler_data hdnl_data;
  void *data;
} json_writer;

#ifdef __cplusplus
extern "C" {
#endif

json_buffer* alloc_json_buffer(size_t len);

json_buffer* alloc_json_buffer_static(size_t len, void *buf);

void destroy_json_buffer(json_buffer *p);

int json_handler_ctag(json_buffer *p, const json_handler_data *hndl_data, void *data);

int json_handler_otag(json_buffer *p, const json_handler_data *hndl_data, void *data);

int json_handler_entry_text(json_buffer *p, const json_handler_data *hndl_data, void *data);

int json_handler_entry_number(json_buffer *p, const json_handler_data *hndl_data, void *data);

const char* json_handler_compress_in_place(char *str);

#ifdef __cplusplus
}
#endif

