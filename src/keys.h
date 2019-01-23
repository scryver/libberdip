/*
'a'-'z' for chars
'0' - '9' for nums
phys_w/s/a/d (game_up/down/left/right)
ctrl/shift/alt/super in modifiers
lastInput has text with shift modifiers
*/

typedef enum Keys
{
    Key_None = 0,
    /* 1 - 7 */
    Key_Backspace = 8,
    Key_Tab = '\t',      // 9
    Key_Enter = '\n',    // 10
    /* 11 - 26 */
    Key_Escape = 27,
    /* 28 - 31 */
    Key_Space = ' ',     // 32
    Key_Bang = '!',      // 33
    Key_DoubleQuote = '"', // 34
    Key_Hash = '#',
    Key_Dollar = '$',
    Key_Percent = '%',
    Key_Ampersand = '&',
    Key_Quote = '\'',    // 39
    Key_LeftParen = '(',
    Key_RightParen = ')',
    Key_Asterisk = '*',
    Key_Plus = '+',
    Key_Comma = ',',     // 44
    Key_Minus = '-',     // 45
    Key_Dot = '.',       // 46
    Key_Slash = '/',     // 47
    Key_0 = '0',         // 48
    Key_1 = '1',         // 49
    Key_2 = '2',         // 50
    Key_3 = '3',         // 51
    Key_4 = '4',         // 52
    Key_5 = '5',         // 53
    Key_6 = '6',         // 54
    Key_7 = '7',         // 55
    Key_8 = '8',         // 56
    Key_9 = '9',         // 57
    Key_Colon = ':',
    Key_SemiColon = ';', // 59
    Key_Less = '<',
    Key_Equal = '=',     // 61
    Key_Greater = '>',
    Key_Question = '?',
    Key_At = '@',
    /* 65 - 90 (A-Z) */
    Key_LeftBracket = '[',  // 91
    Key_BackSlash = '\\',   // 92
        Key_RightBracket = ']', // 93
    Key_RoofIcon = '^',     // :) Call it what you like
    Key_Underscore = '_',
    Key_Grave = '`',        // 96
        Key_A = 'a',            // 97
    Key_B = 'b',            // 98
    Key_C = 'c',            // 99
    Key_D = 'd',            // 100
    Key_E = 'e',            // 101
    Key_F = 'f',            // 102
    Key_G = 'g',            // 103
    Key_H = 'h',            // 104
    Key_I = 'i',            // 105
    Key_J = 'j',            // 106
    Key_K = 'k',            // 107
    Key_L = 'l',            // 108
    Key_M = 'm',            // 109
    Key_N = 'n',            // 110
    Key_O = 'o',            // 111
    Key_P = 'p',            // 112
    Key_Q = 'q',            // 113
    Key_R = 'r',            // 114
    Key_S = 's',            // 115
    Key_T = 't',            // 116
    Key_U = 'u',            // 117
    Key_V = 'v',            // 118
    Key_W = 'w',            // 119
    Key_X = 'x',            // 120
    Key_Y = 'y',            // 121
    Key_Z = 'z',            // 122
    Key_LeftBrace = '{',
    Key_Pipe = '|',
    Key_RightBrace = '}',
    Key_Tilde = '~',
    Key_Delete = 127,
    // NOTE(michiel): Everything from 128 forward is fine for own use
    
    Key_Home = 128,
    Key_End,
    Key_PageUp,
    Key_PageDown,
    Key_Insert,
    Key_CapsLock,
    
    Key_NumLock,
    Key_ScrollLock,
    Key_PrintScreen,
    
    Key_OS,
    Key_Menu,
    
    Key_Up,
    Key_Down,
    Key_Left,
    Key_Right,
    
    Key_GameUp,     // w   - on querty keyboard
    Key_GameDown,   // s
    Key_GameLeft,   // a
    Key_GameRight,  // d
    Key_GameLeftUp, // q
    Key_GameRightUp,// e
    
    Key_F1,
    Key_F2,
    Key_F3,
    Key_F4,
    Key_F5,
    Key_F6,
    Key_F7,
    Key_F8,
    Key_F9,
    Key_F10,
    Key_F11,
    Key_F12,
    
    Key_Num0,
    Key_Num1,
    Key_Num2,
    Key_Num3,
    Key_Num4,
    Key_Num5,
    Key_Num6,
    Key_Num7,
    Key_Num8,
    Key_Num9,
    Key_NumDivide,
    Key_NumMultiply,
    Key_NumAdd,
    Key_NumSubtract,
    Key_NumDot,
    Key_NumEnter,
    
    } Keys;
