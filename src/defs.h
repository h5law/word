/* defs.h
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

#ifndef _DEFS_H
#define _DEFS_H

#include <sys/types.h>

#ifndef PROGNAME
#define PROGNAME "words"
#endif /* #ifndef PROGNAME */

#ifndef MAX_KEY_SIZE
#define MAX_KEY_SIZE 512
#endif /* #ifndef MAX_KEY_SIZE */

#ifndef MAX_VALUE_SIZE
#define MAX_VALUE_SIZE 4096
#endif /* #ifndef MAX_VALUE_SIZE */

typedef struct key_value_t {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} key_value_t;

typedef struct store_entry_t {
    char         descriptor[MAX_KEY_SIZE];
    key_value_t *entries;
    size_t       count;
} store_entry_t;

#endif /* #ifndef _DEFS_H */

// vim: ft=c ts=4 sts4 sw=4 et ai cin
