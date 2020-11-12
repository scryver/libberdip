#define ENUM_START(name)              global String g##name[] =
#define ENUM_ITEM(name, desc, ...)    [name] = static_string(desc)

