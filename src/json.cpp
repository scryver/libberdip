internal void
advance(JsonParser *parser)
{
    --parser->scanner.size;
    ++parser->scanner.data;
}

internal void
strip_front_whitespace(JsonParser *parser)
{
    while (parser->scanner.size && 
           is_whitespace(parser->scanner.data[0]))
    {
        advance(parser);
    }
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
        JsonValue value = json_parse_value(parser);
        buf_push(values, value);
        
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
                item.value = json_parse_value(parser);
            }
            else
            {
                fprintf(stderr, "Object key and value not separated by a colon.\n");
                result = false;
                break;
            }
            buf_push(items, item);
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

internal JsonValue
json_parse_value(JsonParser *parser)
{
    JsonValue result = {};
    
    strip_front_whitespace(parser);
    
    if (parser->scanner.data[0] == '{')
    {
        if (json_parse_object(parser, &result.object))
        {
            result.kind = JsonValue_Object;
        }
    }
    else if (parser->scanner.data[0] == '[')
    {
        if (json_parse_array(parser, &result.array))
        {
            result.kind = JsonValue_Array;
        }
    }
    else if (parser->scanner.data[0] == '"')
    {
        if (json_parse_string(parser, &result.string))
        {
            result.kind = JsonValue_String;
        }
    }
    else if ((parser->scanner.data[0] == '-') ||
             is_digit(parser->scanner.data[0]))
    {
        if (json_parse_number(parser, &result.number))
        {
            result.kind = JsonValue_Number;
        }
    }
    else if (parser->scanner.size >= 4)
    {
        result.kind = JsonValue_Constant;
        String constTest = parser->scanner;
        constTest.size = 5;
        if (constTest == string(5, "false"))
        {
            result.constant = JsonConstant_False;
        }
        else
        {
            constTest.size = 4;
            if (constTest == string(4, "true"))
            {
                result.constant = JsonConstant_True;
            }
            else if (constTest == string(4, "null"))
            {
                result.constant = JsonConstant_Null;
            }
            else
            {
                result.kind = JsonValue_None;
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
