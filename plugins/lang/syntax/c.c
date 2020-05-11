#include <yed/plugin.h>
#include <yed/highlight.h>

#define ARRAY_LOOP(a) for (__typeof((a)[0]) *it = (a); it < (a) + (sizeof(a) / sizeof((a)[0])); ++it)

highlight_info hinfo;

void unload(yed_plugin *self);
void syntax_c_line_handler(yed_event *event);
void syntax_c_frame_handler(yed_event *event);
void syntax_c_buff_mod_pre_handler(yed_event *event);
void syntax_c_buff_mod_post_handler(yed_event *event);


int yed_plugin_boot(yed_plugin *self) {
    yed_event_handler frame, line, buff_mod_pre, buff_mod_post;
    char              *kwds[] = {
        "__asm__",  "asm",
        "bool",     "break",
        "case",     "char",     "const",  "continue",
        "default",  "do",       "double",
        "else",     "enum",     "extern",
        "float",    "for",
        "goto",
        "if",       "inline",   "int",
        "long",
        "restrict", "return",
        "short",    "size_t",   "sizeof", "ssize_t",  "static", "struct", "switch",
        "typedef",
        "union",    "unsigned",
        "void",     "volatile",
        "while",
    };
    char              *pp_kwds[] = {
        "define",
        "else", "endif", "error",
        "if", "ifdef", "ifndef", "include",
        "message",
        "pragma",
        "undef",
        "warning",
    };

    yed_plugin_set_unload_fn(self, unload);


    frame.kind          = EVENT_FRAME_PRE_BUFF_DRAW;
    frame.fn            = syntax_c_frame_handler;
    line.kind           = EVENT_LINE_PRE_DRAW;
    line.fn             = syntax_c_line_handler;
    buff_mod_pre.kind   = EVENT_BUFFER_PRE_MOD;
    buff_mod_pre.fn     = syntax_c_buff_mod_pre_handler;
    buff_mod_post.kind  = EVENT_BUFFER_POST_MOD;
    buff_mod_post.fn    = syntax_c_buff_mod_post_handler;

    yed_plugin_add_event_handler(self, frame);
    yed_plugin_add_event_handler(self, line);
    yed_plugin_add_event_handler(self, buff_mod_pre);
    yed_plugin_add_event_handler(self, buff_mod_post);


    highlight_info_make(&hinfo);

    ARRAY_LOOP(kwds)
        highlight_add_kwd(&hinfo, *it, HL_KEY);
    ARRAY_LOOP(pp_kwds)
        highlight_add_prefixed_kwd(&hinfo, '#', *it, HL_PP);
    highlight_add_kwd(&hinfo, "__VA_ARGS__", HL_PP);
    highlight_add_kwd(&hinfo, "__FILE__", HL_PP);
    highlight_add_kwd(&hinfo, "__func__", HL_PP);
    highlight_add_kwd(&hinfo, "__FUNCTION__", HL_PP);
    highlight_add_kwd(&hinfo, "__LINE__", HL_PP);
    highlight_add_kwd(&hinfo, "__DATE__", HL_PP);
    highlight_add_kwd(&hinfo, "__TIME__", HL_PP);
    highlight_add_kwd(&hinfo, "__STDC__", HL_PP);
    highlight_add_kwd(&hinfo, "__STDC_VERSION__", HL_PP);
    highlight_add_kwd(&hinfo, "__STDC_HOSTED__", HL_PP);
    highlight_add_kwd(&hinfo, "__cplusplus", HL_PP);
    highlight_add_kwd(&hinfo, "__OBJC__", HL_PP);
    highlight_add_kwd(&hinfo, "__ASSEMBLER__", HL_PP);
    highlight_add_kwd(&hinfo, "NULL", HL_CON);
    highlight_add_kwd(&hinfo, "stdin", HL_CON);
    highlight_add_kwd(&hinfo, "stdout", HL_CON);
    highlight_add_kwd(&hinfo, "stderr", HL_CON);
    highlight_suffixed_words(&hinfo, '(', HL_CALL);
    highlight_numbers(&hinfo);
    highlight_within(&hinfo, "\"", "\"", '\\', -1, HL_STR);
    highlight_within(&hinfo, "'", "'", '\\', 1, HL_CHAR);
    highlight_to_eol_from(&hinfo, "//", HL_COMMENT);
    highlight_within_multiline(&hinfo, "/*", "*/", 0, HL_COMMENT);
    highlight_within_multiline(&hinfo, "#if 0", "#endif", 0, HL_COMMENT);

    ys->redraw = 1;

    return 0;
}

void unload(yed_plugin *self) {
    highlight_info_free(&hinfo);
    ys->redraw = 1;
}

void syntax_c_frame_handler(yed_event *event) {
    yed_frame *frame;

    frame = event->frame;

    if (!frame
    ||  !frame->buffer
    ||  frame->buffer->kind != BUFF_KIND_FILE
    || (frame->buffer->file.ft != FT_C
     && frame->buffer->file.ft != FT_CXX)) {
        return;
    }

    highlight_frame_pre_draw_update(&hinfo, event);
}

void syntax_c_line_handler(yed_event *event) {
    yed_frame *frame;

    frame = event->frame;

    if (!frame
    ||  !frame->buffer
    ||  frame->buffer->kind != BUFF_KIND_FILE
    || (frame->buffer->file.ft != FT_C
     && frame->buffer->file.ft != FT_CXX)) {
        return;
    }

    highlight_line(&hinfo, event);
}

void syntax_c_buff_mod_pre_handler(yed_event *event) {
    yed_frame *frame;

    frame = event->frame;

    if (!frame
    ||  !frame->buffer
    ||  frame->buffer->kind != BUFF_KIND_FILE
    || (frame->buffer->file.ft != FT_C
     && frame->buffer->file.ft != FT_CXX)) {
        return;
    }

    highlight_buffer_pre_mod_update(&hinfo, event);
}

void syntax_c_buff_mod_post_handler(yed_event *event) {
    yed_frame *frame;

    frame = event->frame;

    if (!frame
    ||  !frame->buffer
    ||  frame->buffer->kind != BUFF_KIND_FILE
    || (frame->buffer->file.ft != FT_C
     && frame->buffer->file.ft != FT_CXX)) {
        return;
    }

    highlight_buffer_post_mod_update(&hinfo, event);
}
