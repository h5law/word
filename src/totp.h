/* totp.h
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

#ifndef _TOTP_H
#define _TOTP_H

#include <stdint.h>
#include <sys/types.h>

#ifndef T0
#define T0 (( uint64_t )0)
#endif /* #ifndef T0 */

#ifndef TIME_STEP
#define TIME_STEP (( uint32_t )30)
#endif /* #ifndef TIME_STEP */

time_t get_time(void);

time_t calculate_time(time_t current, time_t offset);

uint32_t totp(const uint8_t *key, const size_t key_len, uint64_t counter,
              uint32_t digits);

#endif /* #ifndef _TOTP_H */

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
