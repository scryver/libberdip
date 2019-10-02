
struct JsonValue;
struct JsonItem;

enum JsonValueKind
{
    JsonValue_None,
    JsonValue_Object,
    JsonValue_Array,
    JsonValue_String,
    JsonValue_Number,
    JsonValue_Constant,
    //JsonValue_Error,
};

enum JsonConstants
{
    JsonConstant_None,
    JsonConstant_Null,
    JsonConstant_True,
    JsonConstant_False,
};

struct JsonObject
{
    u32 itemCount;
    JsonItem *items;
};

struct JsonArray
{
    u32 valueCount;
    JsonValue *values;
};

struct JsonValue
{
    JsonValueKind kind;

    union
    {
        JsonObject    object;
        JsonArray     array;
        String        string;
        String        number; // NOTE(michiel): We do not parse the number for you
        JsonConstants constant;
        //String        errorMessage;
    };
};

struct JsonItem
{
    String key;
    JsonValue value;
};

struct JsonParser
{
    Arena arena;     // NOTE(michiel): Storage for the parsed data structure

    b32 saveAll;     // NOTE(michiel): If true, all needed strings are copied out of the scanner.
    String scanner;
};

// NOTE(michiel): The parse uses scanData to build the structure. Be aware that you need to keep the scanData alive
// as long as you use the JsonValue. If a string is needed for example it will directly point into scanData, unless
// parser->saveAll is true. Returns true on success.
internal b32 json_parse_value(JsonParser *parser, JsonValue *value);
