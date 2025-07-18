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
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <ncurses.h>
#include <form.h>

// #include <dialog.h>
//
// #define MAX_INPUT_LENGTH 256
// #define MAX_NOTES_LENGTH 8
//
// static const char *form_display_fmt_str =
//         "Descriptor:  %s\n"
//         "Username:    %s\n"
//         "Email:       %s\n"
//         "Password:    %s\n"
//         "OTP Code:    %s\n" // TODO: Replace secret with code
//         "Extra Notes: \n%s";
// #define FORM_DATA_STR(form_ptr, str) \
//     uint16_t str_len = \
//             (14 + (6 * MAX_INPUT_LENGTH)) + (8 * MAX_INPUT_LENGTH); \
//     str = calloc(str_len, sizeof(char)); \
//     snprintf(str, str_len, form_display_fmt_str, \
//              ( struct form_data_t * )form_ptr->descriptor, \
//              ( struct form_data_t * )form_ptr->username, \
//              ( struct form_data_t * )form_ptr->email, \
//              ( struct form_data_t * )form_ptr->password, \
//              ( struct form_data_t * )form_ptr->otp_secret, \ ( struct
//              form_data_t * )form_ptr->notes);
//
// struct form_data_t {
//     char    descriptor[MAX_INPUT_LENGTH];
//     char    username[MAX_INPUT_LENGTH];
//     char    email[MAX_INPUT_LENGTH];
//     char    password[MAX_INPUT_LENGTH];
//     char    otp_secret[MAX_INPUT_LENGTH];
//     uint8_t note_count;
//     char   *notes[MAX_INPUT_LENGTH];
// };
//
// void display_results(struct form_data_t *data)
// {
//     char *result_text = NULL;
//     FORM_DATA_STR(data, result_text);
//     dialog_msgbox("Entry", result_text, 20, 70, 1);
// }
//
// /* Function to get single line input */
// int get_single_line_input(const char *title, const char *prompt, char
// *buffer,
//                           size_t buffer_size)
// {
//     return dialog_inputbox(title, prompt, 10, 60, buffer, 0);
// }
//
// int create_temp_file_for_notes(char *filename, const char *initial_content)
// {
//     FILE *fp;
//     strcpy(filename, "/tmp/dialog_notes_XXXXXX");
//     int fd = mkstemp(filename);
//     if (fd == -1) {
//         return -1;
//     }
//     fp = fdopen(fd, "w");
//     if (fp == NULL) {
//         close(fd);
//         return -1;
//     }
//     if (initial_content)
//         fprintf(fp, "%s", initial_content);
//     else
//         fprintf(fp, "Enter extra notes, details and information here.\n");
//     fclose(fp);
//     return 0;
// }
//
// int get_multiline_input(const char *title, const char *prompt, char
// *tmp_file)
// {
//     return dialog_textbox(title, tmp_file, 15, 70);
// }
//
// /* Function to read notes from temporary file */
// int read_notes_from_file(const char *filename, struct form_data_t *data)
// {
//     FILE *fp     = NULL;
//     char *buffer = NULL;
//     char  ch     = '\0';
//     int   len    = 0;
//     int   num    = 0;
//
//     fp           = fopen(filename, "r");
//     if (fp == NULL) {
//         return -1;
//     }
//     buffer = calloc(MAX_INPUT_LENGTH * MAX_NOTES_LENGTH, sizeof(char));
//     if (buffer == NULL) {
//         fclose(fp);
//         return -1;
//     }
//
//     ch = getc(fp);
//     while ((ch != '\n') && (ch != EOF)) {
//         if (num == MAX_NOTES_LENGTH)
//             break;
//         if (ch == '\n' || len == MAX_INPUT_LENGTH) {
//             buffer[len]      = '\n';
//             data->notes[num] = calloc(MAX_INPUT_LENGTH, sizeof(char));
//             strcpy(data->notes[num], buffer);
//             memset(buffer, '\0', len * sizeof(char));
//             ++num;
//             len = 0;
//         } else {
//             buffer[len] = ch;
//             ++len;
//         }
//         ch = getc(fp);
//     }
//     assert(num <= MAX_NOTES_LENGTH);
//     data->note_count = num;
//
//     free(buffer);
//     fclose(fp);
//     unlink(filename);
//     return 0;
// }
//
// int run_user_form(struct form_data_t *data)
// {
//     int  result;
//     char temp_filename[256];
//
//     init_dialog(stdin, stdout);
//     dialog_vars.backtitle        = "User Input Form";
//     dialog_vars.cr_wrap          = 1;
//     dialog_vars.dlg_clear_screen = 1;
//
//     result =
//             get_single_line_input("Descriptor", "Entry description",
//                                   data->descriptor,
//                                   strlen(data->descriptor));
//     if (result != DLG_EXIT_OK)
//         goto cleanup;
//
//     result = get_single_line_input("Username", "Entry username",
//                                    data->username, strlen(data->username));
//     if (result != DLG_EXIT_OK)
//         goto cleanup;
//
//     result = get_single_line_input("Email", "Entry email address",
//                                    data->email, strlen(data->email));
//     if (result != DLG_EXIT_OK)
//         goto cleanup;
//
//     result = get_single_line_input("Password", "Entry password",
//                                    data->password, 0);
//     if (result != DLG_EXIT_OK)
//         goto cleanup;
//
//     result =
//             get_single_line_input("2FA", "Entry OTP secret key",
//                                   data->otp_secret,
//                                   strlen(data->otp_secret));
//     if (result != DLG_EXIT_OK)
//         goto cleanup;
//
//     if (create_temp_file_for_notes(temp_filename, "") == 0) {
//         result = dialog_textbox("Notes", temp_filename, 20, 80);
//         if (result == DLG_EXIT_OK) {
//             read_notes_from_file(temp_filename, data);
//         } else {
//             goto cleanup;
//         }
//     } else {
//         dialog_msgbox("Error", "Could not create temporary file for notes",
//         8,
//                       50, 1);
//         goto cleanup;
//     }
//
//     /* Confirm submission */
//     result = dialog_yesno("Confirm", "Submit this form data?", 8, 40);
//     if (result == DLG_EXIT_OK) {
//         display_results(data);
//     }
//
// cleanup:
//     end_dialog();
//     return result;
// }

