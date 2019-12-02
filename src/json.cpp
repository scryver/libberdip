internal void
advance(String *s)
{
    --s->size;
    ++s->data;
}

internal void
strip_whitespace(String *s)
{
    while (s->size && is_whitespace(s->data[0]))
    {
        advance(s);
    }
}

internal b32
json_is_constant(String input)
{
    b32 result = false;
    String boolean = input;
    if (input.size > 3)
    {
        boolean.size = 4;
        if (boolean == string(4, "null"))
        {
            result = true;
        }
        else if (boolean == string(4, "true"))
        {
            result = true;
        }
        else if (input.size > 4)
        {
            boolean.size = 5;
            if (boolean == string(5, "false"))
            {
                result = true;
            }
        }
    }
    return result;
}

internal b32
json_is_number(String input)
{
    return (input.size && input.data && (is_digit(input.data[0]) || (input.data[0] == '-')));
}

internal b32
json_is_string(String input)
{
    return (input.size && input.data && (input.data[0] == '"'));
}

internal b32
json_is_array(String input)
{
    return (input.size && input.data && (input.data[0] == '['));
}

internal b32
json_is_object(String input)
{
    return (input.size && input.data && (input.data[0] == '{'));
}

internal String
json_skip_constant(String input)
{
    String result = input;
    if (input.size > 3)
    {
        if ((input.data[0] == 'n') &&
            (input.data[1] == 'u') &&
            (input.data[2] == 'l') &&
            (input.data[3] == 'l'))
        {
            result.size -= 4;
            result.data += 4;
        }
        else if ((input.data[0] == 't') &&
                 (input.data[1] == 'r') &&
                 (input.data[2] == 'u') &&
                 (input.data[3] == 'e'))
        {
            result.size -= 4;
            result.data += 4;
        }
        else if ((input.size > 4) &&
                 (input.data[0] == 'f') &&
                 (input.data[1] == 'a') &&
                 (input.data[2] == 'l') &&
                 (input.data[3] == 's') &&
                 (input.data[4] == 'e'))
        {
            result.size -= 5;
            result.data += 5;
        }
    }
    return result;
}

internal String
json_skip_number(String input)
{
    String result = input;

    if (result.size && (result.data[0] == '-'))
    {
        advance(&result);
    }

    while (result.size && is_digit(result.data[0]))
    {
        advance(&result);
    }

    if (result.size && (result.data[0] == '.'))
    {
        advance(&result);

        while (result.size && is_digit(result.data[0]))
        {
            advance(&result);
        }
    }

    if (result.size && ((result.data[0] == 'e') ||
                        (result.data[0] == 'E')))
    {
        advance(&result);
        if (result.size && ((result.data[0] == '+') ||
                            (result.data[0] == '-')))
        {
            advance(&result);
        }

        while (result.size && is_digit(result.data[0]))
        {
            advance(&result);
        }
    }

    return result;
}

internal String
json_skip_string(String input)
{
    String result = input;
    if (result.size && (result.data[0] == '"'))
    {
        advance(&result);
        while (result.size && (result.data[0] != '"'))
        {
            if (result.data[0] == '\\')
            {
                advance(&result);
                if (result.size == 0)
                {
                    break;
                }
            }
            advance(&result);
        }
        if (result.size && (result.data[0] == '"'))
        {
            advance(&result);
        }
    }
    return result;
}

internal String
json_skip_array(String input)
{
    String result = input;

    if (json_is_array(result))
    {
        advance(&result);
        strip_whitespace(&result);
        while (result.size)
        {
            if (result.data[0] == ']')
            {
                advance(&result);
                break;
            }
            result = json_skip_value(result);
            strip_whitespace(&result);

            if (result.size && (result.data[0] == ','))
            {
                advance(&result);
                strip_whitespace(&result);
            }
        }
    }

    return result;
}

internal String
json_skip_object(String input)
{
    String result = input;

    if (json_is_object(result))
    {
        advance(&result);
        strip_whitespace(&result);
        while (result.size)
        {
            if (result.data[0] == '}')
            {
                advance(&result);
                break;
            }
            result = json_skip_string(result);
            strip_whitespace(&result);
            if (result.size && (result.data[0] == ':'))
            {
                advance(&result);
                strip_whitespace(&result);
                result = json_skip_value(result);
                strip_whitespace(&result);
            }
            else
            {
                // TODO(michiel): Errors
                result.size = 0;
                result.data = 0;
            }

            if (result.size && (result.data[0] == ','))
            {
                advance(&result);
                strip_whitespace(&result);
            }
        }
    }

    return result;
}

