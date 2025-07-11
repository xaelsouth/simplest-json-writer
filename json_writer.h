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
  const char *const otag; /**< Opening tag. */
  const char *const name; /**< Entry name. */
  const char *const fmt;  /**< Format. */
  const char *const ctag; /**< Closing tag. */
  int level;        /**< Amount of \t for pretty print. */
} json_handler_data;

typedef int (*json_handler)(void *p, const json_handler_data *hndl_data, void *const data);

typedef struct {
  const json_handler hndl;
  const json_handler_data hdnl_data;
  void *const data;
} json_writer;

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NO_MALLOC)
__attribute__((weak)) void* json_malloc(size_t len);

__attribute__((weak)) void json_free(void *p);

/** @brief Allocate work buffer. */
void* json_alloc_buffer(size_t len);

/** @brief Deallocate work buffer. */
void json_destroy_buffer(void *p);
#endif

/** @brief Initialise work buffer from static memory; No deallocation is needed. */
void* json_init_buffer(size_t len, void *buf);

int json_handler_ctag(void *p, const json_handler_data *hndl_data, void *const data);

int json_handler_otag(void *p, const json_handler_data *hndl_data, void *const data);

int json_handler_entry_text(void *p, const json_handler_data *hndl_data, void *const data);

int json_handler_entry_number(void *p, const json_handler_data *hndl_data, void *const data);

/** @brief Returns formatted JSON string. */
char* json_get_string(void *p);

/** @brief Returns JSON string without any space characters. */
char* json_get_compressed_string(void *p);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <stdexcept>

class JSONBufferWrapper final {
  void *p;

  public:

  JSONBufferWrapper() = delete;
  JSONBufferWrapper(const JSONBufferWrapper&) = delete;
  JSONBufferWrapper& operator=(const JSONBufferWrapper&) = delete;
  JSONBufferWrapper(JSONBufferWrapper&&) = delete;
  JSONBufferWrapper& operator=(JSONBufferWrapper&&) = delete;

  #if !defined(NO_MALLOC)
  explicit JSONBufferWrapper(std::size_t len) : p(json_alloc_buffer(len)) {

    if (!p) {
      throw std::runtime_error("Can't allocate JSON buffer");
    }
  }

  ~JSONBufferWrapper() noexcept {
    json_destroy_buffer(p);
  }
  #endif

  explicit JSONBufferWrapper(std::size_t len, void *buf) : p(json_init_buffer(len, buf)) {

    if (!p) {
      throw std::runtime_error("Insufficient buffer length provided");
    }
  }

  void* operator&() {
    return p;
  }

  const void* operator&() const {
    return p;
  }
};
#endif

