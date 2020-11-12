#define ENUM_ITEM_VALUE(name, desc, value)   name = value
#define ENUM_ITEM_DEFAULT(name, desc)        name
#define ENUM_ITEM_GET_NTH_ARG(_1, _2, N, ...) N

#define ENUM_START(name)            enum name
#define ENUM_ITEM(name, desc, ...)  \
ENUM_ITEM_GET_NTH_ARG("dummy", ## __VA_ARGS__, \
ENUM_ITEM_VALUE, ENUM_ITEM_DEFAULT)(name, desc, ## __VA_ARGS__)