internal String
json_skip_value(String input)
{
    String result = {};
    if (json_is_object(input))
    {
        result = json_skip_object(input);
    }
    else if (json_is_array(input))
    {
        result = json_skip_array(input);
    }
    else if (json_is_string(input))
    {
        result = json_skip_string(input);
    }
    else if (json_is_number(input))
    {
        result = json_skip_number(input);
    }
    else if (json_is_constant(input))
    {
        result = json_skip_constant(input);
    }
    else
    {
        // TODO(michiel): Errors
    }
    return result;
}

internal b32
json_parse_bool(String input)
{
    strip_whitespace(&input);

    b32 result = false;
    String boolean = input;
    if (input.size > 3)
    {
        boolean.size = 4;

        if (boolean == string(4, "true"))
        {
            result = true;
        }
    }
    return result;
}

internal s64
json_parse_integer(String input)
{
    strip_whitespace(&input);

    s64 result = 0;

    b32 negate = input.size && (input.data[0] == '-');
    if (negate)
    {
        advance(&input);
    }

    if (input.size && (input.data[0] != '0'))
    {
        while (input.size && is_digit(input.data[0]))
        {
            result = result * 10 + (input.data[0] & 0xF);
            advance(&input);
        }
    }

    return result;
}

internal String
json_parse_string(String input)
{
    // NOTE(michiel): This will _not_ unescape values or parse the utf8, this will return the json string as is.
    strip_whitespace(&input);

    String result = {};

    if (input.size && (input.data[0] == '"'))
    {
        advance(&input);
        result.data = input.data;
        while (input.size && (input.data[0] != '"'))
        {
            if (input.data[0] == '\\')
            {
                advance(&input);
                ++result.size;
                if (input.size &&
                    ((input.data[0] == '"') ||
                     (input.data[0] == '\\') ||
                     (input.data[0] == '/') ||
                     (input.data[0] == 'b') ||
                     (input.data[0] == 'f') ||
                     (input.data[0] == 'n') ||
                     (input.data[0] == 'r') ||
                     (input.data[0] == 't')))
                {
                    advance(&input);
                    ++result.size;
                }
                else if (input.size && (input.data[0] == 'u'))
                {
                    advance(&input);
                    ++result.size;
                    if (input.size >= 4)
                    {
                        for (u32 hexIndex = 0; hexIndex < 4; ++hexIndex)
                        {
                            if (is_hex_digit(input.data[0]))
                            {
                                advance(&input);
                                ++result.size;
                            }
                            else
                            {
                                // TODO(michiel): Errors
                                input.size = 0;
                                result.size = 0;
                                result.data = 0;
                                break;
                            }
                        }
                    }
                    else
                    {
                        // TODO(michiel): Errors
                        input.size = 0;
                        result.size = 0;
                        result.data = 0;
                    }
                }
                else
                {
                    // TODO(michiel): Errors
                    input.size = 0;
                    result.size = 0;
                    result.data = 0;
                }
            }
            else
            {
                advance(&input);
                ++result.size;
            }
        }
    }

    return result;
}

internal u32
codepoint_to_utf8(u32 codepoint, u8 *buffer)
{
    // NOTE(michiel): Returns number of bytes used in buffer, 4 bytes should be available, no more than
    // will ever be used.
    u32 result = 0;

    if (codepoint > 0x7F)
    {
        if (codepoint > 0x7FF)
        {
            if (codepoint > 0xFFFF)
            {
                buffer[result++] = 0xF0 | ((codepoint >> 18) & 0x07);
                buffer[result++] = 0x80 | ((codepoint >> 12) & 0x3F);
            }
            else
            {
                buffer[result++] = 0xE0 | ((codepoint >> 12) & 0x0F);
            }
            buffer[result++] = 0x80 | ((codepoint >>  6) & 0x3F);
        }
        else
        {
            buffer[result++] = 0xC0 | ((codepoint >>  6) & 0x1F);
        }
        buffer[result++] = 0x80 | (codepoint & 0x3F);
    }
    else
    {
        buffer[result++] = codepoint;
    }
    return result;
}

