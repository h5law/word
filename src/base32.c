/* base32.c
 * Copyright (c) 2025 - h5law <dev@h5law.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int base32_decode_char(char c)
{
    char ret = -1;

    if (c >= 'A' && c <= 'Z')
        ret = c - 'A';
    else if (c >= '2' && c <= '7')
        ret = c - '2' + 26;
    else if (c >= 'a' && c <= 'z')
        ret = c - 'a';
    else
        ret = -1;

    assert(ret == -1 || (ret & 0x1F) == ret);

    return ret;
}

int base32_decode(char **dst, size_t *dstlen, const char *src, size_t srclen)
{
    size_t padlen  = 0;
    size_t lastlen = 0;

    *dst           = NULL;
    *dstlen        = 0;

    // Check padding
    for (size_t i = 1; i < srclen; i++) {
        if (src[srclen - i] == '=')
            padlen++;
        else
            break;
    }

    // Check source
    for (size_t i = 0; i < srclen - padlen; i++) {
        if (base32_decode_char(src[i]) > 0x1F) {
            return -1;
        }
    }

    // Calculate the length of the last quantum in src
    lastlen             = (srclen - padlen) % 8;
    size_t num_quantums = (srclen - padlen) / 8;

    if (lastlen > 0)
        num_quantums += 1; // Last quantum is a partial quantum
    else
        lastlen = 8;       // Last quantum is a full quantum

    // Calculate dst buffer size
    *dstlen = ((srclen - padlen) / 8) * 5;

    switch (lastlen) {
    case 8:
        break;
    case 7:
        *dstlen += 4;
        break;
    case 5:
        *dstlen += 3;
        break;
    case 4:
        *dstlen += 2;
        break;
    case 2:
        *dstlen += 1;
        break;
    default:
        // ERROR: Not a multiple of a byte.
        *dstlen = 0;
        break;
    }

    if (dstlen == 0) {
        return -1;
    }

    *dst = ( char * )malloc(sizeof(char) * (*dstlen));

    size_t      qlen;
    char       *pdst = *dst;
    const char *psrc = src;

    for (size_t q = 0; q < num_quantums; q++) {
        if (q == num_quantums - 1)
            qlen = lastlen;
        else
            qlen = 8;

        // dst       0           1          2          3           4
        //      [11111 111][11 11111 1][1111 1111][1 11111 11][111 11111]
        // src     0      1      2      3       4      5      6      7

        switch (qlen) {
        case 8: // 8 = 5 bytes in quantum
            pdst[4]  = base32_decode_char(psrc[7]);
            pdst[4] |= base32_decode_char(psrc[6]) << 5;
        case 7: // 7 = 4 bytes in quantum
            pdst[3]  = base32_decode_char(psrc[6]) >> 3;
            pdst[3] |= (base32_decode_char(psrc[5]) & 0x1F) << 2;
            pdst[3] |= base32_decode_char(psrc[4]) << 7;
        case 5: // 5 = 3 bytes in quantum
            pdst[2]  = base32_decode_char(psrc[4]) >> 1;
            pdst[2] |= base32_decode_char(psrc[3]) << 4;
        case 4: // 4 = 2 bytes in quantum
            pdst[1]  = base32_decode_char(psrc[3]) >> 4;
            pdst[1] |= (base32_decode_char(psrc[2]) & 0x1F) << 1;
            pdst[1] |= base32_decode_char(psrc[1]) << 6;
        case 2: // 2 = 1 byte in quantum
            pdst[0]  = base32_decode_char(psrc[1]) >> 2;
            pdst[0] |= base32_decode_char(psrc[0]) << 3;
            break;
        default:
            break;
        }

        psrc += 8;
        pdst += 5;
    }

    return 0;
}
