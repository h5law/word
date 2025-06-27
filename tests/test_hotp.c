/* test_hotp.c
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

/* HOTP Algorithm: Test Suite
 *
 * The following test data uses the ASCII string
 * "12345678901234567890" for the secret:
 * Secret = 0x3132333435363738393031323334353637383930
 *
 * Count    Hexadecimal HMAC-SHA-1(secret, count)
 * 0        cc93cf18508d94934c64b65d8ba7667fb7cde4b0
 * 1        75a48a19d4cbe100644e8ac1397eea747a2d33ab
 * 2        0bacb7fa082fef30782211938bc1c5e70416ff44
 * 3        66c28227d03a2d5529262ff016a1e6ef76557ece
 * 4        a904c900a64b35909874b33e61c5938a8e15ed1c
 * 5        a37e783d7b7233c083d4f62926c7a25f238d0316
 * 6        bc9cd28561042c83f219324d3c607256c03272ae
 * 7        a4fb960c0bc06e1eabb804e5b397cdc4b45596fa
 * 8        1b3c89f65e6c9e883012052823443f048b4332db
 * 9        1637409809a679dc698207310c8c7fc07290d9e5
 *
 *                   Truncated
 * Count    Hexadecimal    Decimal        HOTP
 * 0        4c93cf18       1284755224     755224
 * 1        41397eea       1094287082     287082
 * 2         82fef30        137359152     359152
 * 3        66ef7655       1726969429     969429
 * 4        61c5938a       1640338314     338314
 * 5        33c083d4        868254676     254676
 * 6        7256c032       1918287922     287922
 * 7         4e5b397         82162583     162583
 * 8        2823443f        673399871     399871
 * 9        2679dc69        645520489     520489
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "../src/hotp.h"

const uint8_t  secret[21] = "12345678901234567890";
const uint32_t digits     = 6;

uint32_t hotps[]          = {755224, 287082, 359152, 969429, 338314,
                             254676, 287922, 162583, 399871, 520489};

int main(int argc, char **argv)
{
    printf("Beginning HOTP test suite...\n");
    for (int i = 0; i < 10; ++i) {
        uint32_t otp = hotp(secret, 21, i, 6);
        printf("HOTP(%s,%d) = %d (expecting %d)\n", secret, i, otp, hotps[i]);
        assert(otp == hotps[i]);
    }
}

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
