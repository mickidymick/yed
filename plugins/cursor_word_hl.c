#include <yed/plugin.h>

/* This plugin has a @leak on reload. */

void cursor_word_hl_line_handler(yed_event *event);
void cursor_word_hl_cursor_moved_handler(yed_event *event);
void cursor_word_hl_hl_word(yed_event *event);

static char *the_word;
static int   the_word_len;

int yed_plugin_boot(yed_plugin *self) {
    yed_event_handler line, cursor_moved;

    line.kind          = EVENT_LINE_PRE_DRAW;
    line.fn            = cursor_word_hl_line_handler;
    cursor_moved.kind  = EVENT_CURSOR_MOVED;
    cursor_moved.fn    = cursor_word_hl_cursor_moved_handler;

    yed_plugin_add_event_handler(self, line);
    yed_plugin_add_event_handler(self, cursor_moved);

    return 0;
}

void cursor_word_hl_line_handler(yed_event *event) {
    yed_frame *frame;

    frame = event->frame;

    if (!frame
    ||   frame != ys->active_frame
    ||  !frame->buffer
    ||  frame->buffer->has_selection
    ||  frame->buffer->kind != BUFF_KIND_FILE) {
        return;
    }

    if (the_word) {
        cursor_word_hl_hl_word(event);
    }
}

void cursor_word_hl_cursor_moved_handler(yed_event *event) {
    yed_frame *frame;
    char      *word;

    frame = event->frame;

    if (!frame
    ||  !frame->buffer
    ||  frame->buffer->kind != BUFF_KIND_FILE) {
        return;
    }

    word = yed_word_under_cursor();

    if (!word) {
        if (the_word) {
            free(the_word);
            the_word     = NULL;
            frame->dirty = 1;
        }
        return;
    }

    if (the_word) {
        if (strcmp(the_word, word) == 0) {
            free(word);
            return;
        }
        free(the_word);
    }

    the_word     = word;
    the_word_len = strlen(the_word);
    frame->dirty = 1;
}

void cursor_word_hl_hl_word(yed_event *event) {
    yed_line  *line;
    yed_attrs *attr;
    yed_attrs  asc;
    int        i, is_alnum, is_active_row,
               col, old_col, spaces, word_len;
    char       c, *word;

    line = yed_buff_get_line(event->frame->buffer, event->row);
    asc  = yed_active_style_get_associate();
    col  = 1;

    is_active_row = event->frame->cursor_line == event->row;

    while (col <= line->visual_width) {
        is_alnum = 0;
        old_col  = col;
        word     = array_data(line->chars) + col - 1;
        word_len = 0;
        spaces   = 0;
        c        = yed_line_col_to_char(line, col);

        if (isalnum(c) || c == '_') {
            if (isalpha(c)) {
                is_alnum = 1;
            }
            while (col <= line->visual_width) {
                word_len += 1;
                col      += 1;
                c         = yed_line_col_to_char(line, col);

                if (isalpha(c)) {
                    is_alnum = 1;
                }

                if (!isalnum(c) && c != '_') {
                    break;
                }
            }
        } else {
            while (col <= line->visual_width) {
                word_len += 1;
                col      += 1;
                c         = yed_line_col_to_char(line, col);

                if (isalnum(c) || c == '_' || isspace(c)) {
                    break;
                }
            }
        }

        if (isspace(c)) {
            while (col <= line->visual_width) {
                spaces += 1;
                col    += 1;
                c       = yed_line_col_to_char(line, col);

                if (!isspace(c)) {
                    break;
                }
            }
        }

        if (is_alnum) {
            if (the_word_len == word_len) {
                if (is_active_row
                &&  event->frame->cursor_col >= old_col
                &&  event->frame->cursor_col < col) {
                    /* Don't highlight the one actually under the cursor. */
                } else if (strncmp(word, the_word, word_len) == 0) {
                    for (i = 0; i < word_len; i += 1) {
                        attr = array_item(event->line_attrs, old_col + i - 1);
                        yed_combine_attrs(attr, &asc);
                    }
                }
            }
        }
    }
}
