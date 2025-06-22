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

#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include "json_writer.h"


#define ARRAY_BEGIN(arr) (arr)
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#define ARRAY_END(arr) (ARRAY_BEGIN(arr) + ARRAY_SIZE(arr))


char entry1_text[64] = { 0 };
char entry2_text[64] = { 0 };
char entry3_text[64] = { 0 };
char entry4_text[64] = { 0 };
int edgelock_state = 0;
int bootcounter = 0;

static const json_writer json_writers[] = {

  { json_handler_otag, { .otag = "{\n" }, },

  { json_handler_entry_text, { .otag = "", .name = "entry1", .fmt = "%s", .ctag = ",\n", .level = 1 }, entry1_text },
  { json_handler_entry_text, { .otag = "", .name = "entry2", .ctag = ",\n", .level = 1 }, entry2_text },
  { json_handler_entry_text, { .otag = "", .name = "entry3", .ctag = ",\n", .level = 1 }, entry3_text },
  { json_handler_entry_text, { .otag = "", .name = "entry4", .ctag = ",\n", .level = 1 }, entry4_text },
  { json_handler_otag, { .otag = "\"entry5\": {\n", .level = 1 } },

  { json_handler_entry_number, { .otag = "", .name = "entry6", .fmt = "%d", .ctag = ",\n", .level = 2 }, &edgelock_state },
  { json_handler_entry_number, { .otag = "", .name = "entry7", .ctag = "\n", .level = 2 }, &bootcounter },

  { json_handler_ctag, { .ctag = "}\n", .level = 1 } },

  { json_handler_ctag, { .ctag = "}" }, },
};

static void _json_example1(void *p) {

  snprintf(entry1_text, sizeof(entry1_text), "entry1_text");
  snprintf(entry2_text, sizeof(entry2_text), "entry2_text");
  snprintf(entry3_text, sizeof(entry3_text), "entry3 text with spaces");
  snprintf(entry4_text, sizeof(entry4_text), "entry4 with \\\"\\\" \\\"\\\" quotes and \\\\ \\\\ \\\\ \\\\ backslashes");
  edgelock_state = 42;
  bootcounter = 314;

  for (const json_writer *writer = ARRAY_BEGIN(json_writers);
       writer != ARRAY_END(json_writers); ++writer) {
    if (writer->hndl) {
      writer->hndl(p, &writer->hdnl_data, writer->data);
    }
  }

  const char *uncompressed_string_a = json_get_string(p);
  printf("%s\n", uncompressed_string_a);

  const char *compressed_string_a = json_get_compressed_string(p);
  printf("%s\n", compressed_string_a);

  const char *compressed_string_b = R"({"entry1":"entry1_text","entry2":"entry2_text","entry3":"entry3 text with spaces","entry4":"entry4 with \"\" \"\" quotes and \\ \\ \\ \\ backslashes","entry5":{"entry6":42,"entry7":314}})";
  assert(strcmp(compressed_string_a, compressed_string_b) == 0);
}

int main(int argc, char *argv[]) {

  if (1) {
    static uint8_t static_buffer[1024];
    void *p = json_init_buffer(sizeof(static_buffer), static_buffer);

    if (!p)
      return EXIT_FAILURE;

    _json_example1(p);
  }

  #if !defined(NO_MALLOC)
  if (1) {
    void *p = json_alloc_buffer(1024);

    if (!p)
      return EXIT_FAILURE;

    _json_example1(p);

    json_destroy_buffer(p);
  }

  if (1) {
    try {
      JSONBufferWrapper p(1024);
      _json_example1(&p);
    }
    catch (const std::runtime_error &err) {
      std::cerr << err.what() << std::endl;
      return EXIT_FAILURE;
    }
    catch (...) {
      std::cerr << "Unknown error" << std::endl;
      return EXIT_FAILURE;
    }
  }
  #endif

  return EXIT_SUCCESS;
}