internal String
json_decode_string(String input, u8 *outData)
{
    // NOTE(michiel): This will _always_ return a string less or equal in length as the input.
    // It will transform the escaped values back to their original value and calculate the
    // unicode codepoints, if any.
    String result = {};
    result.data = outData;

    while (input.size)
    {
        if (input.data[0] == '\\')
        {
            advance(&input);
            if (input.size)
            {
                switch (input.data[0])
                {
                    case '"':  { advance(&input); result.data[result.size++] = '"'; } break;
                    case '\\': { advance(&input); result.data[result.size++] = '\\'; } break;
                    case '/':  { advance(&input); result.data[result.size++] = '/'; } break;
                    case 'b':  { advance(&input); result.data[result.size++] = '\b'; } break;
                    case 'f':  { advance(&input); result.data[result.size++] = '\f'; } break;
                    case 'n':  { advance(&input); result.data[result.size++] = '\n'; } break;
                    case 'r':  { advance(&input); result.data[result.size++] = '\r'; } break;
                    case 't':  { advance(&input); result.data[result.size++] = '\t'; } break;

                    case 'u':
                    {
                        advance(&input);
                        u32 codepoint = 0;
                        for (u32 idx = 0; idx < 4; ++idx)
                        {
                            if (is_hex_digit(input.data[0]))
                            {
                                codepoint |= parse_half_hex_byte(input.data[0]) << (12 - idx * 4);
                            }
                            else
                            {
                                // TODO(michiel): Errors
                                input.size = 0;
                                result.size = 0;
                                break;
                            }
                            advance(&input);
                        }

                        // TODO(michiel): Is this normal or a special roon case?
                        if ((codepoint & 0xFF) == codepoint)
                        {
                            result.data[result.size++] = codepoint & 0xFF;
                        }
                        else
                        {
                            result.size += codepoint_to_utf8(codepoint, result.data);
                        }
                    } break;

                    case 'U':
                    {
                        advance(&input);
                        u32 codepoint = 0;
                        for (u32 idx = 0; idx < 8; ++idx)
                        {
                            if (is_hex_digit(input.data[0]))
                            {
                                codepoint |= parse_half_hex_byte(input.data[0]) << (28 - idx * 4);
                            }
                            else
                            {
                                // TODO(michiel): Errors
                                input.size = 0;
                                result.size = 0;
                                break;
                            }
                            advance(&input);
                        }

                        // TODO(michiel): Is this normal or a special roon case?
                        if ((codepoint & 0xFF) == codepoint)
                        {
                            result.data[result.size++] = codepoint & 0xFF;
                        }
                        else
                        {
                            result.size += codepoint_to_utf8(codepoint, result.data);
                        }
                    } break;

                    default:
                    {
                        // TODO(michiel): Errors
                        input.size = 0;
                        result.size = 0;
                    } break;
                }
            }
        }
        else
        {
            result.data[result.size++] = input.data[0];
            advance(&input);
        }
    }

    return result;
}

internal JsonArrayIter
json_iterate_array(String input)
{
    JsonArrayIter result = {};
    if (input.data && (input.data[0] == '['))
    {
        result.string = input;
        advance(&result.string);
    }
    return result;
}

internal b32
is_valid(JsonArrayIter *iter)
{
    return (iter->string.size != 0);
}

internal void
json_next_array(JsonArrayIter *iter)
{
    iter->string = json_skip_value(iter->string);
    strip_whitespace(&iter->string);
    if (iter->string.size && (iter->string.data[0] == ']'))
    {
        iter->string.size = 0;
        iter->string.data = 0;
    }
    else if (iter->string.size && (iter->string.data[0] == ','))
    {
        advance(&iter->string);
        strip_whitespace(&iter->string);
    }
}

