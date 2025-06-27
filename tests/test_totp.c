/* test_totp.c
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

/* TOTP Algorithm: Test Suite
 *
 * The test token shared secret uses the ASCII string value
 *   "12345678901234567890".  With Time Step X = 30, and the Unix epoch as
 *   the initial value to count time steps, where T0 = 0, the TOTP
 *   algorithm will display the following values for specified modes and
 *   timestamps.
 *  +-------------+--------------+------------------+----------+--------+
 *  |  Time (sec) |   UTC Time   | Value of T (hex) |   TOTP   |  Mode  |
 *  +-------------+--------------+------------------+----------+--------+
 *  |      59     |  1970-01-01  | 0000000000000001 | 94287082 |  SHA1  |
 *  |             |   00:00:59   |                  |          |        |
 *  |  1111111109 |  2005-03-18  | 00000000023523EC | 07081804 |  SHA1  |
 *  |             |   01:58:29   |                  |          |        |
 *  |  1111111111 |  2005-03-18  | 00000000023523ED | 14050471 |  SHA1  |
 *  |             |   01:58:31   |                  |          |        |
 *  |  1234567890 |  2009-02-13  | 000000000273EF07 | 89005924 |  SHA1  |
 *  |             |   23:31:30   |                  |          |        |
 *  |  2000000000 |  2033-05-18  | 0000000003F940AA | 69279037 |  SHA1  |
 *  |             |   03:33:20   |                  |          |        |
 *  | 20000000000 |  2603-10-11  | 0000000027BC86AA | 65353130 |  SHA1  |
 *  |             |   11:33:20   |                  |          |        |
 *  +-------------+--------------+------------------+----------+--------+
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/totp.h"

int main(int argc, char **argv)
{
    const char *secret      = "12345678901234567890";

    int test_count          = 6;

    const uint64_t counts[] = {59,         1111111109, 1111111111,
                               1234567890, 2000000000, 20000000000};

    const uint32_t totps[]  = {94287082, 7081804,  14050471,
                               89005924, 69279037, 65353130};

    printf("Beginning TOTP test suite...\n");
    for (int i = 0; i < test_count; ++i) {
        uint32_t code;
        time_t   count;
        count = calculate_time(counts[i], T0);
        code  = totp(( const uint8_t * )secret, strlen(secret), count, 8);
        printf("TOTP(%s,%lu) = %d (expecting %d)\n", secret, counts[i], code,
               totps[i]);
        assert(code == totps[i]);
    }

    return 0;
}

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
