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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "_uuid_impl.h"
#include "_uuid_time.h"
#include "_uuid_clock.h"

/**
 * @brief
 * creates a new universally unique identifier UUID
 *
 * @param [out] uuid new uuid object
 * @retval <0 failed generate
 * @retval 0 success generate
 **/
int _uuid_generate(_uuid_t *uu) {
    assert(uu != NULL);
    /* get node_id */
    if (_uuid_get_node(uu->node) < 0) {
        return -1;
    }
    /* get Time Stamp */
    _uuid_get_time(&uu->time_low, &uu->time_mid, &uu->time_hi_and_version);
    /* Set Version to time_hi_and_version Part */
    uu->time_hi_and_version = uu->time_hi_and_version | 0x1000;
    /* get Clock Sequence */
    _uuid_get_clock(&uu->clock_seq_low, &uu->clock_seq_hi_and_reserved);
    /* Set Reserved to clock_seq_hi_and_reserved Part */
    uu->clock_seq_hi_and_reserved = uu->clock_seq_hi_and_reserved | 0x80;
    return 0;
}

/**
 * @brief
 * compares the two supplied uuid variables
 *
 * @param [in] u1 pointer to _uuid_t object
 * @param [in] u2 pointer to _uuid_t object
 * @return
 * Returns an integer less than, equal to, or greater than zero
 * if uuid1 is found. respectively, to be lexigraphically less
 * than, equal, or greater than uuid2
 **/
int _uuid_compare(const _uuid_t *u1, const _uuid_t *u2) {
    assert(u1 != NULL && u2 != NULL);
    if (u1->time_low != u2->time_low) {
        return (u1->time_low < u2->time_low) ? -1 : 1;
    }
    if (u1->time_mid != u2->time_mid) {
        return (u1->time_mid < u2->time_mid) ? -1 : 1;
    }
    if (u1->time_hi_and_version != u2->time_hi_and_version) {
        return (u1->time_hi_and_version < u2->time_hi_and_version) ? -1 : 1;
    }
    if (u1->clock_seq_hi_and_reserved != u2->clock_seq_hi_and_reserved) {
        return (u1->clock_seq_hi_and_reserved < u2->clock_seq_hi_and_reserved) ?
            -1 : 1;
    }
    if (u1->clock_seq_low != u2->clock_seq_low) {
        return (u1->clock_seq_low < u2->clock_seq_low) ? -1 : 1;
    }
    return memcmp(u1->node, u2->node, sizeof(uuid_node_t));
}

/**
 * @brief
 * converts string to _uuid_t
 * The input is "%08x-%04x-%04x-%04x-%012x"
 *
 * @param [in] in input string
 * @param [out] out _uuid_t object
 * @retval 0 success
 * @retval -1 fail
 **/
int _uuid_parse(const uuid_string_t in, _uuid_t *out) {
    const char *pin = in;
    size_t siz = sizeof(uuid_string_t);
    int i;
    char tmp[3];

    assert(out != NULL);
    for (i = 0; i < siz - 1; i++, pin++) {
        if ((i == 8) || (i == 13) || (i == 18) || (i == 23)) {
            if ((*pin) =='-') {
                continue;
            } else {
                return -1;
            }
        }
        if (!isxdigit(*pin)) {
            return -1;
        }
    }
    out->time_low = (uint32_t)(strtoul(in, NULL, 16));
    out->time_mid = (uint16_t)(strtoul(in + 9, NULL, 16));
    out->time_hi_and_version = (uint16_t)(strtoul(in + 14, NULL, 16));
    out->clock_seq_hi_and_reserved =
        (uint8_t)((strtoul(in + 19, NULL, 16) & 0xff00) >> 8);
    out->clock_seq_low = (uint8_t)(strtoul(in + 19, NULL, 16) & 0x0ff);
    pin = in + 24;
    tmp[2] = '\0';
    for (i = 0; i < 6; i++) {
        tmp[0] = *pin++;
        tmp[1] = *pin++;
        out->node[i] = (unsigned char) ( strtoul(tmp, NULL, 16) );
    }
    return 0;
}

/**
 * @brief
 * converts the supplied UUID from the binary representation 
 * into a 36byte string (plus tailing '\\0')
 * The output is a string of the form
 * "%08x-%04x-%04x-%04x-%012x" + '\\0'
 *
 * @param [in] uuid pointer to _uuid_t object
 * @param [out] str output string
 **/
void _uuid_unparse(const _uuid_t *uu, uuid_string_t out) {
    assert(uu != NULL);
    snprintf(out, sizeof(uuid_string_t),
             "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uu->time_low, uu->time_mid, uu->time_hi_and_version,
             uu->clock_seq_hi_and_reserved, uu->clock_seq_low,
             uu->node[0], uu->node[1], uu->node[2],
             uu->node[3], uu->node[4], uu->node[5]);
}

/**
 * @brief
 * convert _uuid_t to uuid_t
 *
 * @param [out] dst uuid_t object
 * @param [in] src _uuid_t object
 **/
void _uuid_pack(uuid_t dst, const _uuid_t *src) {
    assert(src != NULL);
    dst[0] = (unsigned char)((src->time_low >> 24) & 0xff);
    dst[1] = (unsigned char)((src->time_low >> 16) & 0xff);
    dst[2] = (unsigned char)((src->time_low >> 8) & 0xff);
    dst[3] = (unsigned char)(src->time_low & 0xff);
    dst[4] = (unsigned char)((src->time_mid >> 8) & 0xff);
    dst[5] = (unsigned char)(src->time_mid & 0xff);
    dst[6] = (unsigned char)((src->time_hi_and_version >> 8) & 0xff);
    dst[7] = (unsigned char)(src->time_hi_and_version & 0xff);
    dst[8] = (unsigned char)(src->clock_seq_hi_and_reserved);
    dst[9] = (unsigned char)(src->clock_seq_low);
    memcpy(&dst[10], src->node, sizeof(uuid_node_t));
    return;
}

/**
 * @brief
 * convert uuid_t to _uuid_t
 *
 * @param [out] dst _uuid_t object
 * @param [in] src uuid_t object
 **/
void _uuid_unpack(_uuid_t *dst, const uuid_t src) {
    int i;

    assert(dst != NULL);
    dst->time_low = 0;
    for (i = 0; i < 4; i++) {
        dst->time_low = dst->time_low << 8;
        dst->time_low = dst->time_low + src[i];
    }
    dst->time_mid = 0;
    for (; i < 6; i++) {
        dst->time_mid = dst->time_mid << 8;
        dst->time_mid = dst->time_mid + src[i];
    }
    dst->time_hi_and_version = 0;
    for (; i < 8; i++) {
        dst->time_hi_and_version = dst->time_hi_and_version << 8;
        dst->time_hi_and_version = dst->time_hi_and_version + src[i];
    }
    dst->clock_seq_hi_and_reserved = (uint8_t)src[8];
    dst->clock_seq_low = (uint8_t)src[9];
    memcpy(dst->node, &src[10], sizeof(uuid_node_t));
    return;
}
