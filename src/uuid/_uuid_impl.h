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

#ifndef __UUID_IMPL_H_
#define __UUID_IMPL_H_

#include <stdint.h>

#include "uuid.h"
#include "_uuid_node.h"

/**
 * @brief
 * sims_uuid_t implementation struct
 */
typedef struct _uuid_t {
    /** low 32bit of 60bit time information */
    uint32_t time_low;
    /** middle 33-48bit of 60bit time information */
    uint16_t time_mid;
    /** high 12bit of 60bit time information */
    uint16_t time_hi_and_version;
    /** high 6bit of 16bit clock sequence and 2bit reserved */
    uint8_t clock_seq_hi_and_reserved;
    /** low 8bit of 16bit clock sequence */
    uint8_t clock_seq_low;
    /** valid node(mac) address */
    uuid_node_t node;
} _uuid_t;

#ifdef __cplusplus
extern "C" {
#endif

    int _uuid_generate(_uuid_t *uu);
    int _uuid_compare(const _uuid_t *u1, const _uuid_t *u2);
    int _uuid_parse(const uuid_string_t in, _uuid_t *out);
    void _uuid_unparse(const _uuid_t *uu, uuid_string_t out);
    void _uuid_pack(uuid_t dst, const _uuid_t *src);
    void _uuid_unpack(_uuid_t *dst, const uuid_t src);

#ifdef __cplusplus
}
#endif

#endif /* __UUID_IMPL_H_ */
