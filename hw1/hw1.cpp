#include <array>
#include <cstring>
#include <iostream>

#include "tokens.hpp"

using namespace std;

int main()
{
    int token;
    // clang-format off
    array<const char *, ENUM_SIZE> token_to_string = {"", "VOID", "INT", "BYTE", "B", "BOOL", "AND", "OR", "NOT",
                                                      "TRUE", "FALSE", "RETURN", "IF", "ELSE", "WHILE", "BREAK",
                                                      "CONTINUE", "SC", "COMMA", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
                                                      "ASSIGN", "RELOP", "BINOP", "COMMENT", "ID", "NUM", "STRING",
                                                      "ERROR", "UNRECOGNIZED_CHAR", "UNCLOSED_STRING", "HEX_LENGTH_0",
                                                      "HEX_LENGTH_1", "ESCAPE_ERROR"};
    // clang-format on
    while ((token = yylex()))
    {
        switch (token)
        {
        case COMMENT:
            cout << yylineno << " " << token_to_string[token] << " //" << endl;
            break;
        case STRING: {
            string buffer;
            string input_text(yytext);
            for (unsigned int i = 1; i < input_text.length() - 1; ++i)
            {
                if (input_text[i] == '\\')
                {
                    if (i == input_text.length() - 2)
                    {
                        cout << "Error unclosed string" << endl;
                        exit(0);
                    }
                    i++;
                    switch (input_text[i])
                    {
                    case '\\':
                        buffer += '\\';
                        break;
                    case 'x': {
                        // check both characters are legal hex characters
                        int c1 = (input_text.substr(i + 1, 1)).c_str()[0];
                        int c2 = (input_text.substr(i + 2, 1)).c_str()[0];
                        if (!((('0' <= c1 && c1 <= '7')) &&
                              (('0' <= c2 && c2 <= '9') || ('A' <= c2 && c2 <= 'F') || ('a' <= c2 && c2 <= 'f'))))
                        {
                            cout << "Error undefined escape sequence " << input_text.substr(i, 3) << endl;
                            exit(0);
                        }
                        buffer.push_back((char)std::stoi(input_text.substr(i + 1, 2), nullptr, 16));
                        i += 2;
                        break;
                    }
                    case 't':
                        buffer += '\t';
                        break;
                    case 'n':
                        buffer += '\n';
                        break;
                    case '0':
                        buffer.push_back('\0');
                        // buffer += '0';
                        break;
                    case 'r':
                        buffer += '\r';
                        break;
                    case '"':
                        buffer += '"';
                        break;
                    default:
                        cout << "Error undefined escape sequence " << input_text[i] << endl;
                        exit(0);
                    }
                }
                else
                {
                    buffer += input_text[i];
                }
            }
            cout << yylineno << " " << token_to_string[token] << " " << buffer.c_str() << endl;
            break;
        }
        case UNRECOGNIZED_CHAR:
            cout << "Error " << yytext[strlen(yytext) - 1] << endl;
            exit(0);
        case UNCLOSED_STRING:
            cout << "Error unclosed string" << endl;
            exit(0);
        case ESCAPE_ERROR: {
            string input_text = yytext;
            for (unsigned int i = 1; i < input_text.length() - 1; ++i)
            {
                if (input_text[i] == '\\')
                {
                    i++;
                    if (input_text[i] != 'n' || input_text[i] != 't' || input_text[i] != '0' || input_text[i] != 'r' ||
                        input_text[i] != '\\' || input_text[i] != '\"')
                    {
                        cout << "Error undefined escape sequence " << input_text[i] << endl;
                        exit(0);
                    }
                }
            }
            exit(0);
        }
        case HEX_LENGTH_0:
            cout << "Error undefined escape sequence x" << endl;
            exit(0);
        case HEX_LENGTH_1:
            cout << "Error undefined escape sequence x" << yytext[strlen(yytext) - 2] << endl;
            exit(0);
        case ERROR:
            cout << "Error " << yytext << endl;
            exit(0);
        default:
            cout << yylineno << " " << token_to_string[token] << " " << yytext << endl;
            break;
        }
    }
    return 0;
}
