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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "json_writer.h"


#define ARRAY_BEGIN(arr) (arr)
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#define ARRAY_END(arr) (ARRAY_BEGIN(arr) + ARRAY_SIZE(arr))


char entry1_text[64] = {0};
char entry2_text[32] = {0};
char entry3_text[32] = {0};
int edgelock_state = 0;
int bootcounter = 0;

static json_writer json_writers[] = {

  {json_handler_otag, {.otag = "{\n"}, },

    {json_handler_entry_text, {.otag = "", .name = "entry1", .fmt = "%s", .ctag=",\n", .level=1}, entry1_text},
    {json_handler_entry_text, {.otag = "", .name = "entry2", .ctag=",\n", .level=1}, entry2_text},
    {json_handler_entry_text, {.otag = "", .name = "entry3", .ctag=",\n", .level=1}, entry3_text},
    {json_handler_otag, {.otag = "\"entry4\": {\n", .level=1}},

      {json_handler_entry_number, {.otag = "", .name = "entry5", .fmt = "%d", .ctag=",\n", .level=2}, &edgelock_state},
      {json_handler_entry_number, {.otag = "", .name = "entry6", .ctag="\n", .level=2}, &bootcounter},

    {json_handler_ctag, {.ctag = "}\n", .level=1}},

  {json_handler_ctag, {.ctag = "}"}, },
};

int main(int argc, char *argv[]) {

  if (1)
  {
    static uint8_t static_buffer[1024];
    json_buffer *p = alloc_json_buffer_static(sizeof(static_buffer), static_buffer);

    if (p == NULL)
      return EXIT_FAILURE;

    destroy_json_buffer(p);
  }

  if (1)
  {
    json_buffer *p = alloc_json_buffer(1024);

    if (!p)
      return EXIT_FAILURE;

    snprintf(entry1_text, sizeof(entry1_text), "entry1_text");
    snprintf(entry2_text, sizeof(entry2_text), "entry2_text");
    snprintf(entry3_text, sizeof(entry3_text), "entry3 text with spaces");
    edgelock_state = 42;
    bootcounter = 314;

    for (json_writer *writer = ARRAY_BEGIN(json_writers);
         writer != ARRAY_END(json_writers); ++writer)
    {
      if (writer->hndl) {
        writer->hndl(p, &writer->hdnl_data, writer->data);
      }
    }
    
    printf("%s\n", json_handler_string_buffer(p));

    printf("%s\n", json_handler_compress_in_place(p));
        
    destroy_json_buffer(p);
  }

  return EXIT_SUCCESS;
}

