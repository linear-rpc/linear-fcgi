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
#include <sys/time.h>
#include <assert.h>

#include "_uuid_time.h"

/**
 * Time Offset from 15/Oct/1582 00:00:00.00
 *             to    1/Jan/1970 00:00:00.00
 **/
#define __TIME_OFFSET (0x01b21dd213814000LL)

/**
 * @brief
 * get time
 *
 * @param [out] time_low
 * low 32bit of 60bit time information
 * @param [out] time_mid
 * middle 33-48bit of 60bit time information
 * @param [out] time_hi
 * high 12bit of 60bit time information
 */
void _uuid_get_time(uint32_t *time_low, uint16_t *time_mid, uint16_t *time_hi) {
    struct timeval tv;
    /* Offset to adjust UUID UTC TIME */
    uint64_t uutime = __TIME_OFFSET;

    assert(time_low != NULL && time_mid != NULL && time_hi != NULL);
    /* get Time */
    assert(gettimeofday(&tv, NULL) == 0);
    /* Convert To 100 nsec order */
    uutime += ((uint64_t)tv.tv_usec * 10 + (uint64_t)tv.tv_sec * 10000000);
    /* Convert UUID Time low,mid,high */
    *time_low = (uint32_t)(uutime & 0xffffffff);
    uutime = uutime >> 32;
    *time_mid = (uint16_t)(uutime & 0xffff);
    uutime = uutime >> 16;
    *time_hi = (uint16_t)(uutime & 0x0fff);
    return;
}
