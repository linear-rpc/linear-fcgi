/*
 * The MIT License (MIT)
 *
 * Copyright 2015 Sony Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Implementation of UUID (Time Based) Generator

#ifndef _UUID_H_
#define _UUID_H_

/**
 * @brief
 * type definition of uuid_t
 */
typedef unsigned char uuid_t[16];
/**
 * @brief
 * type definition of uuid_string_t
 */
typedef char uuid_string_t[37];

#ifdef __cplusplus
extern "C" {
#endif

    void uuid_clear(uuid_t uu);
    int uuid_compare(const uuid_t uu1, const uuid_t uu2);
    void uuid_copy(uuid_t dst, const uuid_t src);
    void uuid_generate(uuid_t out);
    int uuid_is_null(const uuid_t uu);
    int uuid_parse(const uuid_string_t in, uuid_t out);
    void uuid_unparse(const uuid_t in, uuid_string_t out);

#ifdef __cplusplus
}
#endif

#endif /* _UUID_H_ */
