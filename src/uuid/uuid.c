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

#include <string.h>
#include <pthread.h>

#include "uuid.h"
#include "_uuid_impl.h"

/**
 * @brief
 * global lock for uuid
 **/
static pthread_mutex_t _g_lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief
 * clear uuid
 *
 * @param [out] uu uuid_t object
 **/
void uuid_clear(uuid_t uu) {
    memset(uu, 0, sizeof(uuid_t));
}

/**
 * @brief
 * compares the two supplied uuids
 *
 * @param [in] uu1 uuid_t object
 * @param [in] uu2 uuid_t object
 * @return
 * Returns an integer less than, equal to, or greater than zero
 * if uu1 is found. respectively, to be lexigraphically less
 * than, equal, or greater than uu2
 **/
int uuid_compare(const uuid_t uu1, const uuid_t uu2) {
    _uuid_t u1, u2;

    _uuid_unpack(&u1, uu1);
    _uuid_unpack(&u2, uu2);
    return _uuid_compare(&u1, &u2);
}

/**
 * @brief
 * copies the UUID variable src to dst
 *
 * @param [out] dst destination object of uuid
 * @param [in] src source object of uuid
 **/
void uuid_copy(uuid_t dst, const uuid_t src) {
    memcpy(dst, src, sizeof(uuid_t));
}

/**
 * @brief
 * creates a new time-based UUID
 *
 * @param [out] uuid new uuid object
 * @note
 * when fail to generate, out is nul object.
 * so, check with uuid_is_null after generate
 **/
void uuid_generate(uuid_t out) {
    _uuid_t u;

    uuid_clear(out);
    if (pthread_mutex_lock(&_g_lock) != 0) {
        return;
    }
    if (_uuid_generate(&u) != 0) {
        pthread_mutex_unlock(&_g_lock);
        return;
    }
    _uuid_pack(out, &u);
    pthread_mutex_unlock(&_g_lock);
}

/**
 * @brief
 * check uu is NUL object
 *
 * @param [in] uu uuid object
 * @retval 1 null
 * @retval 0 not null
 **/
int uuid_is_null(const uuid_t uu) {
    const char *puu = (const char *)uu;
    int i;

    for (i = 0; i < sizeof(uuid_t); i++) {
        if (*puu++ != 0) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief
 * converts uuid_string_t to uuid_t
 * The input is "%08x-%04x-%04x-%04x-%012x"
 *
 * @param [in] in input string
 * @param [out] out uuid object
 * @retval 0 success
 * @retval -1 fail
 **/
int uuid_parse(const uuid_string_t in, uuid_t out) {
    _uuid_t u;

    if (_uuid_parse(in, &u) != 0) {
        return -1;
    }
    _uuid_pack(out, &u);
    return 0;
}

/**
 * @brief
 * converts uuid_t to uuid_string_t
 * The output is "%08x-%04x-%04x-%04x-%012x"
 *
 * @param [in] uu uuid object
 * @param [out] out output string
 **/
void uuid_unparse(const uuid_t in, uuid_string_t out) {
    _uuid_t u;

    _uuid_unpack(&u, in);
    _uuid_unparse(&u, out);
    return;
}

/* EOF */