internal b32
json_find(String json, String key, JsonValueKind kind, String *subString)
{
    b32 result = false;
    strip_whitespace(&json);

    if (json.size && json.data && (json.data[0] == '{'))
    {
        advance(&json);
        strip_whitespace(&json);

        while (json.size)
        {
            if (json.data[0] == '}')
            {
                break;
            }

            String foundKey = json_parse_string(json);
            b32 found = foundKey == key;

            json.size -= foundKey.size + 2; // NOTE(michiel): +2 for the ending quote "
            json.data += foundKey.size + 2;
            strip_whitespace(&json);

            if (json.size && json.data[0] == ':')
            {
                advance(&json);
            }
            else
            {
                // TODO(michiel): Errors
            }
            strip_whitespace(&json);

            if (json_is_object(json))
            {
                if (found && (kind == JsonValue_Object))
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_object(json);
                }
            }
            else if (json_is_array(json))
            {
                if (found && (kind == JsonValue_Array))
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_array(json);
                }
            }
            else if (json_is_string(json))
            {
                if (found && (kind == JsonValue_String))
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_string(json);
                }
            }
            else if (json_is_number(json))
            {
                if (found && (kind == JsonValue_Number))
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_number(json);
                }
            }
            else if (json_is_constant(json))
            {
                if (found && (kind == JsonValue_Constant))
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_constant(json);
                }
            }
            else
            {
                // NOTE(michiel): Unknown...
                advance(&json);
            }

            strip_whitespace(&json);
            if (json.size && (json.data[0] == ','))
            {
                advance(&json);
                strip_whitespace(&json);
            }
        }
    }

    return result;
}

internal b32
json_find_first(String json, JsonValueKind kind, String *foundKey, String *subString)
{
    b32 result = false;
    strip_whitespace(&json);

    if (json.size && json.data && (json.data[0] == '{'))
    {
        advance(&json);
        strip_whitespace(&json);

        while (json.size)
        {
            if (json.data[0] == '}')
            {
                break;
            }

            *foundKey = json_parse_string(json);
            json.size -= foundKey->size + 2; // NOTE(michiel): +2 for the ending quote "
            json.data += foundKey->size + 2;
            strip_whitespace(&json);

            if (json.size && json.data[0] == ':')
            {
                advance(&json);
            }
            else
            {
                // TODO(michiel): Errors
            }
            strip_whitespace(&json);

            if (json_is_object(json))
            {
                if (kind == JsonValue_Object)
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_object(json);
                }
            }
            else if (json_is_array(json))
            {
                if (kind == JsonValue_Array)
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_array(json);
                }
            }
            else if (json_is_string(json))
            {
                if (kind == JsonValue_String)
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_string(json);
                }
            }
            else if (json_is_number(json))
            {
                if (kind == JsonValue_Number)
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_number(json);
                }
            }
            else if (json_is_constant(json))
            {
                if (kind == JsonValue_Constant)
                {
                    *subString = json;
                    result = true;
                    break;
                }
                else
                {
                    json = json_skip_constant(json);
                }
            }
            else
            {
                // NOTE(michiel): Unknown...
                advance(&json);
            }

            strip_whitespace(&json);
            if (json.size && (json.data[0] == ','))
            {
                advance(&json);
                strip_whitespace(&json);
            }
        }
    }

    return result;
}

//
// Full parser
//

#if JSON_FULL_PARSING

internal void
advance(JsonParser *parser)
{
    advance(&parser->scanner);
}

internal void
strip_front_whitespace(JsonParser *parser)
{
    strip_whitespace(&parser->scanner);
}

internal u32
json_parse_digits(JsonParser *parser)
{
    u32 result = 0;
    while (parser->scanner.size &&
           is_digit(parser->scanner.data[0]))
    {
        advance(parser);
        ++result;
    }
    return result;
}