static WINDOW *mainwin;
static WINDOW *childwin;

int draw_ui(void)
{
    // struct winsize w;
    // int            width  = 64;
    // int            height = 16;
    // int            cols   = 80;
    // int            rows   = 25;
    // int            x      = (cols - width) / 2;
    // int            y      = (rows - height) / 2;
    // char           c;
    //
    // if ((mainwin = initscr()) == NULL) {
    //     fprintf(stderr, "error: unable to initialise ncurses screen\n");
    //     return -1;
    // }
    //
    // noecho();
    // keypad(mainwin, 1);
    //
    // box(mainwin, 0, 0);
    //
    // childwin = subwin(mainwin, height, width, y, x);
    // box(childwin, 0, 0);
    // mvwaddstr(childwin, 5, 3, "Press 'q' to quit");
    //
    // refresh();
    //
    // while ((c = getch()) != 'q') {
    //     continue;
    // }
    //
    // delwin(childwin);
    // delwin(mainwin);
    // endwin();
    // refresh();
    //
    // return 0;

    FIELD *field[3];
    FORM  *frm;
    int    ch;

    if ((mainwin = initscr()) == NULL) {
        fprintf(stderr, "error: unable to initialise ncurses screen\n");
        return -1;
    }

    noecho();
    cbreak();
    keypad(mainwin, 1);

    box(mainwin, 0, 0);

    field[0] = new_field(1, 32, 4, 36, 0, 0);
    field[1] = new_field(1, 13, 6, 36, 0, 0);
    field[2] = NULL;

    set_field_back(field[0], A_UNDERLINE);
    field_opts_off(field[0], O_AUTOSKIP);
    set_field_back(field[1], A_UNDERLINE);
    field_opts_off(field[1], O_AUTOSKIP);

    frm = new_form(field);
    post_form(frm);

    mvprintw(4, 13, "TOTP Secret: ");
    mvprintw(6, 13, "Code:        ");

    int mode = 0;
    while (1) {
        ch = getch();
        if (mode == 0) {
            switch (ch) {
            case 'j':
            case KEY_DOWN:
                form_driver(frm, REQ_NEXT_FIELD);
                form_driver(frm, REQ_END_LINE);
                break;
            case 'k':
            case KEY_UP:
                form_driver(frm, REQ_PREV_FIELD);
                form_driver(frm, REQ_END_LINE);
                break;
            case 'D':
                form_driver(frm, REQ_CLR_FIELD);
                break;
            case 'q':
                goto cleanup;
                break;
            case 'i':
                mode = 1;
                break;
            default:
                break;
            }
        }
        if (mode == 1) {
            switch (ch) {
            case KEY_ENTER:
            case KEY_DOWN:
                form_driver(frm, REQ_NEXT_FIELD);
                form_driver(frm, REQ_END_LINE);
                break;
            case KEY_UP:
                form_driver(frm, REQ_PREV_FIELD);
                form_driver(frm, REQ_END_LINE);
                break;
            case KEY_BACKSPACE:
                form_driver(frm, REQ_DEL_PREV);
                break;
            case 27: /* Escape */
                mode = 0;
                break;
            default:
                form_driver(frm, ch);
                break;
            }
        }
    }

cleanup:
    unpost_form(frm);
    free_form(frm);
    free_field(field[0]);
    free_field(field[1]);

    endwin();
    return 0;
}

// vim: ft=c ts=4 sts=4 sw=4 et ai cin
