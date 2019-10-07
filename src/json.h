
#ifndef JSON_FULL_PARSING
#define JSON_FULL_PARSING 0
#endif // JSON_FULL_PARSING

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

struct JsonArrayIter
{
    String string;
};

//
// NOTE(michiel): The find interface expects an json object as input string and will try to match the key or value.
//

// NOTE(michiel): Find first occurance of a json 'kind' value and return the key and the starting point of the value.
internal b32 json_find_first(String json, JsonValueKind kind, String *foundKey, String *subString);
// NOTE(michiel): Find 'key' in object and match it to the expect value kind. 'subString' points to the
// start of the value.
internal b32 json_find(String json, String key, JsonValueKind kind, String *subString);

// NOTE(michiel): Loop interface for arrays
internal JsonArrayIter json_iterate_array(String input);
internal b32 is_valid(JsonArrayIter *iter);
internal void json_next_array(JsonArrayIter *iter);
// for (JsonArrayIter iter = json_iterate_array(json);
//      is_valid(&iter);
//      json_next_array(&iter))
// {
//     iter.string points to the value in the array
// }

// NOTE(michiel): Drill down interface
internal b32 json_is_constant(String input);
internal b32 json_is_number(String input);
internal b32 json_is_string(String input);
internal b32 json_is_array(String input);
internal b32 json_is_object(String input);
internal String json_skip_constant(String input);
internal String json_skip_string(String input);
internal String json_skip_number(String input);
internal String json_skip_array(String input);
internal String json_skip_object(String input);
internal String json_skip_value(String input);
internal b32 json_parse_bool(String input);
internal s64 json_parse_integer(String input);
internal String json_parse_string(String input);

//
// NOTE(michiel): Full parser
//

#if JSON_FULL_PARSING
struct JsonValue;
struct JsonItem;

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

#endif

