/* db.h
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

#ifndef _DB_H
#define _DB_H

#include <sys/types.h>

#include <limbo_sqlite3/sqlite3.h>

#define MAX_STR_LEN 512

typedef struct table_entry_t {
    uint64_t id;
    char     descriptor[MAX_STR_LEN];
    size_t   dlen;
    char     username[MAX_STR_LEN];
    size_t   ulen;
    char     password[MAX_STR_LEN];
    size_t   plen;
    char     opt_secret[MAX_STR_LEN];
    size_t   olen;
    int      otp_base32;
    size_t   ncount;
    char     notes[5][MAX_STR_LEN];
} table_entry_t;

typedef unsigned int teid_t;

int db_init_database(sqlite3 *db, const char *path, int flags);

int db_create_table(sqlite3 *db, const char *table_name, const char *schema);

int db_drop_table(sqlite3 *db, const char *table_name);

teid_t db_insert_entry(sqlite3 *db, table_entry_t *entry);

int db_get_entry(sqlite3 *db, table_entry_t *entry);

// int db_remove_entry_by_desc(sqlite3 *db, const char *descriptor);

int db_remove_entry(sqlite3 *db, teid_t entry_id);

int db_update_entry(sqlite3 *db, teid_t entry_id, table_entry_t *new_entry);

int db_deinit_database(sqlite3 *db, const char *path);

#endif /* #ifndef _DB_H */

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
