/* db.c
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

#include <stdio.h>
#include <stdlib.h>

#include <limbo_sqlite3/sqlite3.h>

#include "db.h"

char *db_path;

static const char *create_tbl_sql = "CREATE TABLE WORDS_TBL("
                                    "ID INT PRIMARY        KEY      NOT NULL,"
                                    "DESCRIPTOR            TEXT     NOT NULL,"
                                    "DLEN                  INT      NOT NULL,"
                                    "USERNAME              TEXT     ,"
                                    "ULEN                  INT      0,"
                                    "PASSWORD              TEXT     ,"
                                    "PLEN                  INT      0,"
                                    "OTP_SECRET            TEXT     ,"
                                    "OLEN                  INT      0,"
                                    "OTP_BASE32            INT      1,"
                                    "NCOUNT                INT      0,"
                                    "NOTE1                 TEXT     ,"
                                    "NOTE2                 TEXT     ,"
                                    "NOTE3                 TEXT     ,"
                                    "NOTE4                 TEXT     ,"
                                    "NOTE5                 TEXT     );";

sqlite3 *init_db(void)
{
    sqlite3 *db      = NULL;
    int      rc      = 0;
    char    *err_msg = NULL;

    rc               = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error: cannot open db (%s)\n\t%s\n", db_path,
                sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_exec(db, create_tbl_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error: error creating table (%s)\n\t%s\n", err_msg,
                sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    return db;
}

int create_record(sqlite3 *db, table_entry_t *entry)
{
    const char *sql =
            "INSERT INTO ENTRIES (ID, DESCRIPTOR, DLEN, USERNAME, ULEN, "
            "PASSWORD, PLEN, OTP_SECRET, OLEN, NCOUNT, NOTE1, NOTE2, NOTE3, "
            "NOTE4, NOTE5) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    int           rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int64(stmt, 1, entry->id);
    sqlite3_bind_text(stmt, 2, entry->descriptor, 512, NULL);
    sqlite3_bind_int64(stmt, 3, entry->dlen);
    sqlite3_bind_text(stmt, 4, entry->username, 512, NULL);
    sqlite3_bind_int64(stmt, 5, entry->ulen);
    sqlite3_bind_text(stmt, 6, entry->password, 512, NULL);
    sqlite3_bind_int64(stmt, 7, entry->plen);
    sqlite3_bind_text(stmt, 8, entry->opt_secret, 512, NULL);
    sqlite3_bind_int64(stmt, 9, entry->olen);
    sqlite3_bind_int64(stmt, 10, entry->olen);
    sqlite3_bind_int64(stmt, 11, entry->ncount);
    sqlite3_bind_text(stmt, 12, entry->notes[0], 512, NULL);
    sqlite3_bind_text(stmt, 13, entry->notes[1], 512, NULL);
    sqlite3_bind_text(stmt, 14, entry->notes[2], 512, NULL);
    sqlite3_bind_text(stmt, 15, entry->notes[3], 512, NULL);
    sqlite3_bind_text(stmt, 16, entry->notes[4], 512, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "error: execution failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? SQLITE_OK : rc;
}

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
