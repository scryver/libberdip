global char *gSpaces = "                                                                                                                                                                                                                                                                ";

#ifndef PRINT_INDENT_COUNT
#define PRINT_INDENT_COUNT 4
#endif

#define print_indent(s)       print_indent_api(gFileApi, s)
#define print(s, f, ...)      print_api(gFileApi, s, f, ## __VA_ARGS__)
#define println(s, f, ...)    println_api(gFileApi, s, f, ## __VA_ARGS__)
#define println_begin(s, ...) println_begin_api(gFileApi, s, ## __VA_ARGS__)
#define println_end(s, ...)   println_end_api(gFileApi, s, ## __VA_ARGS__)

#define vprint(s, f, a)         vprint_api(gFileApi, s, f, a)
#define vprintln(s, f, a)       vprintln_api(gFileApi, s, f, a)
#define vprintln_begin(s, f, a) vprintln_begin_api(gFileApi, s, f, a)
#define vprintln_end(s, f, a)   vprintln_end_api(gFileApi, s, f, a)

internal void
print_indent_api(FileAPI *api, FileStream *stream)
{
    i_expect((PRINT_INDENT_COUNT * stream->indent) < 256);
    api->write_to_file(&stream->file, PRINT_INDENT_COUNT * stream->indent, gSpaces);
}

internal void
vprint_api(FileAPI *api, FileStream *stream, char *fmt, va_list args)
{
    api->write_vfmt_to_file(&stream->file, fmt, args);
}

internal void
vprintln_api(FileAPI *api, FileStream *stream, char *fmt, va_list args)
{
    print_indent_api(api, stream);
    api->write_vfmt_to_file(&stream->file, fmt, args);
    api->write_fmt_to_file(&stream->file, "\n");
}

internal void
vprintln_begin_api(FileAPI *api, FileStream *stream, char *fmt, va_list args)
{
    print_indent_api(api, stream);
    api->write_vfmt_to_file(&stream->file, fmt, args);
}

internal void
vprintln_end_api(FileAPI *api, FileStream *stream, char *fmt, va_list args)
{
    api->write_vfmt_to_file(&stream->file, fmt, args);
    api->write_fmt_to_file(&stream->file, "\n");
}

internal void
print_api(FileAPI *api, FileStream *stream, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint_api(api, stream, fmt, args);
    va_end(args);
}

internal void
println_api(FileAPI *api, FileStream *stream, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintln_api(api, stream, fmt, args);
    va_end(args);
}

internal void
println_begin_api(FileAPI *api, FileStream *stream, char *fmt = 0, ...)
{
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        vprintln_begin_api(api, stream, fmt, args);
        va_end(args);
    }
    else
    {
        print_indent_api(api, stream);
    }
}

internal void
println_end_api(FileAPI *api, FileStream *stream, char *fmt = 0, ...)
{
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        vprintln_end_api(api, stream, fmt, args);
        va_end(args);
    }
    else
    {
        api->write_fmt_to_file(&stream->file, "\n");
    }
}
