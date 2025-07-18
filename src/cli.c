/* cli.c
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

#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "base32.h"
#include "totp.h"
#include "defs.h"
#include "ui.h"

void print_usage(void)
{
    fprintf(stderr, "Usage:\t%s [OPTIONS] [COMMANDS]\n", PROGNAME);
    fprintf(stderr, "    \n\tOPTIONS:\n");
    fprintf(stderr, "      \t  -s <str> (secret key max 256)\n");
    fprintf(stderr,
            "      \t  -o <uint64_t> (offset to start counting from)\n");
    fprintf(stderr,
            "      \t  -t <uint32_t> (time period each TOTP is valid for)\n");
    fprintf(stderr, "      \t  -b (base32 encoded secret)\n");
    fprintf(stderr, "    \n\tCOMMANDS:\n");
    fprintf(stderr, "      \t  generate (produce a TOTP)\n");
}

int main(int argc, char **argv)
{
    int      opt          = 0;
    char     secret[1024] = {0};
    uint64_t offset       = 0;
    uint64_t step         = 30;
    size_t   len          = 0;

    int      sfound       = 0;
    int      base32_key   = 0;
    uint8_t *key          = NULL;
    size_t   key_len      = 0;

    while ((opt = getopt(argc, argv, "s:o:t:b")) != -1) {
        switch (opt) {
        case 's':
            len = strlen(optarg);
            memmove(secret, optarg, len);
            ++sfound;
            break;
        case 'o':
            offset = atoll(optarg);
            break;
        case 't':
            step = atoll(optarg);
            break;
        case 'b':
            base32_key = 1;
            break;
        default:
            print_usage();
            exit(1);
        }
    }
    if (sfound == 0 || optind >= argc) {
        print_usage();
        exit(1);
    }

    if (base32_key) {
        if (base32_decode(( char ** )&key, &key_len, secret, len) == -1) {
            free(key);
            return -1;
        }
    } else {
        key     = ( uint8_t * )secret;
        key_len = len;
    }

    draw_ui();

    if (strncmp("generate", argv[optind], 8) == 0) {
        uint32_t otp;
        otp = totp(key, len, calculate_time(get_time(), offset), 6);

        printf("OTP: \t%06d\n", otp);
        if (base32_key)
            free(key);

        exit(0);
    }

    if (base32_key)
        free(key);
    fprintf(stderr, "Unknown command\n");
    print_usage();
    exit(1);

    exit(0);
}

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
