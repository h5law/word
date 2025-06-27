/* hotp.h
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

#ifndef _HOTP_H
#define _HOTP_H

#include <stdint.h>
#include <sys/types.h>

#ifndef HS_LEN
#define HS_LEN 20 /* 20 bytes - 160 bits */
#endif            /* #ifndef HS_LEN */

#ifndef DIGITS
#define DIGITS 6  /* 6 decimal digits determined by HOTP */
#endif            /* #ifndef DIGITS */

uint32_t hotp(uint8_t hmac[HS_LEN]);

void hmac_sha1(const uint8_t *key, const size_t key_len, const uint8_t *data,
               const size_t data_len, uint8_t hash[HS_LEN],
               unsigned int *hash_len);

uint8_t *reverse_str(const uint8_t *data, uint8_t *rev, size_t data_len);
uint64_t reverse_64(uint64_t word);

#endif /* #ifndef _HOTP_H */

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
