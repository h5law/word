/* ui.c
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
#include <locale.h>

#include <stdio.h>
#include <termbox2.h>

#include "db.h"
#include "ui.h"

void print_tb(const char *str, int x, int y, uint16_t fg, uint16_t bg)
{
    while (*str) {
        uint32_t uni;
        str += tb_utf8_char_to_unicode(&uni, str);
        tb_set_cell(x, y, uni, fg, bg);
        x++;
    }
}

void printf_tb(int x, int y, uint16_t fg, uint16_t bg, const char *fmt, ...)
{
    char    buf[4096];
    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buf, sizeof(buf), fmt, vl);
    va_end(vl);
    print_tb(buf, x, y, fg, bg);
}

void pretty_print_resize(struct tb_event *ev)
{
    printf_tb(3, tb_height() - 1, TB_WHITE, TB_DEFAULT,
              "Resize event: w=%d h=%d", ev->w, ev->h);
}

enum {
    BORDER_NORMAL = 0,
    BORDER_CORNER_LEFT,
    BORDER_CORNER_RIGHT,
    BORDER_CORNER_LEFT_BOTTOM,
    BORDER_CORNER_RIGHT_BOTTOM,
    BORDER_VERTICAL,
    BORDER_MAX
};

static const char *const borders[BORDER_MAX] = {
        [BORDER_NORMAL]              = "─",
        [BORDER_CORNER_LEFT]         = "┌",
        [BORDER_CORNER_RIGHT]        = "┐",
        [BORDER_CORNER_LEFT_BOTTOM]  = "└",
        [BORDER_CORNER_RIGHT_BOTTOM] = "┘",
        [BORDER_VERTICAL]            = "│",
};

void draw_box(int x1, int y1, int width, int height, uintattr_t fg,
              uintattr_t bg)
{
    int x2 = x1 + width;
    int y2 = y1 + height;

    for (int x = x1; x < (x2 - 1); ++x) {
        printf_tb(x, y1, fg, bg, borders[BORDER_NORMAL]);
        printf_tb(x, y2 - 1, fg, bg, borders[BORDER_NORMAL]);
    }

    for (int y = y1 + 1; y < y2 - 1; ++y) {
        printf_tb(x1, y, fg, bg, borders[BORDER_VERTICAL]);
        printf_tb(x2 - 1, y, fg, bg, borders[BORDER_VERTICAL]);
    }

    printf_tb(x1, y1, fg, bg, borders[BORDER_CORNER_LEFT]);
    printf_tb(x2 - 1, y1, fg, bg, borders[BORDER_CORNER_RIGHT]);
    printf_tb(x1, y2 - 1, fg, bg, borders[BORDER_CORNER_LEFT_BOTTOM]);
    printf_tb(x2 - 1, y2 - 1, fg, bg, borders[BORDER_CORNER_RIGHT_BOTTOM]);
}

#define LIST_MAX 256

typedef struct {
    char          *items[LIST_MAX];
    table_entry_t *entries[LIST_MAX];
    int            count;
    int            selected;
    int            searching;
    int            scroll;
} List;

void list_init(List *list)
{
    list->count     = 0;
    list->selected  = 0;
    list->searching = 0;
    list->scroll    = 0;
    for (int i = 0; i < LIST_MAX; i++) {
        list->items[i]   = NULL;
        list->entries[i] = NULL;
    }
}

void list_add(List *list, const char *item, table_entry_t *entry)
{
    if (list->count < LIST_MAX) {
        list->items[list->count]   = strdup(item);
        list->entries[list->count] = entry;
        list->count++;
    }
}

void list_free(List *list)
{
    for (int i = 0; i < list->count; i++) {
        free(list->items[i]);
        free(list->entries[i]);
    }
}

void list_draw(List *list, int x, int y, int width, int height)
{
#define LIST_VISIBLE (tb_height() - 6)
    int visible_items = height - 2 < LIST_VISIBLE ? height - 2 : LIST_VISIBLE;
    for (int i = 0; i < visible_items && (i + list->scroll) < list->count;
         i++) {
        int      idx = i + list->scroll;
        uint16_t fg  = (idx == list->selected) ? TB_BLACK : TB_WHITE;
        uint16_t bg  = (idx == list->selected) ? TB_WHITE : TB_BLACK;
        for (int j = x + 1; j < x + width - 1; j++) {
            tb_set_cell(j, y + 2 + i, ' ', fg, bg);
        }
        int item_len = strlen(list->items[idx]);
        int start_x  = x + (width - item_len) / 2;
        if (start_x < x + 1)
            start_x = x + 1; // Ensure within bounds
        printf_tb(start_x, y + 2 + i, fg, bg, "%s", list->items[idx]);
    }
}

void draw_form(table_entry_t *entry, int x, int y, int width, int height)
{
    if (!entry)
        return;
    int line = y + 2;
    printf_tb(x + 2, line++, TB_WHITE, TB_BLACK, "Descriptor: %s",
              entry->descriptor);
    printf_tb(x + 2, line++, TB_WHITE, TB_BLACK, "Username:   %s",
              entry->username);
    printf_tb(x + 2, line++, TB_WHITE, TB_BLACK, "Password:   %s",
              entry->password);
    printf_tb(x + 2, line++, TB_WHITE, TB_BLACK, "OTP Secret: %s",
              entry->opt_secret);
    printf_tb(x + 2, line++, TB_WHITE, TB_BLACK, "OTP Base32: %s",
              entry->otp_base32 ? "true" : "false");
    printf_tb(x + 2, line++, TB_WHITE, TB_BLACK, "Notes: %s",
              entry->notes[0]);
    for (size_t i = 1; i < entry->ncount && i < 5; i++) {
        printf_tb(x + 9, line++, TB_WHITE, TB_BLACK, "%s", entry->notes[i]);
    }
}

void list_handle_input(List *list, struct tb_event *ev, int height)
{
#define LIST_VISIBLE (tb_height() - 6)
    int visible_items = height - 2 < LIST_VISIBLE ? height - 2 : LIST_VISIBLE;
    if (ev->type == TB_EVENT_KEY) {
        if (ev->key == TB_KEY_ARROW_UP && list->selected > 0) {
            list->selected--;
            if (list->selected < list->scroll)
                list->scroll--;
        } else if (ev->key == TB_KEY_ARROW_DOWN &&
                   list->selected < list->count - 1) {
            list->selected++;
            if (list->selected >= list->scroll + visible_items)
                list->scroll++;
        } else if (ev->ch == 47 && !list->searching) {
            list->searching = 1;
        } else if ((ev->key == TB_KEY_ENTER || ev->key == TB_KEY_ESC) &&
                   list->searching) {
            list->searching = 0;
        }
    }
}

int draw_ui()
{
    int ret = 0;

    if ((tb_init()) != TB_OK) {
        return EXIT_FAILURE;
    }
    tb_set_input_mode(TB_INPUT_ALT);
    setlocale(LC_ALL, ""); /* Fix unicode handling */

    tb_set_input_mode(TB_INPUT_ESC);
    struct tb_event ev;

    List list;
    list_init(&list);
    for (int i = 0; i < 40; i++) {
        char buf[32];
        snprintf(buf, 32, "Example entry %d", i);
        table_entry_t *entry = malloc(sizeof(table_entry_t));
        entry->id            = i + 1;
        snprintf(entry->descriptor, MAX_STR_LEN, "Desc %d", i + 1);
        entry->dlen = strlen(entry->descriptor);
        snprintf(entry->username, MAX_STR_LEN, "User %d", i + 1);
        entry->ulen = strlen(entry->username);
        snprintf(entry->password, MAX_STR_LEN, "Pass %d", i + 1);
        entry->plen = strlen(entry->password);
        snprintf(entry->opt_secret, MAX_STR_LEN, "Secret %d", i + 1);
        entry->olen       = strlen(entry->opt_secret);
        entry->otp_base32 = i % 2;
        entry->ncount     = 2;
        snprintf(entry->notes[0], MAX_STR_LEN, "%d", 0);
        for (int i = 1; i < entry->ncount; ++i)
            snprintf(entry->notes[i], MAX_STR_LEN, "%d", i);
        list_add(&list, buf, entry);
    }

    tb_clear();
    int half_width = tb_width() / 2;
    printf_tb(3, 2, TB_WHITE, TB_BLACK, "Search...");
    printf_tb(tb_width() / 2 - 1 - 3, 2, TB_WHITE, TB_BLACK, "(/)");
    draw_box(1, 1, half_width, tb_height() - 2, TB_WHITE, TB_BLACK);
    draw_box(half_width + 1, 1, tb_width() - half_width - 1, tb_height() - 2,
             TB_WHITE, TB_BLACK);
    print_tb("Press <ctrl+q> to quit", tb_width() - 1 - 3 - 22,
             tb_height() - 1, TB_WHITE, TB_BLACK);
    list_draw(&list, 1, 1, half_width, tb_height() - 2);
    draw_form(list.entries[list.selected], half_width + 1, 1,
              tb_width() - half_width - 1, tb_height() - 2);
    tb_present();

    while (1) {
        ret = tb_poll_event(&ev);

        if (ret != TB_OK) {
            if (ret == TB_ERR_POLL && tb_last_errno() == EINTR) {
                continue;
            }
            break;
        }

        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.key == TB_KEY_CTRL_Q) {
                list_free(&list);
                tb_shutdown();
                return 0;
            }
            list_handle_input(&list, &ev, tb_height());
            tb_clear();
            draw_box(1, 1, half_width, tb_height() - 2, TB_WHITE, TB_BLACK);
            draw_box(half_width + 1, 1, tb_width() - half_width - 1,
                     tb_height() - 2, TB_WHITE, TB_BLACK);
            print_tb("Press <ctrl+q> to quit", tb_width() - 1 - 3 - 22,
                     tb_height() - 1, TB_WHITE, TB_BLACK);
            list_draw(&list, 1, 1, half_width, tb_height() - 2);
            draw_form(list.entries[list.selected], half_width + 1, 1,
                      tb_width() - half_width - 1, tb_height() - 2);
            if (list.searching) {
                int  x  = 3;
                char ch = 0;
                for (int i = 3; i < tb_width() / 2 - 1; ++i)
                    printf_tb(i, 2, TB_WHITE, TB_BLACK, " ");
                tb_present();
                ch = getchar();
                for (;;) {
                    if (ch == TB_KEY_ESC || ch == TB_KEY_ENTER) { // esc
                        list.searching = 0;
                        break;
                    }
                    if (ch == TB_KEY_BACKSPACE || ch == TB_KEY_BACKSPACE2) {
                        if (x == 3)
                            goto cont;
                        printf_tb(--x, 2, TB_WHITE, TB_BLACK, " ");
                    } else
                        printf_tb(x++, 2, TB_WHITE, TB_BLACK, "%c", ch);
                cont:
                    tb_present();
                    ch = getchar();
                }
            } else {
                printf_tb(3, 2, TB_WHITE, TB_BLACK, "Search...");
                printf_tb(tb_width() / 2 - 1 - 3, 2, TB_WHITE, TB_BLACK,
                          "(/)");
            }
            tb_present();
            break;
        case TB_EVENT_RESIZE:
            tb_clear();
            half_width = tb_width() / 2;
            draw_box(1, 1, half_width, tb_height() - 2, TB_WHITE, TB_BLACK);
            draw_box(half_width + 1, 1, tb_width() - half_width - 1,
                     tb_height() - 2, TB_WHITE, TB_BLACK);
            print_tb("Press <ctrl+q> to quit", tb_width() - 1 - 3 - 22,
                     tb_height() - 1, TB_WHITE, TB_BLACK);
            list_draw(&list, 1, 1, half_width, tb_height() - 2);
            draw_form(list.entries[list.selected], half_width + 1, 1,
                      tb_width() - half_width - 1, tb_height() - 2);
            if (list.searching) {
                int  x  = 3;
                char ch = 0;
                for (int i = 3; i < tb_width() / 2 - 1; ++i)
                    printf_tb(i, 2, TB_WHITE, TB_BLACK, " ");
                tb_present();
                ch = getchar();
                for (;;) {
                    if (ch == TB_KEY_ESC || ch == TB_KEY_ENTER) { // esc
                        list.searching = 0;
                        break;
                    }
                    if (ch == TB_KEY_BACKSPACE || ch == TB_KEY_BACKSPACE2) {
                        if (x == 3)
                            goto contr;
                        printf_tb(--x, 2, TB_WHITE, TB_BLACK, " ");
                    } else
                        printf_tb(x++, 2, TB_WHITE, TB_BLACK, "%c", ch);
                contr:
                    tb_present();
                    ch = getchar();
                }
            } else {
                printf_tb(3, 2, TB_WHITE, TB_BLACK, "Search...");
                printf_tb(tb_width() / 2 - 1 - 3, 2, TB_WHITE, TB_BLACK,
                          "(/)");
            }
            pretty_print_resize(&ev);
            tb_present();
            break;
        default:
            break;
        }
    }

    list_free(&list);
    tb_shutdown();
    return 0;
}

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