internal b32
json_parse_number(JsonParser *parser, String *number)
{
    b32 result = true;
    i_expect((parser->scanner.data[0] == '-') ||
             is_digit(parser->scanner.data[0]));

    number->size = 0;
    number->data = parser->scanner.data;
    if (parser->scanner.data[0] == '-')
    {
        advance(parser);
        ++number->size;
    }

    if (parser->scanner.size &&
        is_digit(parser->scanner.data[0]))
    {
        if (parser->scanner.data[0] == '0')
        {
            advance(parser);
            ++number->size;
        }
        else
        {
            number->size += json_parse_digits(parser);
        }

        if (parser->scanner.data[0] == '.')
        {
            advance(parser);
            ++number->size;

            if (parser->scanner.size &&
                is_digit(parser->scanner.data[0]))
            {
                number->size += json_parse_digits(parser);
            }
            else
            {
                fprintf(stderr, "Invalid number specified, no digits after the fraction dot (%.*s).\n",
                        safe_truncate_to_u32(number->size + 1), number->data);
                result = false;
            }
        }

        if ((parser->scanner.data[0] == 'e') ||
            (parser->scanner.data[0] == 'E'))
        {
            advance(parser);
            ++number->size;

            if ((parser->scanner.data[0] == '+') ||
                (parser->scanner.data[0] == '-'))
            {
                advance(parser);
                ++number->size;
            }

            if (parser->scanner.size &&
                is_digit(parser->scanner.data[0]))
            {
                number->size += json_parse_digits(parser);
            }
            else
            {
                fprintf(stderr, "Invalid number specified, no digits after the exponent e (%.*s).\n",
                        safe_truncate_to_u32(number->size + 1), number->data);
                result = false;
            }
        }
    }
    else
    {
        fprintf(stderr, "Invalid number specified, %.*s\n", STR_FMT(parser->scanner));
        result = false;
    }

    if (parser->saveAll && number->size)
    {
        u8 *frontPtr = number->data;
        number->data = arena_allocate_array(&parser->arena, u8, number->size + 1);
        copy(number->size, frontPtr, number->data);
        number->data[number->size] = 0;
    }

    return result;
}

internal b32
json_parse_string(JsonParser *parser, String *string)
{
    b32 result = true;
    i_expect(parser->scanner.data[0] == '"');

    advance(parser);
    string->size = 0;
    string->data = parser->scanner.data;
    while (parser->scanner.size &&
           (parser->scanner.data[0] != '"'))
    {
        if (parser->scanner.data[0] == '\\')
        {
            advance(parser);
            ++string->size;
            if ((parser->scanner.data[0] == '"') ||
                (parser->scanner.data[0] == '\\') ||
                (parser->scanner.data[0] == '/') ||
                (parser->scanner.data[0] == 'b') ||
                (parser->scanner.data[0] == 'f') ||
                (parser->scanner.data[0] == 'n') ||
                (parser->scanner.data[0] == 'r') ||
                (parser->scanner.data[0] == 't'))
            {
                advance(parser);
                ++string->size;
            }
            else if (parser->scanner.data[0] == 'u')
            {
                advance(parser);
                ++string->size;
                if (parser->scanner.size >= 4)
                {
                    for (u32 i = 0; i < 4; ++i)
                    {
                        if (is_hex_digit(parser->scanner.data[0]))
                        {
                            advance(parser);
                            ++string->size;
                        }
                        else
                        {
                            fprintf(stderr, "Invalid string specified, not a hex digit after a \\u escape (%c)\n",
                                    parser->scanner.data[0]);
                            result = false;
                            break;
                        }
                    }
                }
                else
                {
                    fprintf(stderr, "Invalid string specified, not enough hex digits after a \\u escape\n");
                    result = false;
                }
            }
            else
            {
                fprintf(stderr, "Invalid string specified, unknown escape sequence (%.*s).\n",
                        2, parser->scanner.data - 1);
                result = false;
            }
        }
        else
        {
            advance(parser);
            ++string->size;
        }
    }

    if (parser->scanner.data[0] == '"')
    {
        advance(parser);
    }
    else
    {
        fprintf(stderr, "Invalid string specified, no closing quote found.\n");
        result = false;
    }

    if (parser->saveAll && string->size)
    {
        u8 *frontPtr = string->data;
        string->data = arena_allocate_array(&parser->arena, u8, string->size + 1);
        copy(string->size, frontPtr, string->data);
        string->data[string->size] = 0;
    }

    return result;
}

