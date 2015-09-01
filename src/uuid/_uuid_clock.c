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
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "_uuid_clock.h"

/**
 * @brief
 * get clock sequence
 *
 * @param [in] seq_low low 8bit of clock sequence
 * @param [in] seq_hi high 6bit of clock sequence
 */
void _uuid_get_clock(uint8_t *seq_low, uint8_t *seq_hi) {
    int fd = -1;
    char buf[2];
    pid_t pid;
    unsigned int seed = 0;
    static int is_initialized = 0;
    static long seq = 0;

    assert(seq_low != NULL && seq_hi != NULL);
    if (!is_initialized) {
        is_initialized = 1;
        /* generate random number */
        fd = open("/dev/urandom", O_RDONLY);
        if (fd < 0 || (read(fd, buf, 2) < 2)) {
            pid = getpid();
            seed = (unsigned int)(  (unsigned int)(pid << 16)
                                  ^ (unsigned int)(pid)
                                  ^ (unsigned int)(time(NULL)) );
            srandom(seed);
            seq = random();
        } else {
            seq = (long)(buf[0] << 8) + (long)(buf[1]);
        }
        if (fd >= 0) {
            close(fd);
        }
    } else {
        seq++;
    }
    /* get low 8bits */
    *seq_low = (uint8_t)(seq & 0xff);
    /* get high 6bits */
    *seq_hi = (uint8_t)((seq >> 8) & 0x3f);
    return;
}