internal b32
json_parse_array(JsonParser *parser, JsonArray *array)
{
    b32 result = true;
    i_expect(parser->scanner.data[0] == '[');

    advance(parser);
    strip_front_whitespace(parser);

    // TODO(michiel): Use the suballocator to control this memory maybe?
    JsonValue *values = 0;

    while (parser->scanner.size &&
           (parser->scanner.data[0] != ']'))
    {
        JsonValue value = {};
        if (json_parse_value(parser, &value))
        {
            buf_push(values, value);
        }
        else
        {
            fprintf(stderr, "Invalid array specified, invalid value.\n");
        }

        strip_front_whitespace(parser);
        if (parser->scanner.data[0] == ',')
        {
            advance(parser);
            strip_front_whitespace(parser);
        }
        else
        {
            break;
        }
    }

    if (parser->scanner.data[0] == ']')
    {
        advance(parser);
    }
    else
    {
        buf_free(values);
        fprintf(stderr, "Invalid array specified, no closing bracket found.\n");
        result = false;
    }

    array->valueCount = buf_len(values);
    if (array->valueCount)
    {
        array->values = arena_allocate_array(&parser->arena, JsonValue, array->valueCount);
        copy(array->valueCount * sizeof(JsonValue), values, array->values);
        buf_free(values);
    }

    return result;
}

internal b32
json_parse_object(JsonParser *parser, JsonObject *object)
{
    b32 result = true;
    i_expect(parser->scanner.data[0] == '{');

    advance(parser);
    strip_front_whitespace(parser);

    // TODO(michiel): Use the suballocator to control this memory maybe?
    JsonItem *items = 0;

    while (parser->scanner.size &&
           (parser->scanner.data[0] != '}'))
    {
        JsonItem item = {};
        if (json_parse_string(parser, &item.key))
        {
            strip_front_whitespace(parser);
            if (parser->scanner.size &&
                (parser->scanner.data[0] == ':'))
            {
                advance(parser);
                if (json_parse_value(parser, &item.value))
                {
                    buf_push(items, item);
                }
                else
                {
                    fprintf(stderr, "Invalid object specified, invalid member value.\n");
                }
            }
            else
            {
                fprintf(stderr, "Object key and value not separated by a colon.\n");
                result = false;
                break;
            }
        }
        else
        {
            fprintf(stderr, "Object key string error\n");
            buf_free(items);
            result = false;
            break;
        }

        strip_front_whitespace(parser);
        if (parser->scanner.data[0] == ',')
        {
            advance(parser);
            strip_front_whitespace(parser);
        }
        else
        {
            break;
        }
    }

    if (parser->scanner.data[0] == '}')
    {
        advance(parser);
    }
    else
    {
        fprintf(stderr, "Invalid object specified, no closing brace found.\n");
        buf_free(items);
        result = false;
    }

    object->itemCount = buf_len(items);
    if (object->itemCount)
    {
        object->items = arena_allocate_array(&parser->arena, JsonItem, object->itemCount);
        copy(object->itemCount * sizeof(JsonItem), items, object->items);
    }

    return result;
}

internal b32
json_parse_value(JsonParser *parser, JsonValue *value)
{
    b32 result = false;

    strip_front_whitespace(parser);

    if (parser->scanner.data[0] == '{')
    {
        value->kind = JsonValue_Object;
        result = json_parse_object(parser, &value->object);
    }
    else if (parser->scanner.data[0] == '[')
    {
        value->kind = JsonValue_Array;
        result = json_parse_array(parser, &value->array);
    }
    else if (parser->scanner.data[0] == '"')
    {
        value->kind = JsonValue_String;
        result = json_parse_string(parser, &value->string);
    }
    else if ((parser->scanner.data[0] == '-') ||
             is_digit(parser->scanner.data[0]))
    {
        value->kind = JsonValue_Number;
        result = json_parse_number(parser, &value->number);
    }
    else if (parser->scanner.size >= 4)
    {
        value->kind = JsonValue_Constant;
        String constTest = parser->scanner;
        constTest.size = 5;
        if (constTest == string(5, "false"))
        {
            result = true;
            value->constant = JsonConstant_False;
        }
        else
        {
            constTest.size = 4;
            if (constTest == string(4, "true"))
            {
                result = true;
                value->constant = JsonConstant_True;
            }
            else if (constTest == string(4, "null"))
            {
                result = true;
                value->constant = JsonConstant_Null;
            }
            else
            {
                fprintf(stderr, "Unknown json value at:\n%.*s\n", STR_FMT(parser->scanner));
                constTest.size = 1;
            }
        }
        parser->scanner.size -= constTest.size;
        parser->scanner.data += constTest.size;
    }
    else
    {
        fprintf(stderr, "Unknown json value at:\n%.*s\n", STR_FMT(parser->scanner));
    }

    if (!result)
    {
        value->kind = JsonValue_None;
    }

    return result;
}

internal void
json_dump(JsonValue *value)
{
    switch (value->kind)
    {
        case JsonValue_Object: {
            fprintf(stdout, "{");
            for (u32 itemIndex = 0; itemIndex < value->object.itemCount; ++itemIndex)
            {
                if (itemIndex)
                {
                    fprintf(stdout, ", ");
                }

                JsonItem *item = value->object.items + itemIndex;
                fprintf(stdout, "\"%.*s\": ", STR_FMT(item->key));
                json_dump(&item->value);
            }
            fprintf(stdout, "}");
        } break;

        case JsonValue_Array: {
            fprintf(stdout, "[");
            for (u32 valueIndex = 0; valueIndex < value->array.valueCount; ++valueIndex)
            {
                if (valueIndex)
                {
                    fprintf(stdout, ",");
                }

                json_dump(value->array.values + valueIndex);
            }
            fprintf(stdout, "]");
        } break;

        case JsonValue_String: {
            fprintf(stdout, "\"%.*s\"", STR_FMT(value->string));
        } break;

        case JsonValue_Number: {
            fprintf(stdout, "%.*s", STR_FMT(value->number));
        } break;

        case JsonValue_Constant: {
            switch (value->constant)
            {
                case JsonConstant_Null: {
                    fprintf(stdout, "null");
                } break;

                case JsonConstant_True: {
                    fprintf(stdout, "true");
                } break;

                case JsonConstant_False: {
                    fprintf(stdout, "false");
                } break;

                INVALID_DEFAULT_CASE;
            }
        } break;

        INVALID_DEFAULT_CASE;
    }
}

internal void
json_print(JsonValue *value, u32 indent = 0)
{
    char *spaces = "                                                                                                    ";

    switch (value->kind)
    {
        case JsonValue_Object: {
            fprintf(stdout, "%.*s<object>\n", indent * 2, spaces);
            ++indent;
            for (u32 itemIndex = 0; itemIndex < value->object.itemCount; ++itemIndex)
            {
                JsonItem *item = value->object.items + itemIndex;
                fprintf(stdout, "%.*s<item key=\"%.*s\">\n", indent * 2, spaces, STR_FMT(item->key));
                ++indent;
                json_print(&item->value, indent);
                --indent;
                fprintf(stdout, "%.*s</item>\n", indent * 2, spaces);
            }
            --indent;
            fprintf(stdout, "%.*s</object>\n", indent * 2, spaces);
        } break;

        case JsonValue_Array: {
            fprintf(stdout, "%.*s<array>\n", indent * 2, spaces);
            ++indent;
            for (u32 valueIndex = 0; valueIndex < value->array.valueCount; ++valueIndex)
            {
                fprintf(stdout, "%.*s<value>\n", indent * 2, spaces);
                ++indent;
                json_print(value->array.values + valueIndex, indent);
                --indent;
                fprintf(stdout, "%.*s</value>\n", indent * 2, spaces);
            }
            --indent;
            fprintf(stdout, "%.*s</array>\n", indent * 2, spaces);
        } break;

        case JsonValue_String: {
            fprintf(stdout, "%.*s<string>\"%.*s\"</string>\n", indent * 2, spaces, STR_FMT(value->string));
        } break;

        case JsonValue_Number: {
            fprintf(stdout, "%.*s<number>%.*s</number>\n", indent * 2, spaces, STR_FMT(value->number));
        } break;

        case JsonValue_Constant: {
            switch (value->constant)
            {
                case JsonConstant_Null: {
                    fprintf(stdout, "%.*s<const null/>\n", indent * 2, spaces);
                } break;

                case JsonConstant_True: {
                    fprintf(stdout, "%.*s<const true/>\n", indent * 2, spaces);
                } break;

                case JsonConstant_False: {
                    fprintf(stdout, "%.*s<const false/>\n", indent * 2, spaces);
                } break;

                INVALID_DEFAULT_CASE;
            }
        } break;

        INVALID_DEFAULT_CASE;
    }
}
#endif
